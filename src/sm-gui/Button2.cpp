#include <sm-gui/Button2.h>

Button2::Button2(BaseComponent* t_component, WidgetMode t_mode, const std::string t_text, float t_indent, std::function<void(Button&)> t_onClick)
    : Button(t_component, t_mode, t_text, t_indent, t_onClick) {
}

int Button2::draw() {
    if (component) { 
        glm::vec4 colA = {0.24f, 0.30f, 0.33f, 1.0f};
        glm::vec4 colB = {0.15f, 0.18f, 0.21f, 1.0f};
        glm::vec4 colC = {0.10f, 0.13f, 0.15f, 1.0f};
        glm::vec4 colD = {0.36f, 0.36f, 0.36f, 1.0f};
        glm::vec4 colE = {0.0f, 0.0f, 0.0f, 0.25f};
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
        
        glm::vec2 mpos = pos + button2LineThickness;
        glm::vec2 msize = size - button2LineThickness * 2.0f;

        component->sendTask(std::make_unique<task_SmgcDrawRect>(mpos, msize, colA), ID_SMGC);
        component->sendTask(std::make_unique<task_SmgcDrawRect>(mpos + glm::vec2{button2GRIndent, button2GRIndent}, glm::vec2{msize.x - 2 * button2GRIndent, msize.y - 2 * button2GRIndent}, colB), ID_SMGC);
        
        component->sendTask(std::make_unique<task_SmgcDrawTriangle>(mpos + glm::vec2{0.0f, msize.y}, mpos + glm::vec2{0.0f, msize.y - button2GRIndent}, mpos + msize, colC), ID_SMGC);
        component->sendTask(std::make_unique<task_SmgcDrawTriangle>(mpos + glm::vec2{msize.x - button2GRIndent, msize.y - button2GRIndent}, mpos + glm::vec2{0.0f, msize.y - button2GRIndent}, mpos + msize, colC), ID_SMGC);
        
        component->sendTask(std::make_unique<task_SmgcDrawTriangle>(mpos, mpos + glm::vec2{button2GRIndent, button2GRIndent}, mpos + glm::vec2(0.0f, msize.y), colC), ID_SMGC);
        component->sendTask(std::make_unique<task_SmgcDrawTriangle>(mpos + glm::vec2{button2GRIndent, msize.y}, mpos + glm::vec2{button2GRIndent, button2GRIndent}, mpos + glm::vec2(0.0f, msize.y), colC), ID_SMGC);
        
        float blackArea = 0.5588f;
        component->sendTask(std::make_unique<task_SmgcDrawRect>(glm::vec2{mpos.x, mpos.y + msize.y * (1.0f - blackArea)}, glm::vec2{msize.x, msize.y * blackArea}, colE), ID_SMGC);

        return drawText();
    }
    return -ENOSYS;
}