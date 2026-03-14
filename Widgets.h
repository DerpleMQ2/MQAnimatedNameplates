#pragma once

#include <string>
#include <vector>

namespace Ui {

bool AnimatedCheckbox(const std::string& label, bool* value);
bool AnimatedSlider(const std::string& label, float* slider_value, float slider_min, float slider_max,
                    const char* format = "%.2f", float labelWidthOverride = 0.0f);
bool AnimatedCombo(const std::string& label, int* value, const std::vector<std::string>& items);

} // namespace Ui