#pragma once

// must come before Textures.h because Textures.h doesnt include memory even though it uses it. FOR SHAME.
#include <memory>

#include "mq/api/Textures.h"
#include "imgui/imgui.h"

namespace Ui {

class IndicatorBar
{
public:
    IndicatorBar(const std::string& textureFrame, const std::string& textureBar);
    void Render(const char* id, ImDrawList* dl, const ImVec2& center_pos, const ImVec2& frameSize,
                const ImVec2& barSize, float percent, float dt);

private:
    mq::MQTexturePtr m_pTextureFrame;
    mq::MQTexturePtr m_pTextureBar;
};

} // namespace Ui
