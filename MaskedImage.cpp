#include "MaskedImage.h"

#include "mq/Plugin.h"
#include "spdlog/spdlog.h"

#include <d3d9.h>
#include <d3dcompiler.h>
#pragma comment(lib, "d3dcompiler")

namespace Ui {

class PixelShader
{
public:
    PixelShader(const char* source)
        : m_shaderSource(source)
    {
    }

    ~PixelShader()
    {
    }

    bool InitShader()
    {
        if (!gpD3D9Device)
            return false;
        if (m_pixelShader)
            return true;

        wil::com_ptr<ID3DBlob> code;
        wil::com_ptr<ID3DBlob> err;

        HRESULT hr = D3DCompile(m_shaderSource, strlen(m_shaderSource), nullptr, nullptr, nullptr,
            "main", "ps_2_0", 0, 0, &code, &err);

        if (FAILED(hr))
        {
            if (err)
            {
                //SPDLOG_ERROR("MaskedImage: shader compile error: {}",
                //    static_cast<const char*>(err->GetBufferPointer()));
                DebugSpewAlways("MaskedImage: shader compiler error: %s",
                    static_cast<const char*>(err->GetBufferPointer()));
            }
            return false;
        }

        hr = gpD3D9Device->CreatePixelShader(static_cast<const DWORD*>(code->GetBufferPointer()), &m_pixelShader);
        return SUCCEEDED(hr);
    }

    void Release()
    {
        m_pixelShader.reset();
    }

    IDirect3DPixelShader9* Get() const { return m_pixelShader.get(); }

private:
    const char* m_shaderSource;
    wil::com_ptr<IDirect3DPixelShader9> m_pixelShader;
};

static constexpr const char* MASK_SHADER_SOURCE = R"(

float4 maskRange  : register(c0);
float4 maskMargin : register(c1);

sampler sampler0 : register(s0);
sampler maskSampler : register(s1);

struct PS_INPUT
{
    float4 pos : POSITION;
    float4 col : COLOR;
    float2 uv  : TEXCOORD0;
};


float4 main(PS_INPUT input) : COLOR
{
    // Sample source texture with vertex color
    float4 src = tex2D(sampler0, input.uv) * input.col;

    // Nine-slice UV remapping for mask texture.
    // Each axis is split into three regions: low-border, center, high-border.
    // Borders map 1:1 in UV space; center stretches to fill remaining space.
    // When a margin is 0, that border region has zero width and is skipped.
    float2 t = input.uv;

    // --- X axis ---
    float mL = maskMargin.x;   // left margin (UV)
    float mR = maskMargin.z;   // right margin (UV)

    float inLeftX   = 1.0 - step(mL, t.x);              // 1 if t.x < mL
    float inRightX  = step(1.0 - mR, t.x);              // 1 if t.x >= 1-mR
    float inCenterX = 1.0 - max(inLeftX, inRightX);

    float leftUVx   = maskRange.x + t.x;                // 1:1 from range start
    float rightUVx  = maskRange.z - (1.0 - t.x);        // 1:1 from range end
    float cWidthX   = max(1.0 - mL - mR, 0.0001);       // input center width
    float mCWidthX  = maskRange.z - maskRange.x - mL - mR; // mask center width
    float centerUVx = maskRange.x + mL + ((t.x - mL) / cWidthX) * mCWidthX;

    float maskU = inLeftX * leftUVx + inRightX * rightUVx + inCenterX * centerUVx;

    // --- Y axis ---
    float mT = maskMargin.y;   // top margin (UV)
    float mB = maskMargin.w;   // bottom margin (UV)

    float inTopY     = 1.0 - step(mT, t.y);
    float inBottomY  = step(1.0 - mB, t.y);
    float inCenterY  = 1.0 - max(inTopY, inBottomY);

    float topUVy     = maskRange.y + t.y;
    float bottomUVy  = maskRange.w - (1.0 - t.y);
    float cWidthY    = max(1.0 - mT - mB, 0.0001);
    float mCWidthY   = maskRange.w - maskRange.y - mT - mB;
    float centerUVy  = maskRange.y + mT + ((t.y - mT) / cWidthY) * mCWidthY;

    float maskV = inTopY * topUVy + inBottomY * bottomUVy + inCenterY * centerUVy;

    // Sample mask and apply alpha
    float4 mask = tex2D(maskSampler, float2(maskU, maskV));

    return src * mask.a;
}
)";

static PixelShader s_maskShader(MASK_SHADER_SOURCE);

struct MaskShaderData
{
    ImTextureID maskTexId;
    float maskRange[4];
    float margins[4];
};

struct SavedShaderState
{
    wil::com_ptr<IDirect3DPixelShader9> savedPixelShader;
    wil::com_ptr<IDirect3DVertexShader9> savedVertexShader;
    wil::com_ptr<IDirect3DBaseTexture9> savedTexture1;
};

