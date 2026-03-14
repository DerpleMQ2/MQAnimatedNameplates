
#include "ConfigVariable.h"
#include "Ui.h"

#include "spdlog/spdlog.h"
#include <fstream>

namespace Ui {

ConfigContainer::ConfigContainer()
{
}

ConfigContainer::ConfigContainer(YAML::Node configNode)
    : m_configNode(configNode)
{
}

void ConfigContainer::Register(ConfigVariableBase* variable)
{
    m_registry.push_back(variable);
}

void ConfigContainer::Update(const ConfigVariableBase& variable)
{
    variable.Store(m_configNode);
    m_dirty = true;
}

void ConfigContainer::SaveConfig(const std::string& fileName)
{
    try
    {
        std::fstream file(fileName, std::ios::out);

        if (!m_configNode.IsNull())
        {
            YAML::Emitter y_out;
            y_out.SetIndent(4);
            y_out.SetFloatPrecision(2);
            y_out.SetDoublePrecision(2);
            y_out << m_configNode;

            file << y_out.c_str();
        }
    }
    catch (const std::exception&)
    {
        SPDLOG_ERROR("Failed to write settings file: {}", fileName);
    }
}

void ConfigContainer::LoadConfig(const std::string& fileName)
{
    try
    {
        m_configNode = YAML::LoadFile(fileName);

        for (ConfigVariableBase* var : m_registry)
        {
            var->Load(m_configNode);
        }
    }
    catch (const YAML::ParserException& ex)
    {
        // failed to parse, notify and return
        SPDLOG_ERROR("Failed to parse YAML in {}: {}", fileName, ex.what());
    }
    catch (const YAML::BadFile&)
    {
        // if we can't read the file, then try to write it with an empty config
        SaveConfig(fileName);
    }
}

template <typename T>
void ConfigVariable<T>::RenderOption(float widthOverride /* = 0.0f */)
{
    if (m_customRenderFunc)
    {
        m_customRenderFunc(*this, widthOverride);
        return;
    }

    if constexpr (std::is_same_v<T, float>)
    {
        float value = m_value;
        if (Ui::AnimatedSlider(m_displayName.c_str(), &value, m_min, m_max, m_format.c_str(), widthOverride)) 
        {
            set(value);
            Config::Get().SaveSettings();
        }
    } else if constexpr (std::is_same_v<T, int>) {
        float value = static_cast<float>(m_value);
        if (Ui::AnimatedSlider(m_displayName.c_str(), &value, static_cast<float>(m_min), static_cast<float>(m_max), m_format.c_str(), widthOverride)) 
        {
            set(static_cast<int>(value));
            Config::Get().SaveSettings();
        }
    } else if constexpr (std::is_same_v<T, bool>) {
        bool value = m_value;
        if (Ui::AnimatedCheckbox(m_displayName.c_str(), &value)) 
        {
            set(value);
            Config::Get().SaveSettings();
        }
    }
}

// Explicit instantiations
template class ConfigVariable<int>;
template class ConfigVariable<float>;
template class ConfigVariable<bool>;

//=============================================================================

ConfigVariableBase::ConfigVariableBase(ConfigContainer& container)
    : m_container(container)
{
    m_container.Register(this);
}

} // namespace Ui
