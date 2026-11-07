#pragma once

#include <vector>
#include <memory>
#include <smpb/BaseComponent.h>
#include <smgc/display2D.h>
#include <smgc/Mouse.h>

#include "BaseWidget.h"

#define layoutMode WidgetMode::Normal

class Layout : public BaseWidget {
public:
    Layout(BaseComponent* t_component, WidgetMode t_mode = layoutMode, bool t_vertical = false);
    int setIndent(float t_indent);
    int setSpace(float t_space);
    float getIndent();
    float getSpace();
    
    bool wantsCrossFill() const override;
    int tick(Mouse& mouse) override;
    int sort();

    int push_back(std::unique_ptr<BaseWidget> widget);
    int push_back_penultimate(std::unique_ptr<BaseWidget> widget);
    int pop_back();

    size_t count();
    BaseWidget* operator[](size_t index) const;
    BaseWidget* operator[](size_t index);
    int draw() override;

    bool needsRedraw();

    using iterator       = std::vector<std::unique_ptr<BaseWidget>>::iterator;
    using const_iterator = std::vector<std::unique_ptr<BaseWidget>>::const_iterator;

    iterator begin();
    iterator end();
    const_iterator begin() const;
    const_iterator end()   const;

    iterator erase(iterator pos);
    iterator erase(iterator first, iterator last);
    iterator insert(iterator pos, std::unique_ptr<BaseWidget> widget);

private:
    struct QueuedOp {
        enum class Kind { PushBack, PushPenultimate, PopBack } kind;
        std::unique_ptr<BaseWidget> widget;
    };

    std::vector<QueuedOp> opQueue;
    std::vector<std::unique_ptr<BaseWidget>> widgets{};
    std::vector<glm::vec2> prevHints{};

    float indent       = 5.0f;
    float space        = 3.0f;
    bool  vertical     = false;
    bool  redrawNeeded = true;

    void flushPending();

protected:
    void onSizeChanged() override;
};