#include "Scene2D.h"

#include <smpb/Core.h>

#include <array>
#include <cmath>
#include <format>

static constexpr std::array<int, 6> kKeys = {
    GLFW_KEY_W, GLFW_KEY_A, GLFW_KEY_S, GLFW_KEY_D,
    GLFW_KEY_O, GLFW_KEY_R,
};

Scene2D::Scene2D() : BaseComponent("Scene2D") {
}

int Scene2D::init() {
    ID_SMGC = Core_getComponentIdxByName("SMGC2D");
    for (int key : kKeys) {
        int errorCode = sendTask(std::make_unique<task_SmgcSubscribeKey>(key), ID_SMGC);
        if (errorCode != 0) return errorCode;
    }
    //const glm::vec2 buttonsSize = {72.0f, 30.0f};

    /*layout = std::make_unique<Layout>(this, layoutMode, true);
    
    auto buttonsLayout = std::make_unique<Layout>(this);
    buttonsLayout->push_back(
        std::make_unique<Button>(this, WidgetMode::Normal, "Add\nButton", buttonIndent,
            [](Button& b) {
                auto scene = dynamic_cast<Scene2D*>(b.getComponent());
                if (scene) {
                    if (scene->layout) {
                        scene->layout->push_back_penultimate(std::make_unique<Button>(scene));
                    }
                }
            }
        )
    );
    buttonsLayout->push_back(
        std::make_unique<Button>(this, WidgetMode::Normal, "Add\nLabel", buttonIndent,
            [](Button& b) {
                auto scene = dynamic_cast<Scene2D*>(b.getComponent());
                if (scene) {
                    if (scene->layout) {
                        scene->layout->push_back_penultimate(std::make_unique<Label>(scene));
                    }
                }
            }
        )
    );
    buttonsLayout->push_back(
        std::make_unique<Button>(this, WidgetMode::Extending, "Del", buttonIndent,
            [](Button& b) {
                auto scene = dynamic_cast<Scene2D*>(b.getComponent());
                if (scene) {
                    if (scene->layout && scene->layout->count() > 3) {
                        scene->layout->erase(scene->layout->end() - 2);
                    }
                }
            }
        )
    );
    layout->push_back(std::move(buttonsLayout));

    auto buttons2Layout = std::make_unique<Layout>(this);
    buttons2Layout->push_back(std::make_unique<Button>(this, WidgetMode::Minimal, "Minimal"));
    buttons2Layout->push_back(std::make_unique<Button>(this, WidgetMode::Normal, "Normal"));
    buttons2Layout->push_back(std::make_unique<Button>(this, WidgetMode::Minimal, "Minimal"));
    layout->push_back(std::move(buttons2Layout));

    layout->push_back(std::make_unique<Spacer>(this));*/
    layout = std::make_unique<Layout>(this, layoutMode, true);
    layout->setIndent(0.0f);
    layout->setSpace(0.0f);

    layout->push_back(std::make_unique<Spacer>(this));


    auto layout2 = std::make_unique<Layout>(this);
    layout2->setIndent(0.0f);
    layout2->setSpace(0.0f);
    auto btn = std::make_unique<Button>(this, WidgetMode::Fixed, "I'M SUPER");
    btn->setMinSize(432.0f, 165.0f);
    layout2->push_back(std::move(btn));
    //layout2->push_back(std::make_unique<Spacer>(this));
    layout->push_back(std::move(layout2));

    auto layout3 = std::make_unique<Layout>(this);
    layout3->setIndent(0.0f);
    layout3->setSpace(0.0f);
    std::vector<std::string> texts = {"Проекты", "Работа", "Режим дня", "Назначения", "Питомцы", "Фауна", "Технологии", "Задания", "Мир", "История", "Фракции"};
    for (const auto& text : texts) {
        auto btn = std::make_unique<Button2>(this, WidgetMode::Normal, text);
        glm::vec2 sms = btn->getMinSize();
        btn->setMinSize(sms.x, 34.0f);
        layout3->push_back(std::move(btn));
    }
    layout->push_back(std::move(layout3));

    return sendTask(std::make_unique<task_SmgcSetOutline>(outlineEnabled), ID_SMGC);
}

int Scene2D::processTask(std::unique_ptr<BaseTask> task) {
    if (taskIs_SmgcKeyState(task->getIdx())) {
        auto* typed = static_cast<task_SmgcKeyState*>(task.get());
        pressedKeys[typed->key] = typed->pressed;
        return 0;
    }
    return -EINVAL;
}

bool Scene2D::isPressed(int key) const {
    auto it = pressedKeys.find(key);
    return it != pressedKeys.end() && it->second;
}

void Scene2D::keyboard() {
    bool outlineKeyPressed = isPressed(GLFW_KEY_O);
    if (outlineKeyPressed && !outlineKeyWasPressed) {
        outlineEnabled = !outlineEnabled;
        sendTask(std::make_unique<task_SmgcSetOutline>(outlineEnabled), ID_SMGC);
        dirty = true;
    }
    outlineKeyWasPressed = outlineKeyPressed;
}

