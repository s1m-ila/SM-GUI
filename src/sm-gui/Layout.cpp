#include <sm-gui/Layout.h>
#include <algorithm>

Layout::Layout(BaseComponent* t_component, WidgetMode t_mode, bool t_vertical)
    : BaseWidget(t_component, t_mode), vertical(t_vertical) {
}

int Layout::setIndent(float t_indent) {
    indent = t_indent;
    return 0;
}
int Layout::setSpace(float t_space) {
    space = t_space;
    return 0;
}
float Layout::getIndent() {
    return indent;
}
float Layout::getSpace() {
    return space;
}

bool Layout::wantsCrossFill() const {
    return true;
}

void Layout::onSizeChanged() {
    sort();
}

int Layout::tick(Mouse& mouse) {
    for (const auto& widget : widgets)
        if (widget) widget->tick(mouse);

    flushPending();

    bool needSort = dirty;
    for (size_t i = 0; i < widgets.size(); ++i) {
        glm::vec2 h = widgets[i]->getMinSize();
        if (h != prevHints[i]) {
            prevHints[i] = h;
            needSort = true;
            dirty = true;
        }
    }

    if (needSort || dirty) sort();
    return 0;
}

int Layout::sort() {
    dirty = false;

    float mainSum  = 0.0f;
    float crossMax = 0.0f;
    for (const auto& w : widgets) {
        if (!w) continue;
        glm::vec2 h = w->getMinSize();
        if (vertical) { mainSum += h.y; crossMax = std::max(crossMax, h.x); }
        else          { mainSum += h.x; crossMax = std::max(crossMax, h.y); }
    }
    if (widgets.size() > 1)
        mainSum += space * (widgets.size() - 1);

    minSize = vertical
        ? glm::vec2{ crossMax + indent * 2.0f, mainSum + indent * 2.0f }
        : glm::vec2{ mainSum + indent * 2.0f, crossMax + indent * 2.0f };

    const int n = static_cast<int>(widgets.size());
    if (n == 0) { redrawNeeded = true; return 0; }

    const float contentW = std::max(0.0f, size.x - indent * 2.0f);
    const float contentH = std::max(0.0f, size.y - indent * 2.0f);
    const float spacingTotal = space * std::max(0, n - 1);
    const float mainSpace  = std::max(0.0f, (vertical ? contentH : contentW) - spacingTotal);
    const float crossSpace = vertical ? contentW : contentH;

    std::vector<float> hint(n, 0.0f);
    std::vector<int> shrinkRank(n, 0);
    std::vector<int> growRank(n, 0);

    float sumHint = 0.0f;
    for (int i = 0; i < n; ++i) {
        auto* w = widgets[i].get();
        glm::vec2 ms = w->getMinSize();
        float h = vertical ? ms.y : ms.x;
        if (h < 0.0f) h = 0.0f;
        hint[i] = h;
        sumHint += h;

        switch (w->getMode()) {
            case WidgetMode::Fixed:     shrinkRank[i] = 4; growRank[i] = 0; break;
            case WidgetMode::Extending: shrinkRank[i] = 3; growRank[i] = 3; break;
            case WidgetMode::Maximal:   shrinkRank[i] = 2; growRank[i] = 2; break;
            case WidgetMode::Normal:    shrinkRank[i] = 1; growRank[i] = 1; break;
            case WidgetMode::Minimal:   shrinkRank[i] = 0; growRank[i] = 0; break;
        }
    }

    std::vector<float> sizes(n, 0.0f);

    if (sumHint <= mainSpace + 0.01f) {
        for (int i = 0; i < n; ++i) sizes[i] = hint[i];
        float remaining = mainSpace - sumHint;

        for (int gr = 3; gr >= 1 && remaining > 0.01f; --gr) {
            int count = 0;
            for (int i = 0; i < n; ++i)
                if (growRank[i] == gr) count++;
            if (count == 0) continue;

            float per = remaining / static_cast<float>(count);
            for (int i = 0; i < n; ++i)
                if (growRank[i] == gr) sizes[i] += per;
            remaining = 0.0f;
        }
    } else {
        for (int i = 0; i < n; ++i) sizes[i] = hint[i];
        float deficit = sumHint - mainSpace;

        for (int sr = 0; sr <= 3 && deficit > 0.01f; ++sr) {
            float classTotal = 0.0f;
            int count = 0;
            for (int i = 0; i < n; ++i) {
                if (shrinkRank[i] == sr) {
                    classTotal += sizes[i];
                    count++;
                }
            }
            if (count == 0) continue;

            if (classTotal >= deficit) {
                float factor = deficit / classTotal;
                for (int i = 0; i < n; ++i)
                    if (shrinkRank[i] == sr)
                        sizes[i] -= sizes[i] * factor;
                deficit = 0.0f;
            } else {
                for (int i = 0; i < n; ++i)
                    if (shrinkRank[i] == sr)
                        sizes[i] = 0.0f;
                deficit -= classTotal;
            }
        }
    }

    float cursor = (vertical ? pos.y : pos.x) + indent;
    for (int i = 0; i < n; ++i) {
        auto* w = widgets[i].get();
        glm::vec2 ms = w->getMinSize();

        float crossHint = vertical ? ms.x : ms.y;
        float crossSize;
        if (w->getMode() == WidgetMode::Fixed) {
            crossSize = std::min(crossHint, crossSpace);
        } else {
            crossSize = w->wantsCrossFill() ? crossSpace : std::min(crossHint, crossSpace);
        }

        glm::vec2 wp, ws;
        if (vertical) {
            wp = {pos.x + indent, cursor};
            ws = {crossSize, sizes[i]};
        } else {
            wp = {cursor, pos.y + indent};
            ws = {sizes[i], crossSize};
        }
        w->setPos(wp);
        w->setSize(ws);
        cursor += sizes[i] + space;
    }

    redrawNeeded = true;
    return 0;
}

