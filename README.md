# Dear ImGui wrapper
My wrapper C++ project for Dear ImGui and more.

## Requirements
- meson
- C++20

## Includes
- [Dear ImGui](https://github.com/ocornut/imgui)
- [ImPlot](https://github.com/epezent/implot)
- [Nlohmann's Json](https://github.com/nlohmann/json)
- [YAML cpp](https://github.com/jbeder/yaml-cpp)
- Glad with OpenGL 3.3 core support

## Code example
This is how you would create simple ImGui + ImPlot application.

main.cpp
```C++
#include <iostream>
#include <imguiwrapper.hpp>   // Requires C++20
#include <implot.h>


struct Demo
{
  // Optional
  void OnCreate() { std::cout << "Demo started!" << std::endl; }

  void OnUpdate(float dt)
  {
    ImGui::DockSpaceOverViewport(ImGui::GetMainViewport());
    ImPlot::ShowDemoWindow();
  }

  // Optional
  void OnClose() { std::cout << "Demo closed :(" << std::endl; }
};

int main()
{
  ImWrap::ContextDefinition def;
  def.window_width = 800;
  def.window_height = 600;
  def.window_title = "Example app!";
  def.window_hints[GLFW_RESIZABLE] = GLFW_TRUE;   // Is set by default. This is only example of hints.
  def.imgui_multiviewport = true;                 // Allow ImGui windows to be dragged out of the main window.
  def.imgui_theme = ImWrap::ImGuiTheme::dark;

  try {
    Demo demo;
    auto context = ImWrap::Context::Create(def);
    ImWrap::run(context, demo);
    ImWrap::Context::Destroy(context);
  }
  catch (const std::runtime_error& e) {
    std::cerr << e.what() << std::endl;
    return 1;
  }

  return 0;
}
```

## Example meson config
- Create `subprojects` directory.
- Create `subprojects/imguiwrapper.hpp` file with contents:
  ```
  [wrap-git]
  url = https://github.com/theretikgm/imguiwrapper
  revision = head
  depth = 1
  
  [provides]
  imguiwrapper = imguiwrapper_dep
  ```
- Create `meson.build` file next to the `main.cpp` (from Code example)
  ```meson
  project('example', 'cpp', 
    version : '0.1',
    default_options : ['warning_level=3',
                       'cpp_std=c++20'])
  
  deps = [
    dependency('imguiwrapper', default_options : ['implot=enabled'])
  ]
  
  executable('main', 'main.cpp', dependencies : deps)
  ```
  - To enable other libraries you would need to enable other options (by default they are disabled):
    ```meson
    dependency('imguiwrapper', 
      default_options : [
        'implot=enabled',
        'nlohmann_json=enabled',
        'glm=enabled',
        'yaml-cpp=enabled']
    )
    ```
- Thats all :)

## Build Linux

    meson setup build -Dbuildtype=release
    meson compile -C build

## Build Windows
Install:
  - meson using python's pip (winget install python3)
  - Windows Visual Studio C++ build tools

In Developer console run:

    meson setup build
    meson compile -C build -Dbuildtype=release
