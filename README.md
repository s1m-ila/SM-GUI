# SM-GUI

**SM-GUI** — виджеты для 2D-рендерера [SMGC](https://github.com/s1m-ila/SMGC) на ядре [SMPB](https://github.com/s1m-ila/SMPB). Он даёт готовые элементы интерфейса — метки, кнопки двух стилей, спейсеры и контейнер-раскладку — поверх `SMGC2D`, и подключается к другому CMake-проекту так же, как `smgc`: через `add_subdirectory` или `FetchContent`.

Компонентов у SM-GUI нет: это **библиотека виджетов**, которую использует ваш собственный компонент-сцена. Вы создаёте виджеты, складываете их в `Layout`, а на каждом тике вызываете `tick(mouse)` и `draw()` — виджеты сами отправляют в `SMGC2D` задачи рисования фигур и текста.

## Содержание

- [Возможности](#возможности)
- [Сборка](#сборка)
- [Подключение к другому проекту](#подключение-к-другому-проекту)
- [Быстрый старт](#быстрый-старт)
- [Архитектура](#архитектура)
  - [Базовый виджет](#базовый-виджет)
  - [Режимы виджетов](#режимы-виджетов)
  - [Раскладка](#раскладка)
  - [Отрисовка](#отрисовка)
  - [События мыши](#события-мыши)
  - [Отложенные операции](#отложенные-операции)
  - [Перерисовка](#перерисовка)
- [Справочник по классам](#справочник-по-классам)
  - [BaseWidget](#basewidget)
  - [Label](#label)
  - [Button](#button)
  - [Button2](#button2)
  - [Spacer](#spacer)
  - [Layout](#layout)
- [Соглашения](#соглашения)
- [Пример: сцена с раскладкой](#пример-сцена-с-раскладкой)
- [Структура проекта](#структура-проекта)
- [Лицензия](#лицензия)

## Возможности

- Готовые виджеты `Label`, `Button`, `Button2`, `Spacer` и контейнер `Layout` для `SMGC2D`. Все координаты и размеры — в пикселях окна, как в задачах SMGC.
- Единая база `BaseWidget`: позиция, размер, минимальный размер, режим растяжения и флаг перерисовки. Виджеты не знают о задаче рисования — они вызывают `sendTask` сами.
- Раскладка `Layout` двух направлений: горизонтальная и вертикальная. Распределяет свободное место между детьми по их режимам, умеет сжимать и растягивать, поддерживает перекрёстное заполнение.
- Три режима растяжения (`Normal`, `Fixed`, `Minimal`, `Maximal`, `Extending`) — от «не трогать» до «забрать всё свободное место». Позволяют собирать как жёсткие панели, так и тянущиеся списки.
- Автоматический размер `Label` по тексту: `Label::setText` измеряет строку через `task_SmgcMeasureText` с текущим шрифтом `SMGC2D` и обновляет `minSize`, поэтому раскладка сама подгоняет размеры под содержимое.
- Кнопки с состояниями. `Button` рисует объёмную «каменную» кнопку со скосами из треугольников; `Button2` — плоскую полосу с затемнением снизу. Оба подсвечиваются при наведении и «вдавливаются» при нажатии.
- `Button::onClick` — функция обратного вызова, которая вызывается один раз на нажатие-отпускание внутри кнопки. Позволяет добавлять и удалять элементы прямо из обработчика.
- Безопасное изменение списка детей из `onClick`: `push_back`, `push_back_penultimate`, `pop_back`, `insert`, `erase` кладут операцию в очередь, и она применяется на ближайшем `tick`. Итераторы не инвалидируются во время обработки.
- Ранний выход из перерисовки. `Layout::needsRedraw()` возвращает `true` только когда что-то изменилось, поэтому сцену можно не пересобирать каждый тик.
- Отладочный режим: `Spacer` рисует синюю полосу, чтобы видеть, куда раскладка отвела место. Включается общим флагом отладки ядра SMPB.
- Работает только с `smgc::display2D`: использует `SMGC2D` и его задачи, но не трогает `SMGC` (3D).
- Собирается как статическая библиотека `sm-gui::elements` и подключается через `add_subdirectory` или `FetchContent`.

## Сборка

### Требования

- CMake 3.16 или новее
- Компилятор с поддержкой C++23 (проверено на GCC)
- Ninja или Make
- [SMPB](https://github.com/s1m-ila/SMPB) и [SMGC](https://github.com/s1m-ila/SMGC), подключённые рядом с проектом или через `FetchContent`
- GLFW 3, GLM и OpenGL, установленные в системе: их ищет `smgc` при сборке

sm-gui сам ничего не скачивает: `smpb` и `smgc` подключаются как подпроекты и должны быть доступны на момент конфигурации.

### Обычная сборка

```bash
cmake -S . -B build -G Ninja
cmake --build build
```

Собираются цели:

| Цель | Тип | Результат | Описание |
|------|-----|-----------|----------|
| `sm-gui_elements` (псевдоним `sm-gui::elements`) | статическая библиотека | `libsm-gui_elements.a` | Все виджеты: `BaseWidget`, `Label`, `Button`, `Button2`, `Spacer`, `Layout`. Публичные заголовки лежат в `include/sm-gui/`. `smpb::core` и `smgc::display2D` подключены как `PUBLIC`-зависимости, поэтому их заголовки и требование C++23 передаются всем, кто линкуется с библиотекой. |
| `sm-gui` | программа | `sm-gui`, `files/core.config` | Пример из `example/`, слинкованный с `sm-gui::elements`. Собирается, только если включена опция `SM_GUI_BUILD_EXAMPLE`. После сборки `core.config` копируется из `files/` репозитория в папку `files/` рядом с программой. |

| Опция | По умолчанию | Описание |
|-------|--------------|----------|
| `SM_GUI_BUILD_EXAMPLE` | `ON`, если sm-gui собирается сам; `OFF` внутри другого проекта | Собирать ли программу `sm-gui` |

Сборка только библиотеки:

```bash
cmake -S . -B build -G Ninja -DSM_GUI_BUILD_EXAMPLE=OFF
cmake --build build
```

Установка библиотеки, заголовков и CMake-пакета `sm-gui` в выбранный префикс:

```bash
cmake --install build --prefix /путь/установки
```

## Подключение к другому проекту

sm-gui подключается как CMake-подпроект. В этом случае собирается только библиотека: программа `sm-gui` не собирается.

### Через `add_subdirectory`

Если исходники sm-gui, smpb и smgc лежат рядом с проектом:

```cmake
cmake_minimum_required(VERSION 3.16)
project(myapp LANGUAGES C CXX)

add_subdirectory(../smpb smpb)
add_subdirectory(../smgc smgc)
add_subdirectory(../sm-gui sm-gui)

add_executable(myapp main.cpp)
target_link_libraries(myapp PRIVATE sm-gui::elements)
smgc_copy_files(myapp)
```

Второй аргумент `add_subdirectory` (папка сборки) обязателен, если путь лежит вне папки проекта.

`smgc_copy_files(myapp)` копирует шейдеры и шрифт `SMGC2D` в папку `files/` рядом с программой. Без этих файлов `SMGC2D` не запустится, а значит, и виджеты рисоваться не будут.

### Через `FetchContent`

sm-gui можно скачать из репозитория, но smpb и smgc всё равно нужно подключить до него:

```cmake
cmake_minimum_required(VERSION 3.16)
project(myapp LANGUAGES C CXX)

include(FetchContent)

FetchContent_Declare(smpb
  GIT_REPOSITORY https://github.com/s1m-ila/SMPB.git
  GIT_TAG        main
)
FetchContent_MakeAvailable(smpb)

FetchContent_Declare(smgc
  GIT_REPOSITORY https://github.com/s1m-ila/SMGC.git
  GIT_TAG        main
)
FetchContent_MakeAvailable(smgc)

FetchContent_Declare(sm-gui
  GIT_REPOSITORY https://github.com/s1m-ila/SM-GUI.git
  GIT_TAG        main
)
FetchContent_MakeAvailable(sm-gui)

add_executable(myapp main.cpp)
target_link_libraries(myapp PRIVATE sm-gui::elements)
smgc_copy_files(myapp)
```

### Использование установленного пакета

После `cmake --install` пакет `sm-gui` ищется через `find_package`:

```cmake
find_package(sm-gui REQUIRED)
target_link_libraries(myapp PRIVATE sm-gui::elements)
```

Пакет сам подтягивает `smpb` и `smgc` через `find_dependency`, поэтому их тоже нужно установить.

### Заголовки

Заголовки подключаются с префиксом `sm-gui/`:

```cpp
#include <sm-gui/BaseWidget.h>
#include <sm-gui/Label.h>
#include <sm-gui/Button.h>
#include <sm-gui/Button2.h>
#include <sm-gui/Spacer.h>
#include <sm-gui/Layout.h>
```

## Быстрый старт

Минимальная сцена: чёрный фон, метка сверху и кнопка под ней. Компонент-сцена добавляется в ядро **после** `SMGC2D`, иначе на первом тике окно ещё не создано.

```cpp
#include <smpb/Core.h>
#include <smpb/Config.h>
#include <smgc/display2D.h>

#include <sm-gui/Layout.h>
#include <sm-gui/Label.h>
#include <sm-gui/Button.h>

class Demo : public BaseComponent {
public:
    Demo() : BaseComponent("Demo") {}

    int init() override {
        ID_SMGC = Core_getComponentIdxByName("SMGC2D");
        layout = std::make_unique<Layout>(this, WidgetMode::Normal, true);
        layout->setIndent(10.0f);
        layout->setSpace(8.0f);

        layout->push_back(std::make_unique<Label>(this, WidgetMode::Normal, "Привет, SM-GUI!"));

        auto button = std::make_unique<Button>(this, WidgetMode::Normal, "Нажми меня");
        button->onClick = [](Button&) {
            std::clog << "clicked!" << std::endl;
        };
        layout->push_back(std::move(button));

        return 0;
    }

    int tick(long long) override {
        Mouse mouse{};
        sendTask(std::make_unique<task_SmgcGetMouse>(&mouse), ID_SMGC);
        sendTask(std::make_unique<task_SmgcGetWindowSize>(&width, &height), ID_SMGC);

        if (!layout->needsRedraw() && !(mouse != prevMouse) &&
            width == prevWidth && height == prevHeight)
            return 0;

        sendTask(std::make_unique<task_SmgcClearObjects>(), ID_SMGC);
        sendTask(std::make_unique<task_SmgcDrawRect>(
            glm::vec2{0.0f}, glm::vec2{width, height}, glm::vec4{0.1f, 0.1f, 0.1f, 1.0f}), ID_SMGC);

        layout->setPos(0.0f, 0.0f);
        layout->setSize(width, height);
        layout->setMinSize(width, height);
        layout->tick(mouse);
        layout->draw();

        sendTask(std::make_unique<task_SmgcSwapObjects>(), ID_SMGC);

        prevMouse = mouse;
        prevWidth = width;
        prevHeight = height;
        return 0;
    }

private:
    int ID_SMGC = -1;
    std::unique_ptr<Layout> layout;
    Mouse prevMouse{};
    int width = 0, height = 0;
    int prevWidth = 0, prevHeight = 0;
};

int main(int argc, char* argv[]) {
    Core core;
    core.appendComponent(std::make_unique<Config>(argc, argv));
    core.appendComponent(std::make_unique<SMGC2D>());
    core.appendComponent(std::make_unique<Demo>());
    core.init();
    return core.cycle();
}
```

## Архитектура

```
             ┌────────────────────────┐
             │       Core (smpb)      │
             └──┬──────────────────┬──┘
      init/tick │                  │ init/tick
                ▼                  ▼
         ┌───────────┐      ┌───────────────┐
         │  SMGC2D   │◄─────│ Demo (сцена)  │
         │  окно,    │task  │ Layout        │
         │  рендер   │─────►│  ├ Label      │
         └───────────┘      │  ├ Button     │
                            │  └ Spacer     │
                            └───────────────┘
```

`SMGC2D` не знает о виджетах, а виджеты не знают о `SMGC2D` — они общаются только задачами `task_Smgc*`. Виджет хранит указатель на `BaseComponent` (вашу сцену) и `ID_SMGC`, полученный один раз в конструкторе через `Core_getComponentIdxByName("SMGC2D")`, и через него отправляет задачи.

### Базовый виджет

`BaseWidget` — корень иерархии. Поля и методы, которые наследуют все виджеты:

| Поле (protected) | Тип | По умолчанию | Описание |
|------------------|-----|--------------|----------|
| `component` | `BaseComponent*` | `nullptr` | Сцена, через которую отправляются задачи |
| `ID_SMGC` | `int` | `-1` | Индекс компонента `SMGC2D` |
| `pos` | `glm::vec2` | `{0, 0}` | Позиция в пикселях |
| `size` | `glm::vec2` | `{50, 50}` | Текущий размер |
| `minSize` | `glm::vec2` | `{0, 0}` | Минимальный размер, ниже которого виджет не сжимается |
| `mode` | `WidgetMode` | `Normal` | Режим растяжения в раскладке |
| `dirty` | `bool` | `true` | Флаг «нужна перерисовка» |
| `crossFill` | `bool` | `true` | Заполнять ли поперечную ось целиком |

| Метод | Описание |
|-------|----------|
| `BaseWidget(BaseComponent* t_component, WidgetMode t_mode = WidgetMode::Normal)` | Запоминает сцену и режим, находит `ID_SMGC`. |
| `virtual int tick(Mouse& mouse)` | Обрабатывает мышь. По умолчанию ничего не делает, возвращает `0`. |
| `virtual int draw()` | Рисует виджет. Базовая реализация рисует отладочную рамку (серый фон, белая заливка, две красные диагонали) и возвращает `0`, либо `-ENOSYS`, если `component == nullptr`. |
| `virtual bool wantsCrossFill() const` | Заполнять ли поперечную ось. По умолчанию возвращает `crossFill`. Контейнеры переопределяют. |
| `getComponent`, `getPos`, `getSize`, `getMinSize`, `getMode`, `getCrossFill` | Геттеры полей. |
| `setPos`, `setSize`, `setMinSize` | Сеттеры с проверкой «значение изменилось» — при изменении ставят `dirty = true`, а `setSize` и `setMinSize` ещё вызывают `onSizeChanged()`. |
| `setMode`, `setCrossFill` | Сеттеры режима и `crossFill`. |
| `virtual void onSizeChanged()` | Хук, вызываемый при изменении размера. В `Layout` переопределён и запускает `sort()`. |

### Режимы виджетов

`enum class WidgetMode`. Режим говорит раскладке, как обращаться с виджетом вдоль главной оси (вертикально в вертикальной раскладке, горизонтально в горизонтальной) и поперёк неё.

| Режим | Сжатие | Растяжение | Поперечная ось | Применение |
|-------|--------|------------|----------------|------------|
| `Normal` (0) | 1 | 1 | `crossFill` | Обычное поведение. Сжимается, когда не хватает места, тянется, когда есть |
| `Fixed` (1) | 4 (последний) | 0 | `min(crossHint, crossSpace)` | Панель с фиксированным размером. Не растягивается, сжимается в последнюю очередь |
| `Minimal` (2) | 0 (первый) | 0 | `crossFill` | Минимум по главной оси. Не растягивается, сжимается в первую очередь |
| `Maximal` (3) | 2 | 2 | `crossFill` | Среднее между `Normal` и `Extending` |
| `Extending` (4) | 3 | 3 | `crossFill` | Занимает всё свободное место. На нём построен `Spacer` |

Числа в столбцах «сжатие» и «растяжение» — это приоритеты: `sort()` сначала сжимает/растягивает виджеты с большим приоритетом, потом с меньшим. Сначала жертвуют `Minimal`, в конце — `Fixed`.

Поперечная ось: если `crossFill == true`, виджет занимает всю поперечную длину контейнера; иначе — свой `minSize` по поперечной оси, но не больше свободного места. `Fixed` всегда ведёт себя так, будто `crossFill == false`.

### Раскладка

`Layout` — контейнер, который расставляет детей в ряд (горизонтально) или в столбец (вертикально) и распределяет между ними свободное место.

**Ориентация.** Третий аргумент конструктора: `false` — горизонтальная (по умолчанию), `true` — вертикальная.

**Отступы и промежутки.** `setIndent(indent)` задаёт отступ со всех сторон контейнера, `setSpace(space)` — промежуток между детьми. По умолчанию `indent = 5`, `space = 3`.

**Алгоритм `sort()`:**

1. Собирает сумму минимальных размеров детей вдоль главной оси и максимум — вдоль поперечной. Это `minSize` самого контейнера.
2. Если детей нет, помечает себя на перерисовку и выходит.
3. Если суммарный `minSize` детей **меньше** доступного места, каждому ребёнку ставится его минимальный размер, а остаток распределяется по приоритетам растяжения (`Extending` → `Maximal` → `Normal`). `Fixed` и `Minimal` остаток не получают.
4. Если суммарный `minSize` **больше** доступного места, дети сжимаются по приоритетам сжатия (`Minimal` → `Normal` → `Maximal` → `Extending` → `Fixed`). Класс сжатия, в который помещается весь дефицит, пропорционально уменьшается. Если дефицит больше класса, класс обнуляется, а остаток переносится на следующий.
5. Раскладывает детей вдоль главной оси с учётом `indent` и `space`, а вдоль поперечной — по `wantsCrossFill()` или по `minSize`.

`sort()` вызывается автоматически:

- из `onSizeChanged()` — когда у `Layout` меняется размер;
- из `tick()`, если изменился `minSize` любого ребёнка (например, `Label::setText`).

`Layout` переопределяет `wantsCrossFill()` и всегда возвращает `true`: контейнер занимает всю поперечную длину родителя. Если это не нужно, вложите `Layout` в `Layout` с `crossFill == false` у внешнего.

### Отрисовка

`draw()` у `Layout` вызывает `flushPending()` и `draw()` каждого ребёнка по порядку. Порядок детей — это порядок отрисовки, поэтому последний ребёнок оказывается сверху.

Виджеты рисуют себя, отправляя в `SMGC2D` задачи рисования:

- `Label` — одна задача `task_SmgcDrawText` по центру.
- `Button` — фон из четырёх прямоугольников и четырёх треугольников (объёмные скосы) плюс текст по центру.
- `Button2` — то же, но с плоским фоном и полупрозрачным затемнением снизу.
- `Spacer` — ничего, кроме отладочного режима.

Цвета подобраны так, чтобы кнопка «вдавливалась» при нажатии: `state = 1` (наведение) делает цвета светлее, `state = 2` (нажатие) — темнее.

Задачи отправляются через `component->sendTask(..., ID_SMGC)`, поэтому рисуются в буфер B `SMGC2D` и появляются на экране только после `task_SmgcSwapObjects`, которую вы отправляете сами в конце `tick()` своей сцены.

### События мыши

`Mouse` приходит из `SMGC2D` задачей `task_SmgcGetMouse`. Поля, которые используют виджеты:

| Поле | Что означает |
|------|--------------|
| `pos` | Позиция курсора в пикселях |
| `press.x` | `1`, если левая кнопка нажата в этом тике |
| `release.x` | `1`, если левая кнопка отпущена в этом тике |

`Button::tick`:

1. Сбрасывает `state = 0` и `pressed = false`.
2. Если курсор внутри `pos … pos + size` — ставит `state = 1`, а при `press.x > 0.5` ещё и `pressed = true`.
3. Если `pressed && release.x > 0.5` — сбрасывает `pressed` и вызывает `onClick(*this)`.
4. Если `pressed` — ставит `state = 2`.
5. Если курсор вне кнопки — сбрасывает `pressed`, но `onClick` не вызывает (нажатие, начатое на кнопке и отпущенное вне её, отменяется).

Компонент-сцена должен добавляться в ядро **после** `SMGC2D`, иначе на первом тике окно ещё не создано и `task_SmgcGetMouse` вернёт мусор. При правильном порядке каждое нажатие видно ровно один раз.

### Отложенные операции

`Layout` хранит детей в `std::vector<std::unique_ptr<BaseWidget>>`, и `onClick` может добавить или удалить элемент прямо во время `tick()`. Чтобы это было безопасно, мутирующие методы не трогают вектор сразу, а кладут операцию в `opQueue`:

| Метод | Операция |
|-------|----------|
| `push_back(widget)` | Добавить в конец |
| `push_back_penultimate(widget)` | Добавить перед последним. Если детей нет — в конец |
| `pop_back()` | Удалить последний |
| `insert(it, widget)` | Вставить по итератору |
| `erase(it)`, `erase(first, last)` | Удалить по итератору или диапазону |

Очередь применяется в `flushPending()`, который вызывается в начале `tick()` и `draw()`, а также из `count()` и `operator[]`. Правила:

- Итераторы, полученные до `flushPending()`, действительны, пока очередь не применена. Во время `onClick` вектор ещё не изменился.
- Если после применения изменилось число детей, `prevHints` (кэш `minSize` для обнаружения изменений) расширяется, а `dirty` и `redrawNeeded` ставятся в `true`.
- `insert` и `erase` применяются **сразу**, без очереди. Использовать их внутри `onClick` на том же `Layout` нельзя: вектор уже изменится, и итераторы станут недействительными. Для добавления и удаления из обработчика используйте `push_back`, `push_back_penultimate` и `pop_back`.

### Перерисовка

`Layout::needsRedraw()` возвращает `true` ровно один раз после каждого изменения и сбрасывает флаг:

- после `flushPending()`, если очередь что-то применила;
- после `sort()`;
- после `onSizeChanged()`;
- после `erase` и `insert`.

Это позволяет сцене не пересобирать кадр каждый тик: достаточно проверить `needsRedraw()`, сравнить `Mouse` и размер окна с предыдущим тиком и выйти, если ничего не изменилось. Так устроен пример в `example/Scene2D.cpp`.

Однако сам по себе `needsRedraw()` не узнает, что изменился текст `Label` или размер `Button` — эти изменения видны только при вызове `tick()` раскладки. Поэтому проверку `needsRedraw()` нужно делать **после** `tick()` или сравнивать `minSize` детей вручную. В примере сравнение `minSize` уже встроено в `Layout::tick()`.

## Справочник по классам

### BaseWidget

`include/sm-gui/BaseWidget.h`, `src/sm-gui/BaseWidget.cpp`. См. поля и методы в разделе [Базовый виджет](#базовый-виджет). Определяет константы:

| Константа | Значение | Где используется |
|-----------|----------|------------------|
| `smallFontSize` | `9.0f` | Размер мелкого шрифта |
| `mediumFontSize` | `15.0f` | Размер шрифта `Label` и `Button` по умолчанию |
| `bigFontSize` | `20.0f` | Размер крупного шрифта |
| `noneFrameThickness` | `2.0f` | Толщина рамки отладочной отрисовки `BaseWidget` |
| `noneLineThickness` | `5.0f` | Толщина линий отладочной отрисовки |
| `noneIndent` | `7.0f` | Отступ линий отладочной отрисовки |

`WidgetMode` определён здесь же.

### Label

`include/sm-gui/Label.h`, `src/sm-gui/Label.cpp` — наследник `BaseWidget`. Текст с автоматическим размером.

| Поле (protected) | Тип | По умолчанию | Описание |
|------------------|-----|--------------|----------|
| `text` | `std::string` | `""` | Текст |
| `countLines` | `int` | `1` | Число строк (`\n` + 1) |
| `fontSize` | `int` | `mediumFontSize` (`15`) | Размер шрифта в пикселях |
| `indent` | `float` | `labelIndent` (`5`) | Отступ вокруг текста |

| Константа | Значение |
|-----------|----------|
| `labelIndent` | `5.0f` |
| `labelMode` | `WidgetMode::Normal` |
| `labelText` | `"МАМА - ЛУЧШАЯ"` |

| Метод | Описание |
|-------|----------|
| `Label(BaseComponent* t_component, WidgetMode t_mode = labelMode, const std::string t_text = labelText, float t_indent = labelIndent)` | Конструктор. Сразу вызывает `setText(t_text)`. |
| `void setText(const std::string t_text)` | Запоминает текст, считает `countLines`, **синхронно** измеряет его через `task_SmgcMeasureText` и обновляет `minSize` как `измеренный размер + 2 · indent`. Ставит `dirty = true`. Указатели на `measured` и `size` живут до возврата из `sendTask`, поэтому измерение готово сразу. |
| `int drawText()` | Отправляет `task_SmgcDrawText` с выравниванием `TextAlign::Center`. Позиция — центр `size` минус половина высоты текста (`fontSize * countLines * 0.5f`). |
| `int draw() override` | Вызывает `drawText()` и возвращает его код, либо `-ENOSYS`, если `component == nullptr`. |

`Label::tick` наследуется от `BaseWidget` и ничего не делает. Лейбл не реагирует на мышь.

### Button

`include/sm-gui/Button.h`, `src/sm-gui/Button.cpp` — наследник `Label`. Объёмная кнопка с состоянием.

| Поле (protected) | Тип | По умолчанию | Описание |
|------------------|-----|--------------|----------|
| `state` | `int` | `0` | `0` — обычное, `1` — наведение, `2` — нажатие |
| `pressed` | `bool` | `false` | Левая кнопка была нажата внутри и ещё не отпущена |

| Поле (public) | Тип | Описание |
|---------------|-----|----------|
| `onClick` | `std::function<void(Button&)>` | Вызывается один раз при отпускании левой кнопки внутри кнопки. Может быть `nullptr` |

| Константа | Значение |
|-----------|----------|
| `buttonGRIndent` | `4.0f` — отступ внутреннего прямоугольника |
| `buttonLineThickness` | `1.0f` — толщина внешней рамки |
| `buttonMode` | `WidgetMode::Normal` |
| `buttonText` | `"И АЛЕКСАНДР"` |
| `buttonIndent` | `labelIndent + buttonGRIndent + buttonLineThickness` |

| Метод | Описание |
|-------|----------|
| `Button(BaseComponent*, WidgetMode = buttonMode, const std::string t_text = buttonText, float t_indent = buttonIndent, std::function<void(Button&)> t_onClick = nullptr)` | Конструктор, пробрасывает всё в `Label` и запоминает `onClick`. |
| `int tick(Mouse& mouse) override` | См. [События мыши](#события-мыши). |
| `int draw() override` | Рисует фон из четырёх `task_SmgcDrawRect` (чёрная рамка, `colA`, `colB`, плюс два треугольника `colC` на скосах сверху и снизу) и текст через `drawText()`. Цвета зависят от `state`. При `component == nullptr` возвращает `-ENOSYS`. |

Кнопка не двигается и не меняет размер от мыши: раскладка выставляет ей позицию и размер, а `tick` только меняет `state`.

### Button2

`include/sm-gui/Button2.h`, `src/sm-gui/Button2.cpp` — наследник `Button`. Плоская полоса с затемнением.

| Константа | Значение |
|-----------|----------|
| `button2GRIndent` | `2.0f` |
| `button2LineThickness` | `1.0f` |
| `button2Mode` | `WidgetMode::Normal` |
| `button2Text` | `"И АЛЕКСАНДР"` |
| `button2Indent` | `labelIndent + buttonGRIndent + buttonLineThickness` |

| Метод | Описание |
|-------|----------|
| `Button2(BaseComponent*, WidgetMode = button2Mode, const std::string t_text = button2Text, float t_indent = button2Indent, std::function<void(Button&)> t_onClick = nullptr)` | Конструктор. |
| `int draw() override` | Плоский фон с полупрозрачным затемнением снизу (`blackArea = 0.5588`). При нажатии и наведении меняет цвета так же, как `Button`. |

`tick` наследуется от `Button`, поэтому обработка мыши и `onClick` идентичны.

### Spacer

`include/sm-gui/Spacer.h`, `src/sm-gui/Spacer.cpp` — наследник `BaseWidget`. Пустое место в раскладке.

| Константа | Значение |
|-----------|----------|
| `spacerMode` | `WidgetMode::Extending` |

| Метод | Описание |
|-------|----------|
| `Spacer(BaseComponent*, WidgetMode = spacerMode)` | Конструктор. |
| `int draw() override` | В отладочном режиме ядра (`Core_isDebugModeEnabled()`) рисует синюю вертикальную полосу по центру (`task_SmgcDrawRect`). В обычном режиме ничего не делает. Возвращает `0` или `-ENOSYS`, если `component == nullptr`. |

`Spacer` не имеет текста и по умолчанию растягивается: в вертикальной раскладке он забирает всю оставшуюся высоту, в горизонтальной
