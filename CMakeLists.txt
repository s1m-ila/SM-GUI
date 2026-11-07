cmake_minimum_required(VERSION 3.16)

project(sm-gui LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(PROJECT_IS_TOP_LEVEL)
  if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    set(CMAKE_BUILD_TYPE Release)
  endif()
endif()

# --- smpb ---
if(NOT TARGET smpb::core)
  if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/../smpb/CMakeLists.txt)
    add_subdirectory(../smpb smpb)
  else()
    include(FetchContent)
    FetchContent_Declare(smpb
      GIT_REPOSITORY https://github.com/s1m-ila/SMPB.git
      GIT_TAG        main
    )
    FetchContent_MakeAvailable(smpb)
  endif()
endif()

# --- smgc ---
if(NOT TARGET smgc::display2D)
  if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/../smgc/CMakeLists.txt)
    add_subdirectory(../smgc smgc)
  else()
    include(FetchContent)
    FetchContent_Declare(smgc
      GIT_REPOSITORY https://github.com/s1m-ila/SMGC.git
      GIT_TAG        main
    )
    FetchContent_MakeAvailable(smgc)
  endif()
endif()

# --- sm-gui::elements ---
add_library(sm-gui_elements STATIC
  src/sm-gui/BaseWidget.cpp
  src/sm-gui/Label.cpp
  src/sm-gui/Button.cpp
  src/sm-gui/Button2.cpp
  src/sm-gui/Spacer.cpp
  src/sm-gui/Layout.cpp
)
add_library(sm-gui::elements ALIAS sm-gui_elements)
set_target_properties(sm-gui_elements PROPERTIES CXX_EXTENSIONS OFF)
target_compile_features(sm-gui_elements PUBLIC cxx_std_23)

target_include_directories(sm-gui_elements
  PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

target_link_libraries(sm-gui_elements
  PUBLIC
    smpb::core
    smgc::display2D
)

option(SM_GUI_BUILD_EXAMPLE "Build sm-gui example" ${PROJECT_IS_TOP_LEVEL})
if(SM_GUI_BUILD_EXAMPLE)
  add_executable(sm-gui
      example/main.cpp
      example/Scene2D.h example/Scene2D.cpp
  )
  target_link_libraries(sm-gui PRIVATE sm-gui::elements)
  set_target_properties(sm-gui PROPERTIES CXX_EXTENSIONS OFF)
  smgc_copy_files(sm-gui)

  include(GNUInstallDirs)
  install(TARGETS sm-gui RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})

  set(FILES
      "core.config"
  )

  foreach(FILE ${FILES})
      add_custom_command(
          TARGET sm-gui POST_BUILD
          COMMAND ${CMAKE_COMMAND} -E copy_if_different
          "${CMAKE_CURRENT_SOURCE_DIR}/files/${FILE}"
          "$<TARGET_FILE_DIR:sm-gui>/files/${FILE}"
          COMMENT "Copying file ${FILE} to build directory..."
      )
  endforeach()
endif()
