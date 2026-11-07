#include <sm-gui/Spacer.h>
#include <algorithm>

Spacer::Spacer(BaseComponent* t_component, WidgetMode t_mode)
    : BaseWidget(t_component, t_mode) {
}
int Spacer::draw() {
    if (!component) return -ENOSYS;
    if (!component->Core_isDebugModeEnabled()) return 0;
    glm::vec4 blue = {0.25f, 0.25f, 0.75f, 1.0f};
    float wc = 0.075f;
    component->sendTask(std::make_unique<task_SmgcDrawRect>(glm::vec2{pos.x + size.x * (0.5f - wc * 0.5f), pos.y}, glm::vec2{size.x * wc, size.y}, blue), ID_SMGC);
    return 0;
}