static std::vector<SavedShaderState> s_savedMaskState;

static void PushAlphaMaskState(const ImDrawList*, const ImDrawCmd* cmd)
{
    IDirect3DDevice9* pDevice = gpD3D9Device;

    const MaskShaderData* data = static_cast<const MaskShaderData*>(cmd->UserCallbackData);
    SavedShaderState& savedState = s_savedMaskState.emplace_back();

    if (!s_maskShader.InitShader())
        return;

    // Backup state
    pDevice->GetPixelShader(&savedState.savedPixelShader);
    pDevice->GetVertexShader(&savedState.savedVertexShader);
    pDevice->GetTexture(1, &savedState.savedTexture1);

    // Set our texture, shader, and constants
    pDevice->SetTexture(1, static_cast<IDirect3DTexture9*>(data->maskTexId));
    pDevice->SetPixelShaderConstantF(0, data->maskRange, 1);
    pDevice->SetPixelShaderConstantF(1, data->margins, 1);
    pDevice->SetPixelShader(s_maskShader.Get());
    pDevice->SetVertexShader(nullptr);
}

static void PopAlphaMaskState(const ImDrawList*, const ImDrawCmd*)
{
    IDirect3DDevice9* pDevice = gpD3D9Device;
    IM_ASSERT(!s_savedMaskState.empty());

    SavedShaderState& savedState = s_savedMaskState.back();

    if (pDevice)
    {
        pDevice->SetPixelShader(savedState.savedPixelShader.get());
        pDevice->SetVertexShader(savedState.savedVertexShader.get());
        pDevice->SetTexture(1, savedState.savedTexture1.get());
    }

    s_savedMaskState.pop_back();
}

// Apply a clipping alpha mask to the current clip rect. The mask texture should be a grayscale image where
// alpha at 100% means fully visible and alpha at 0% means fully clipped. Margins can be
// used to specify the size of the unscaled border area for nine-slice scaling of the mask.
static void PushAlphaMask(ImDrawList* dl, ImTextureID maskTexture, const ImVec2& uv_min, const ImVec2& uv_max,
    const ImVec4& margins = ImVec4(0, 0, 0, 0))
{
    MaskShaderData data
    {
        .maskTexId = maskTexture,
        .maskRange = { uv_min.x, uv_min.y, uv_max.x, uv_max.y },
        .margins = { margins.x, margins.y, margins.z, margins.w }
    };

    dl->AddCallback(PushAlphaMaskState, &data, sizeof(data));
}

static void PopAlphaMask(ImDrawList* dl)
{
    dl->AddCallback(PopAlphaMaskState, nullptr, 0);
}

// --- Nine-slice shader ----------------------------------------------------------

// Render an image using nine-slice scaling technique
static constexpr const char* NINE_SLICE_SHADER = R"(

float4 patchRange  : register(c0);
float4 patchMargin : register(c1);

sampler sampler0 : register(s0);

struct PS_INPUT
{
    float4 pos : POSITION;
    float4 col : COLOR;
    float2 uv  : TEXCOORD0;
};

float4 main(PS_INPUT input) : COLOR
{
    // Nine-slice UV remapping for mask texture.
    // Each axis is split into three regions: low-border, center, high-border.
    // Borders map 1:1 in UV space; center stretches to fill remaining space.
    // When a margin is 0, that border region has zero width and is skipped.
    float2 t = input.uv;

    // --- X axis ---
    float mL = patchMargin.x;   // left margin (UV)
    float mR = patchMargin.z;   // right margin (UV)

    float inLeftX   = 1.0 - step(mL, t.x);              // 1 if t.x < mL
    float inRightX  = step(1.0 - mR, t.x);              // 1 if t.x >= 1-mR
    float inCenterX = 1.0 - max(inLeftX, inRightX);

    float leftUVx   = patchRange.x + t.x;                // 1:1 from range start
    float rightUVx  = patchRange.z - (1.0 - t.x);        // 1:1 from range end
    float cWidthX   = max(1.0 - mL - mR, 0.0001);        // input center width
    float mCWidthX  = patchRange.z - patchRange.x - mL - mR; // mask center width
    float centerUVx = patchRange.x + mL + ((t.x - mL) / cWidthX) * mCWidthX;

    float maskU = inLeftX * leftUVx + inRightX * rightUVx + inCenterX * centerUVx;

    // --- Y axis ---
    float mT = patchMargin.y;   // top margin (UV)
    float mB = patchMargin.w;   // bottom margin (UV)

    float inTopY     = 1.0 - step(mT, t.y);
    float inBottomY  = step(1.0 - mB, t.y);
    float inCenterY  = 1.0 - max(inTopY, inBottomY);

    float topUVy     = patchRange.y + t.y;
    float bottomUVy  = patchRange.w - (1.0 - t.y);
    float cWidthY    = max(1.0 - mT - mB, 0.0001);
    float mCWidthY   = patchRange.w - patchRange.y - mT - mB;
    float centerUVy  = patchRange.y + mT + ((t.y - mT) / cWidthY) * mCWidthY;

    float maskV = inTopY * topUVy + inBottomY * bottomUVy + inCenterY * centerUVy;

    // Sample source texture with vertex color
    float4 src = tex2D(sampler0, float2(maskU, maskV)) * input.col;
    return src;
}
)";
static PixelShader s_nineSliceShader(NINE_SLICE_SHADER);

