#pragma once

#include <smpb/BaseComponent.h>

#include <smgc/display2D.h>
#include <smgc/Mouse.h>
#include <cerrno>

#define smallFontSize 9.0f
#define mediumFontSize 15.0f
#define bigFontSize 20.0f

#define noneFrameThickness 2.0f
#define noneLineThickness 5.0f
#define noneIndent 7.0f

enum class WidgetMode {
    Normal    = 0,
    Fixed     = 1,
    Minimal   = 2,
    Maximal   = 3,
    Extending = 4,
};

class BaseWidget {
public:
    BaseWidget(BaseComponent* t_component, WidgetMode t_mode = WidgetMode::Normal);
    virtual ~BaseWidget() = default;
    virtual int tick(Mouse& mouse);
    virtual int draw();

    BaseComponent* getComponent() const;

    void setPos(const glm::vec2& t_pos);
    void setPos(float x, float y);

    void setSize(const glm::vec2& t_size);
    void setSize(float x, float y);

    void setMinSize(const glm::vec2& t_minSize);
    void setMinSize(float x, float y);

    void setMode(WidgetMode t_mode);
    void setCrossFill(bool fill);

    glm::vec2 getPos() const;
    glm::vec2 getSize() const;
    glm::vec2 getMinSize() const;
    WidgetMode getMode() const;
    bool getCrossFill() const;

    virtual bool wantsCrossFill() const;
protected:
    virtual void onSizeChanged() {}
    BaseComponent* component = nullptr;
    int ID_SMGC = -1;
    glm::vec2 pos{0.0f, 0.0f};
    glm::vec2 size{50.0f, 50.0f};
    glm::vec2 minSize{0.0f, 0.0f};
    WidgetMode mode = WidgetMode::Normal;
    bool dirty = true;
    bool crossFill = true;
};
