#pragma once

#include <smpb/BaseComponent.h>

#include <smgc/display2D.h>
#include <string>

#include "BaseWidget.h"

#define labelIndent 5.0f
#define labelMode WidgetMode::Normal
#define labelText "МАМА - ЛУЧШАЯ"

class Label : public BaseWidget {
public:
    Label(BaseComponent* t_component, WidgetMode t_mode = labelMode, const std::string t_text = labelText, float t_indent = labelIndent);
    void setText(const std::string t_text);
    int drawText();
    int draw() override;
protected:
    std::string text = "";
    int countLines = 1;
    int fontSize = mediumFontSize;
    float indent = labelIndent;
};
