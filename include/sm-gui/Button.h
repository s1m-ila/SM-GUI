#pragma once

#include <smpb/BaseComponent.h>

#include <smgc/display2D.h>
#include <smgc/Mouse.h>
#include <string>
#include <functional>

#include "BaseWidget.h"
#include "Label.h"

#define buttonGRIndent 4.0f
#define buttonLineThickness 1.0f

#define buttonMode WidgetMode::Normal
#define buttonText "И АЛЕКСАНДР"
#define buttonIndent labelIndent + buttonGRIndent + buttonLineThickness

class Button : public Label {
public:
    Button(BaseComponent* t_component, WidgetMode t_mode = buttonMode, const std::string t_text = buttonText, float t_indent = buttonIndent, std::function<void(Button&)> t_onClick = nullptr);
    int tick(Mouse& mouse) override;
    int draw() override;
    std::function<void(Button&)> onClick = nullptr;
protected:
    int state = 0;
    bool pressed = false;
};
