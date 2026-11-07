#pragma once

#include <smpb/BaseComponent.h>

#include <smgc/display2D.h>
#include <smgc/Mouse.h>
#include <string>

#include "BaseWidget.h"
#include "Button.h"

#define button2GRIndent 2.0f
#define button2LineThickness 1.0f

#define button2Mode WidgetMode::Normal
#define button2Text "И АЛЕКСАНДР"
#define button2Indent labelIndent + buttonGRIndent + buttonLineThickness

class Button2 : public Button {
public:
    Button2(BaseComponent* t_component, WidgetMode t_mode = button2Mode, const std::string t_text = button2Text, float t_indent = button2Indent, std::function<void(Button&)> t_onClick = nullptr);
    int draw() override;
};
