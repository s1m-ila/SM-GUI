#include <sm-gui/Button.h>

Button::Button(BaseComponent* t_component, WidgetMode t_mode, const std::string t_text, float t_indent, std::function<void(Button&)> t_onClick)
    : Label(t_component, t_mode, t_text, t_indent), onClick(t_onClick) {
}

int Button::tick(Mouse& mouse) {
    state = 0;
    if (mouse.pos.x > pos.x && mouse.pos.y > pos.y && mouse.pos.x <= pos.x + size.x  && mouse.pos.y <= pos.y + size.y) {
        state = 1;
        if (mouse.press.x > 0.5f) {
            pressed = true;
        } if (pressed && mouse.release.x > 0.5f) {
            pressed = false;
            if (onClick) onClick(*this);
        }
        if (pressed) state = 2;
    } else {
        pressed = false;
    }
    return 0;
}

int Button::draw() {
    if (component) { 
        glm::vec4 colA = {0.49f, 0.38f, 0.20f, 1.0f};
        glm::vec4 colB = {0.42f, 0.32f, 0.18f, 1.0f};
        glm::vec4 colC = {0.28f, 0.22f, 0.12f, 1.0f};
        glm::vec4 colD = {0.02f, 0.02f, 0.02f, 1.0f};
        if (state == 1) {
            colA = {0.58f, 0.44f, 0.21f, 1.0f};
            colB = {0.53f, 0.39f, 0.20f, 1.0f};
            colC = {0.36f, 0.26f, 0.13f, 1.0f};
        } else if (state == 2) {
            colA = {0.35f, 0.26f, 0.14f, 1.0f};
            colB = {0.38f, 0.29f, 0.15f, 1.0f};
            colC = {0.38f, 0.31f, 0.19f, 1.0f};
        }
        component->sendTask(std::make_unique<task_SmgcDrawRect>(pos, size, colD), ID_SMGC);
        
        glm::vec2 mpos = pos + buttonLineThickness;
        glm::vec2 msize = size - buttonLineThickness * 2.0f;

        component->sendTask(std::make_unique<task_SmgcDrawRect>(mpos, msize, colA), ID_SMGC);
        component->sendTask(std::make_unique<task_SmgcDrawRect>(mpos + glm::vec2{buttonGRIndent, buttonGRIndent}, glm::vec2{msize.x - 2 * buttonGRIndent, msize.y - 2 * buttonGRIndent}, colB), ID_SMGC);
        
        component->sendTask(std::make_unique<task_SmgcDrawTriangle>(mpos + glm::vec2{0.0f, msize.y}, mpos + glm::vec2{0.0f, msize.y - buttonGRIndent}, mpos + msize, colC), ID_SMGC);
        component->sendTask(std::make_unique<task_SmgcDrawTriangle>(mpos + glm::vec2{msize.x - buttonGRIndent, msize.y - buttonGRIndent}, mpos + glm::vec2{0.0f, msize.y - buttonGRIndent}, mpos + msize, colC), ID_SMGC);
        
        component->sendTask(std::make_unique<task_SmgcDrawTriangle>(mpos, mpos + glm::vec2{buttonGRIndent, buttonGRIndent}, mpos + glm::vec2(0.0f, msize.y), colC), ID_SMGC);
        component->sendTask(std::make_unique<task_SmgcDrawTriangle>(mpos + glm::vec2{buttonGRIndent, msize.y}, mpos + glm::vec2{buttonGRIndent, buttonGRIndent}, mpos + glm::vec2(0.0f, msize.y), colC), ID_SMGC);

        return drawText();
    }
    return -ENOSYS;
}