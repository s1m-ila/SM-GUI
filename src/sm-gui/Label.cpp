#include <sm-gui/Label.h>

Label::Label(BaseComponent* t_component, WidgetMode t_mode, const std::string t_text, float t_indent)
    : BaseWidget(t_component, t_mode), text(t_text) {
    setText(text);
}

void Label::setText(const std::string t_text) {
    text = t_text;
    countLines = text.empty() ? 1 : std::count(text.begin(), text.end(), '\n') + 1;

    if (component) {
        glm::vec2 measured{0.0f, 0.0f};
        component->sendTask(
            std::make_unique<task_SmgcMeasureText>(text, fontSize, &measured.x, &measured.y),
            ID_SMGC);
        minSize = measured + indent * 2.0f;
    }
    dirty = true;
}

int Label::drawText() {
    return component->sendTask(std::make_unique<task_SmgcDrawText>(
        text, pos + glm::vec2{size.x / 2.0f, size.y / 2.0f - fontSize * countLines * 0.5f}, fontSize, glm::vec4{1.0f, 1.0f, 1.0f, 1.0f}, TextAlign::Center), ID_SMGC);
}

int Label::draw() {
    if (component) { 
        return drawText();
    }
    return -ENOSYS;
}