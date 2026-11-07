#include <sm-gui/BaseWidget.h>

BaseWidget::BaseWidget(BaseComponent* t_component, WidgetMode t_mode)
    : component(t_component), mode(t_mode) {
    if (component) {
        ID_SMGC = component->Core_getComponentIdxByName("SMGC2D");
    }
}

BaseComponent* BaseWidget::getComponent() const {
    return component;
}

void BaseWidget::setPos(const glm::vec2& t_pos) {
    if (pos == t_pos) return;
    pos = t_pos;
    dirty = true;
}
void BaseWidget::setPos(float x, float y) {
    setPos({x, y});
}
void BaseWidget::setSize(const glm::vec2& t_size) {
    if (size == t_size) return;
    size = t_size;
    dirty = true;
    onSizeChanged();
}
void BaseWidget::setSize(float x, float y) {
    setSize({x, y});
}
void BaseWidget::setMinSize(const glm::vec2& t_minSize) {
    if (minSize == t_minSize) return;
    minSize = t_minSize;
    dirty = true;
    onSizeChanged();
}
void BaseWidget::setMinSize(float x, float y) {
    setMinSize({x, y});
}
void BaseWidget::setMode(WidgetMode t_mode) {
    mode = t_mode;
}
void BaseWidget::setCrossFill(bool fill) {
    if (crossFill == fill) return;
    crossFill = fill;
    dirty = true;
}

glm::vec2 BaseWidget::getPos() const {
    return pos;
}
glm::vec2 BaseWidget::getSize() const {
    return size;
}
glm::vec2 BaseWidget::getMinSize() const {
    return minSize;
}
WidgetMode BaseWidget::getMode() const {
    return mode;
}
bool BaseWidget::getCrossFill() const {
    return crossFill;
}

bool BaseWidget::wantsCrossFill() const {
    return crossFill;
}

int BaseWidget::tick(Mouse& mouse) {
    return 0; 
}

int BaseWidget::draw() {
    if (component) { 
        const glm::vec4 darkGray = glm::vec4{0.25f, 0.25f, 0.25f, 1.0f};
        const glm::vec4 white = glm::vec4{1.0f, 1.0f, 1.0f, 1.0f};
        const glm::vec4 red = glm::vec4{0.75f, 0.25f, 0.25f, 1.0f};
        component->sendTask(std::make_unique<task_SmgcDrawRect>(pos, size, darkGray), ID_SMGC);
        component->sendTask(std::make_unique<task_SmgcDrawRect>(pos + noneFrameThickness, size - noneFrameThickness * 2.0f, white), ID_SMGC);
        component->sendTask(std::make_unique<task_SmgcDrawLine>(pos + noneIndent, pos + size - noneIndent, noneLineThickness, red), ID_SMGC);
        component->sendTask(std::make_unique<task_SmgcDrawLine>(pos + glm::vec2{noneIndent, size.y - noneIndent}, pos + glm::vec2{size.x - noneIndent, noneIndent}, noneLineThickness, red), ID_SMGC);
        return 0;
    }
    return -ENOSYS;
}