static std::vector<SavedShaderState> s_saved9SliceState;


static void PushNineSliceState(const ImDrawList*, const ImDrawCmd* cmd)
{
    IDirect3DDevice9* pDevice = gpD3D9Device;

    const MaskShaderData* data = static_cast<const MaskShaderData*>(cmd->UserCallbackData);
    SavedShaderState& savedState = s_saved9SliceState.emplace_back();

    if (!s_nineSliceShader.InitShader())
        return;

    // Backup state
    pDevice->GetPixelShader(&savedState.savedPixelShader);
    pDevice->GetVertexShader(&savedState.savedVertexShader);

    // Set our parameters
    pDevice->SetPixelShaderConstantF(0, data->maskRange, 1);
    pDevice->SetPixelShaderConstantF(1, data->margins, 1);
    pDevice->SetPixelShader(s_nineSliceShader.Get());
    pDevice->SetVertexShader(nullptr);
}

static void PopNineSliceState(const ImDrawList*, const ImDrawCmd*)
{
    IDirect3DDevice9* pDevice = gpD3D9Device;
    IM_ASSERT(!s_saved9SliceState.empty());

    SavedShaderState& savedState = s_saved9SliceState.back();

    if (pDevice)
    {
        pDevice->SetPixelShader(savedState.savedPixelShader.get());
        pDevice->SetVertexShader(savedState.savedVertexShader.get());
    }

    s_saved9SliceState.pop_back();
}

// Push state to apply a nine-slice scaling shader to subsequent draw calls. The shader remaps UVs according to the
// specified margins and patch range.
static void PushNineSlice(ImDrawList* dl, const ImVec2& uv_min, const ImVec2& uv_max,
    const ImVec4& margins = ImVec4(0, 0, 0, 0))
{
    MaskShaderData data
    {
        .maskRange = { uv_min.x, uv_min.y, uv_max.x, uv_max.y },
        .margins = { margins.x, margins.y, margins.z, margins.w }
    };

    dl->AddCallback(PushNineSliceState, &data, sizeof(data));
}

static void PopNineSlice(ImDrawList* dl)
{
    dl->AddCallback(PopNineSliceState, nullptr, 0);
}


// --- MaskedImage ----------------------------------------------------------

MaskedImage::MaskedImage(const std::string& sourcePath, const std::string& maskPath)
{
    m_pSource = mq::CreateTexturePtr(sourcePath);
    m_pMask   = mq::CreateTexturePtr(maskPath);
}

bool MaskedImage::IsValid() const
{
    return m_pSource && m_pSource->IsValid()
        && m_pMask   && m_pMask->IsValid();
}

void MaskedImage::Render(ImDrawList* dl, const ImVec2& min, const ImVec2& max, ImU32 tint) const
{
    if (!IsValid())
        return;

    CXSize textureSize = m_pMask->GetTextureSize();
    float margin_u = 17.0f / textureSize.cx;
    float margin_v = 17.0f / textureSize.cy;

    //PushAlphaMask(dl, m_pMask->GetTextureID(), ImVec2(0, 0), ImVec2(0.75f, 0.75f),
    //    ImVec4(margin_u, margin_v, margin_u, margin_v));

    PushNineSlice(dl, ImVec2(0, 0), ImVec2(1.0f, 1.0f), ImVec4(0.25f, 0.25f, 0.25f, 0.25f));

    dl->AddImage(m_pSource->GetTextureID(), min, max, ImVec2(0, 0), ImVec2(1, 1), tint);

    PopNineSlice(dl);
    //PopAlphaMask(dl);
}