int Layout::push_back(std::unique_ptr<BaseWidget> widget) {
    if (!widget) return -ENOSYS;
    opQueue.push_back({QueuedOp::Kind::PushBack, std::move(widget)});
    return 0;
}

int Layout::push_back_penultimate(std::unique_ptr<BaseWidget> widget) {
    if (!widget) return -ENOSYS;
    opQueue.push_back({QueuedOp::Kind::PushPenultimate, std::move(widget)});
    return 0;
}

int Layout::pop_back() {
    opQueue.push_back({QueuedOp::Kind::PopBack, nullptr});
    return 0;
}

void Layout::flushPending() {
    if (opQueue.empty() && prevHints.size() == widgets.size())
        return;

    bool changed = false;

    for (auto& op : opQueue) {
        switch (op.kind) {
            case QueuedOp::Kind::PushBack:
                widgets.push_back(std::move(op.widget));
                changed = true;
                break;

            case QueuedOp::Kind::PushPenultimate:
                if (widgets.empty())
                    widgets.push_back(std::move(op.widget));
                else
                    widgets.insert(widgets.end() - 1, std::move(op.widget));
                changed = true;
                break;

            case QueuedOp::Kind::PopBack:
                if (!widgets.empty()) {
                    widgets.pop_back();
                    changed = true;
                }
                break;
        }
    }
    opQueue.clear();

    if (prevHints.size() != widgets.size()) {
        prevHints.resize(widgets.size(), glm::vec2{0.0f, 0.0f});
        changed = true;
    }

    if (changed) {
        dirty = true;
        redrawNeeded = true;
    }
}

bool Layout::needsRedraw() {
    bool result = redrawNeeded;
    redrawNeeded = false;
    return result;
}

size_t Layout::count() {
    flushPending();
    return widgets.size();
}

BaseWidget* Layout::operator[](size_t i) {
    flushPending();
    return i < widgets.size() ? widgets[i].get() : nullptr;
}

BaseWidget* Layout::operator[](size_t i) const {
    return i < widgets.size() ? widgets[i].get() : nullptr;
}

int Layout::draw() {
    if (!component) return -ENOSYS;
    flushPending();
    for (const auto& w : widgets) w->draw();
    return 0;
}

Layout::iterator       Layout::begin() { flushPending(); return widgets.begin(); }
Layout::iterator       Layout::end()   { flushPending(); return widgets.end();   }
Layout::const_iterator Layout::begin() const { return widgets.begin(); }
Layout::const_iterator Layout::end()   const { return widgets.end();   }

Layout::iterator Layout::erase(iterator pos) {
    auto it = widgets.erase(pos);
    dirty = true;
    redrawNeeded = true;
    return it;
}

Layout::iterator Layout::erase(iterator first, iterator last) {
    auto it = widgets.erase(first, last);
    dirty = true;
    redrawNeeded = true;
    return it;
}

Layout::iterator Layout::insert(iterator pos, std::unique_ptr<BaseWidget> widget) {
    auto it = widgets.insert(pos, std::move(widget));
    dirty = true;
    redrawNeeded = true;
    return it;
}