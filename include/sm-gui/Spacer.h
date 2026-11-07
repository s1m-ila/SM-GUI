#pragma once

#include <smpb/BaseComponent.h>

#include <smgc/display2D.h>

#include "BaseWidget.h"

#define spacerMode WidgetMode::Extending

class Spacer : public BaseWidget {
public:
    Spacer(BaseComponent* t_component, WidgetMode t_mode = spacerMode);
    int draw() override;
};
