#pragma once

#include <smpb/BaseComponent.h>

#include <smgc/display2D.h>
#include <smgc/Mouse.h>

#include <memory.h>

#define smallFontSize 15.0f
#define bigFontSize 15.0f

// Button
#define buttonIndent 4.0f
#define buttonLineThickness 1.0f

class Widbet {
public:
    Button(BaseComponent* t_component);
    int tick(Mouse& mouse) virtual;
    int draw() virtual;
protect:
    BaseComponent* component = nullptr;
    int ID_SMGC;
};