/*int Scene2D::drawButton(glm::vec2 pos, glm::vec2 size, bool pressed) {
    int lineSize = 10.0f;
    int fontSize = 30.0f;
    glm::vec4 colA = {0.75f, 0.75f, 0.75f, 1.0f};
    glm::vec4 colB = {0.5f, 0.5f, 0.5f, 1.0f};
    glm::vec4 colC = {0.25f, 0.25f, 0.25f, 1.0f};

    glm::vec4 col;
    if (pressed) {
        col = colA;
        colA = colC;
        colC = col;
    }

    sendTask(std::make_unique<task_SmgcDrawRect>(pos + glm::vec2{0.0f, 0.0f}, glm::vec2{size.x, size.y}, colB), ID_SMGC);
    sendTask(std::make_unique<task_SmgcDrawRect>(pos + glm::vec2{0.0f, 0.0f}, glm::vec2{size.x - lineSize, size.y - lineSize}, colA), ID_SMGC);
    sendTask(std::make_unique<task_SmgcDrawRect>(pos + glm::vec2{lineSize, lineSize}, glm::vec2{size.x - lineSize, size.y - lineSize}, colC), ID_SMGC);
    sendTask(std::make_unique<task_SmgcDrawRect>(pos + glm::vec2{lineSize, lineSize}, glm::vec2{size.x - lineSize * 2, size.y - lineSize * 2}, colB), ID_SMGC);
    return sendTask(std::make_unique<task_SmgcDrawText>(
        "Я кнопка?", pos + glm::vec2{size.x / 2.0f, size.y / 2.0f - fontSize * 0.5f}, fontSize, glm::vec4{1.0f, 1.0f, 1.0f, 1.0f}, TextAlign::Center), ID_SMGC);
    
}*/

int Scene2D::drawButton(const glm::vec2& pos, const glm::vec2& size, const std::string& text, int state) {
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
    sendTask(std::make_unique<task_SmgcDrawRect>(pos, size, colD), ID_SMGC);
    
    glm::vec2 mpos = pos + buttonLineThickness;
    glm::vec2 msize = size - buttonLineThickness * 2.0f;

    sendTask(std::make_unique<task_SmgcDrawRect>(mpos, msize, colA), ID_SMGC);
    sendTask(std::make_unique<task_SmgcDrawRect>(mpos + glm::vec2{buttonGRIndent, buttonGRIndent}, glm::vec2{msize.x - 2 * buttonGRIndent, msize.y - 2 * buttonGRIndent}, colB), ID_SMGC);
    
    sendTask(std::make_unique<task_SmgcDrawTriangle>(mpos + glm::vec2{0.0f, msize.y}, mpos + glm::vec2{0.0f, msize.y - buttonGRIndent}, mpos + msize, colC), ID_SMGC);
    sendTask(std::make_unique<task_SmgcDrawTriangle>(mpos + glm::vec2{msize.x - buttonGRIndent, msize.y - buttonGRIndent}, mpos + glm::vec2{0.0f, msize.y - buttonGRIndent}, mpos + msize, colC), ID_SMGC);
    
    sendTask(std::make_unique<task_SmgcDrawTriangle>(mpos, mpos + glm::vec2{buttonGRIndent, buttonGRIndent}, mpos + glm::vec2(0.0f, msize.y), colC), ID_SMGC);
    sendTask(std::make_unique<task_SmgcDrawTriangle>(mpos + glm::vec2{buttonGRIndent, msize.y}, mpos + glm::vec2{buttonGRIndent, buttonGRIndent}, mpos + glm::vec2(0.0f, msize.y), colC), ID_SMGC);

    return sendTask(std::make_unique<task_SmgcDrawText>(
        text, pos + glm::vec2{size.x / 2.0f, size.y / 2.0f - mediumFontSize * 0.5f}, mediumFontSize, glm::vec4{1.0f, 1.0f, 1.0f, 1.0f}, TextAlign::Center), ID_SMGC);
    
}

int Scene2D::tick(long long tickCounter) {
    keyboard();

    sendTask(std::make_unique<task_SmgcGetWindowSize>(&windowWidth, &windowHeight), ID_SMGC);

    Mouse mouse;
    sendTask(std::make_unique<task_SmgcGetMouse>(&mouse), ID_SMGC);

    if (layout->needsRedraw() || oMouse != mouse || oWindowWidth != windowWidth || oWindowHeight != windowHeight || dirty) {
        int n = std::rand() % 1000;
        std::clog << getPrefix() << n << " draw frame! mouse.pos = glm::vec2(" << mouse.pos.x << ", " << mouse.pos.y << ')' << std::endl;
        sendTask(std::make_unique<task_SmgcClearObjects>(), ID_SMGC);
        
        sendTask(std::make_unique<task_SmgcDrawRect>(glm::vec2{0.0f, 0.0f}, glm::vec2{windowWidth, windowHeight}, glm::vec4{0.5f, 0.5f, 0.5f, 1.0f}), ID_SMGC);
        
        const glm::vec2 buttonsSize = {72.0f, 30.0f};

        layout->setSize(windowWidth, windowHeight);
        layout->setMinSize(windowWidth, windowHeight);
        layout->tick(mouse);
        layout->draw();
        drawButton( mouse.pos, buttonsSize, "Восток", 0);

        sendTask(std::make_unique<task_SmgcSwapObjects>(), ID_SMGC);
    }

    oMouse = mouse;
    oWindowWidth = windowWidth, oWindowHeight = windowHeight;
    dirty = false;

    return 0;
}

void Scene2D::death() {
    for (int key : kKeys) sendTask(std::make_unique<task_SmgcUnsubscribeKey>(key), ID_SMGC);
}
