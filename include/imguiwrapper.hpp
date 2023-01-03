/**
 * ---------------------------------------------------------
 * Author: TheRetikGM
 * License: MIT
 * ---------------------------------------------------------
 * Contains wrapper objects for working with ImGui using OpenGL API.
 *
 * Note: namespace ImWrap
 * Usage:
 *    1/ Create ContextDefinition and customize it.
 *    2/ Create Context using the Context::Create() method.
 *    3/ Define an class with OnCreate(), OnUpdate(float) and OnClose() methods.
 *    4/ Use run using the context and object instance.
 *    5/ Delete the context using Context::Delete() method.
 */
#pragma once
#include <map>
#include <stdint.h>
#include <exception>
#include <stdexcept>
#include <memory>
#include <concepts>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <imgui.h>
#ifdef IMGUIWRAPPER_IMPLOT
  #include <implot.h>
#endif
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>
#include <misc/cpp/imgui_stdlib.h>

namespace ImWrap
{

  enum class ImGuiTheme : int { classic = 0, light, dark };

  struct ContextDefinition
  {
    uint32_t window_width = 800;
    uint32_t window_height = 600;
    std::string window_title = "ImGui wrapper!";
    std::map<int, int> window_hints = { 
      { GLFW_CONTEXT_VERSION_MAJOR, 3 },  
      { GLFW_CONTEXT_VERSION_MINOR, 3 },  
      { GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE }
    };
    // Enable VSync.
    uint32_t swap_interval = 1;
    int exit_key = GLFW_KEY_ESCAPE;
    // By default only exit key is handled. For the other keys, you can use ImGui functions.
    GLFWkeyfun custom_key_callback = nullptr;
    
    ImGuiTheme imgui_theme = ImGuiTheme::classic; 
    ImGuiConfigFlags imgui_config_flags = ImGuiConfigFlags_DockingEnable;
    // Allow imgui windows to be dragged outside of the current viewport.
    bool imgui_multiviewport = false;
  };

  class Context
  {
    struct Callbacks {
      static void key(GLFWwindow* window, int key, int scancode, int action, int mods)
      {
        (void)scancode; (void)mods;
        if (!Context::m_instance)
          return;
        if (key == Context::m_instance->m_def.exit_key && action == GLFW_PRESS)
          glfwSetWindowShouldClose(window, true);
      }

      static void fb_size(GLFWwindow* window, int width, int height)
      {
        (void)window;
        if (!Context::m_instance)
          return;
        Context::m_instance->m_windowWidth = width;
        Context::m_instance->m_windowHeight = height;
        glViewport(0, 0, width, height);
      }
      
      static void error(int code, const char* description)
      {
        throw std::runtime_error("GLFW error! Code: " + std::to_string(code) + ". Desc: " + std::string(description));
      }
    };

  public:
    GLFWwindow* m_Window = nullptr;

    static Context* Create(const ContextDefinition& def)
    {
      return m_instance ? m_instance : (m_instance = new Context(def));
    }
    static void Destroy(Context* context)
    {
      if (context)
        delete context;
    }

    inline static Context* Instance() { return m_instance; }
    inline const ContextDefinition& GetDefinition() const { return m_def; }
    inline uint32_t GetWidth() const { return m_windowWidth; }
    inline uint32_t GetHeight() const { return m_windowHeight; }
    inline void Close() { glfwSetWindowShouldClose(m_Window, true); }

    void SetImGuiTheme(ImGuiTheme theme)
    {
      switch (theme) {
        case ImGuiTheme::classic: ImGui::StyleColorsClassic(); break;
        case ImGuiTheme::light: ImGui::StyleColorsLight(); break;
        case ImGuiTheme::dark: ImGui::StyleColorsDark(); break;
      }
    }

  private:
    static inline Context* m_instance = nullptr;
    ContextDefinition m_def{};
    uint32_t m_windowWidth = 0;
    uint32_t m_windowHeight = 0;

