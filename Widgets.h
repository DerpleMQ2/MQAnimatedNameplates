#pragma once

#include <string>
#include <vector>

namespace Ui {

bool AnimatedCheckbox(const std::string& label, bool* value);
bool AnimatedCombo(const char* label, int* value, const std::vector<std::string>& items);

bool AnimatedSlider(const char* label, float* slider_value, float slider_min, float slider_max,
    const char* format = "%.2f", float width = 0.0f);
bool AnimatedSlider(const char* label, int* slider_value, int slider_min, int slider_max,
    const char* format = "%d", float width = 0.0f);
bool AnimatedSlider(const char* label, unsigned int* slider_value, unsigned int slider_min, unsigned int slider_max,
    const char* format = "%d", float width = 0.0f);

} // namespace Ui
