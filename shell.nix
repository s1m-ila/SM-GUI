{ pkgs ? import <nixpkgs> {} }:

pkgs.mkShell {
  # Пакеты, необходимые для компиляции и линковки
  buildInputs = with pkgs; [
    # Инструменты сборки
    cmake
    gcc
    pkg-config

    # GLFW
    glfw

    # OpenGL
    libGL
    libGLU
    glfw

    # GLM
    glm

    # Дополнительные зависимости для GLFW (X11 и т.д.)
    libX11
    libXrandr
    libXcursor
    libXi
    libXinerama
    libXext
  ];

  # Переменные окружения для работы во время выполнения
  # Это критично для загрузки слоев валидации и библиотек
  LD_LIBRARY_PATH = pkgs.lib.makeLibraryPath (with pkgs; [
    glfw
    libGL
    libGLU
    vulkan-loader
    vulkan-validation-layers
    libX11
    libXrandr
    libXcursor
    libXi
    libXinerama
    libXext
  ]);

  # Указываем путь к слоям валидации
  VK_LAYER_PATH = "${pkgs.vulkan-validation-layers}/share/vulkan/explicit_layer.d";

  # Некоторые туториалы ожидают эту переменную, хотя в NixOS она может быть не нужна
  # VULKAN_SDK = "${pkgs.vulkan-headers}";
}