void MaskedImage::RenderNineSlice(ImDrawList* dl, const ImVec2& min, const ImVec2& max, const ImVec2& maskSize, const ImVec4& margins, ImU32 tint) const
{
    if (!IsValid())
        return;

    const float destW = max.x - min.x;
    const float destH = max.y - min.y;

    float sx = 1.0f;
    float sy = 1.0f;

    if (margins.x + margins.z > destW && destW > 0.0f)
    {
        sx = destW / (margins.x + margins.z);
    }

    if (margins.y + margins.w > destH && destH > 0.0f)
    {
        sy = destH / (margins.y + margins.w);
    }

    const float left   = margins.x * sx;
    const float top    = margins.y * sy;
    const float right  = margins.z * sx;
    const float bottom = margins.w * sy;

    const ImVec2 srcSize  = m_pSource->GetTextureSize();

    const float srcUvX[4]  = { 0.0f, left / srcSize.x,  1.0f - right  / srcSize.x,  1.0f };
    const float srcUvY[4]  = { 0.0f, top  / srcSize.y,  1.0f - bottom / srcSize.y,  1.0f };
    
    const float maskUvX[4] = {
        0.0f,
        margins.x / maskSize.x,
        (maskSize.x - margins.z) / maskSize.x,
        1.0f
    };

    const float maskUvY[4] = {
        0.0f,
        margins.y / maskSize.y,
        (maskSize.y - margins.w) / maskSize.y,
        1.0f
    };

    const float dX[4] = { min.x, min.x + left,   max.x - right,  max.x };
    const float dY[4] = { min.y, min.y + top,    max.y - bottom, max.y };

    PushAlphaMask(dl, m_pMask->GetTextureID(), ImVec2(0, 0), ImVec2(0.75f, 0.75f), ImVec4(17, 17, 17, 17));

    for (int row = 0; row < 3; ++row)
    {
        for (int col = 0; col < 3; ++col)
        {
            ImVec2 destMin(dX[col],     dY[row]);
            ImVec2 destMax(dX[col + 1], dY[row + 1]);

            if (destMax.x <= destMin.x || destMax.y <= destMin.y)
                continue;

            ImVec2 srcUvMin(srcUvX[col],     srcUvY[row]);
            ImVec2 srcUvMax(srcUvX[col + 1], srcUvY[row + 1]);

            ImVec2 maskUvMin(maskUvX[col],     maskUvY[row]);
            ImVec2 maskUvMax(maskUvX[col + 1], maskUvY[row + 1]);

            dl->AddImage(m_pSource->GetTextureID(), destMin, destMin, srcUvMin, srcUvMax, tint);
        }
    }

    PopAlphaMask(dl);
}

void MaskedImage::RenderMask(ImDrawList* dl, const ImVec2& min, const ImVec2& max, ImU32 tint) const
{
    if (!m_pMask || !m_pMask->IsValid())
        return;

    dl->AddImage(m_pMask->GetTextureID(), min, max, ImVec2(0, 0), ImVec2(1, 1), tint);
}

void MaskedImage::RenderMaskNineSlice(ImDrawList* dl, const ImVec2& min, const ImVec2& max, const ImVec2& maskSize, const ImVec4& margins, ImU32 tint) const
{
    if (!m_pMask || !m_pMask->IsValid())
        return;

    const float destW = max.x - min.x;
    const float destH = max.y - min.y;

    const float sx = (margins.x + margins.z > destW && destW > 0.0f) ? destW / (margins.x + margins.z) : 1.0f;
    const float sy = (margins.y + margins.w > destH && destH > 0.0f) ? destH / (margins.y + margins.w) : 1.0f;

    const float left   = margins.x * sx;
    const float top    = margins.y * sy;
    const float right  = margins.z * sx;
    const float bottom = margins.w * sy;

    const float maskUvX[4] = {
    0.0f,
    margins.x / maskSize.x,
    (maskSize.x - margins.z) / maskSize.x,
    1.0f
    };

    const float maskUvY[4] = {
        0.0f,
        margins.y / maskSize.y,
        (maskSize.y - margins.w) / maskSize.y,
        1.0f
    };

    const float dX[4] = { min.x, min.x + left,  max.x - right,  max.x };
    const float dY[4] = { min.y, min.y + top,   max.y - bottom, max.y };
    const ImU32 colors[3] = { IM_COL32(240,0,0,20), IM_COL32(0,240,0,20) ,IM_COL32(0,0,240,20) };

    for (int row = 0; row < 3; ++row)
    {
        for (int col = 0; col < 3; ++col)
        {
            ImVec2 destMin(dX[col],     dY[row]);
            ImVec2 destMax(dX[col + 1], dY[row + 1]);

            if (destMax.x <= destMin.x || destMax.y <= destMin.y)
                continue;

            dl->AddRectFilled(destMin, destMax, colors[col]);

            dl->AddImage(m_pMask->GetTextureID(), destMin, destMax,
                ImVec2(maskUvX[col], maskUvY[row]), ImVec2(maskUvX[col + 1], maskUvY[row + 1]), tint);
        }
    }
}

void MaskedImage::ReleaseShader()
{
    s_maskShader.Release();
    s_nineSliceShader.Release();
}

} // namespace Ui