    Context(const ContextDefinition& context_definition) 
      : m_def(context_definition)
      , m_windowWidth(m_def.window_width)
      , m_windowHeight(m_def.window_height)
    {
      glfwSetErrorCallback(Callbacks::error);
      glfwInit();

      // Set window hints.
      char glsl_version[20] = {};
      sprintf(glsl_version, "#version %1i%1i0", m_def.window_hints[GLFW_CONTEXT_VERSION_MAJOR], m_def.window_hints[GLFW_CONTEXT_VERSION_MINOR]);
      for (auto& [key, value] : m_def.window_hints)
        glfwWindowHint(key, value);

      // Create GLFW window and set its properties.
      m_Window = glfwCreateWindow(m_def.window_width, m_def.window_height, m_def.window_title.c_str(), NULL, NULL);
      if (!m_Window)
        throw std::runtime_error("Failed to create GLFW window.");
      glfwSetKeyCallback(m_Window, m_def.custom_key_callback ? m_def.custom_key_callback : Callbacks::key); 
      glfwSetFramebufferSizeCallback(m_Window, Callbacks::fb_size);
      glfwMakeContextCurrent(m_Window);
      glfwSwapInterval(m_def.swap_interval);

      // Load OpenGL functions.
      if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        glfwDestroyWindow(m_Window);
        glfwTerminate();
        throw std::runtime_error("Failed to initialize OpenGL context.");
      }

      // Setup imgui.
      ImGui::CreateContext();
#ifdef IMGUIWRAPPER_IMPLOT
      ImPlot::CreateContext();
#endif
      SetImGuiTheme(m_def.imgui_theme);
      ImGui::GetIO().ConfigFlags |= m_def.imgui_config_flags;
      if (m_def.imgui_multiviewport)
        ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
      ImGui_ImplGlfw_InitForOpenGL(m_Window, true);
      ImGui_ImplOpenGL3_Init(glsl_version);
    }

    ~Context()
    {
      ImGui_ImplOpenGL3_Shutdown();
      ImGui_ImplGlfw_Shutdown();
#ifdef IMGUIWRAPPER_IMPLOT
      ImPlot::CreateContext();
#endif
      ImGui::DestroyContext();
      glfwDestroyWindow(m_Window);
      glfwTerminate();
    }
  };

  // Check for existing methods using C++20 concepts (who needs inheritance lol).
  template<typename T> concept IHasOnCreate = requires(T& t) { t.OnCreate(); };
  template<typename T> concept IHasOnUpdate = requires(T& t) { t.OnUpdate(0.0f); };
  template<typename T> concept IHasOnClose = requires(T& t) { t.OnClose(); };

  // Run main loop on given context.
  // Object has to have atleast OnUpdate(float) method.
  // Optional methods are:
  //    OnCreate();
  //    OnClose();
  template<class TObj>
  requires IHasOnUpdate<TObj>
  void run(const Context* context, TObj& obj)
  {
    if constexpr (IHasOnCreate<TObj>)
      obj.OnCreate();

    float last_tm = 0.0f, this_tm = 0.0f;
    float dt = 0.0f; 

    glDisable(GL_DEPTH_TEST);

    // Main loop.
    while(!glfwWindowShouldClose(context->m_Window))
    {
      // Update deltatime.
      this_tm = (float)glfwGetTime();
      dt = last_tm - this_tm; 
      last_tm = this_tm;

      // Update ImGui frame.
      ImGui_ImplOpenGL3_NewFrame();
      ImGui_ImplGlfw_NewFrame();
      ImGui::NewFrame();

      // Reset viewport and clear it.
      glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
      glClear(GL_COLOR_BUFFER_BIT);

      // User-defined update function.
      obj.OnUpdate(dt);

      // Draw ImGui.
      ImGui::Render();
      ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

      // Update and render additional ImGui platform windows.
      if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
        ImGui::UpdatePlatformWindows();
        ImGui::RenderPlatformWindowsDefault();
        glfwMakeContextCurrent(context->m_Window);
      }

      // Swap buffers and process events.
      glfwSwapBuffers(context->m_Window);
      glfwPollEvents();
    }

    if constexpr (IHasOnClose<TObj>)
      obj.OnClose();
  }
} // namespace ImWrap
