/**
 * Author: TheRetikGM
 * Date: 2022-12-27
 * Licence: MIT
 * -----------------------------
 * UI for editing SwayWM inputs.
*/
#include <iostream>
#include <string>
#include <vector>
#include <array>
#include <cstdlib>
#include <cstring>
#include <regex>
#include <sstream>
#include <map>

#include <imguiwrapper.hpp>
#include <nlohmann/json.hpp>

#define perr(fmt) fprintf(stderr, fmt)
#define perr_e(fmt) fprintf(stderr, "error -- " fmt "\n")
#define WIN_WIDTH 520
#define WIN_HEIGHT 650
#define WIN_TITLE "Input toolkit"

using json = nlohmann::json;
typedef std::map<std::string, std::string> Config;
using namespace ImWrap;

struct Input {
  std::string id = "";
  std::string name = "";
  std::string type = "";

  Config kb_conf;
  Config tablet_conf;
  Config map_conf;
  Config libin_conf;
};

bool show_demo = false;

void get_inputs(std::vector<Input>& inputs)
{
  inputs.clear();
  FILE* fp = NULL;
  fp = popen("/sbin/swaymsg -t get_inputs", "r");
  if (!fp) {
    perr_e("Could not call /sbin/swaymsg.");
    std::exit(1);
  }

  // Get the json output and store it inside nlohmann_json structure.
  char str[200] = {};
  std::stringstream json_steam;
  while (fgets(str, 99, fp) != NULL) {
    std::string line(str);
    json_steam << line;
  }
  json j;
  json_steam >> j;
  json_steam.clear();

  // Create inputs from structure.
  for (auto& input : j) {
    Input i;
    input["identifier"].get_to(i.id);
    input["name"].get_to(i.name);
    input["type"].get_to(i.type);
    inputs.push_back(i);
  }

  pclose(fp);
}

bool input_getter(void* data, int idx, const char** selected)
{
  std::vector<Input>* inputs = (std::vector<Input>*)data;
  if ((size_t)idx >= inputs->size())
    return false;
  *selected = (*inputs)[idx].name.c_str();
  return true;
}

void imgui_kb_config(Config& kb_conf)
{
  (void)kb_conf;
  ImGui::LabelText("Config", "Value");
  static int repeat_delay = 0;
  ImGui::SliderInt("repeat_delay", &repeat_delay, 0, 2000, "%d ms");
  static int repeat_rate = 0;
  ImGui::SliderInt("repeat_rate", &repeat_rate, 0, 20, "%d cps");
  static std::string xkb_layouts = ""; // Comma seperated list of layouts.
  ImGui::InputText("xkb_layouts", &xkb_layouts);
  static std::string xkb_model = "";
  ImGui::InputText("xkb_model", &xkb_model);
  static std::string xkb_options = "";
  ImGui::InputText("xkb_options", &xkb_options);
  static std::string xkb_rules = "";
  ImGui::InputText("xkb_rules", &xkb_rules);
  static std::string xkb_switch_layout = "";
  ImGui::InputText("xkb_switch_layout", &xkb_switch_layout);
  static std::string xkb_variant = "";
  ImGui::InputText("xkb_variant", &xkb_variant);
  static bool capslock = false;
  ImGui::Checkbox("Capslock", &capslock);
  static bool numlock = false;
  ImGui::Checkbox("Numlock", &numlock);
}

void imgui_enum_combo(const char* label, int* v, std::vector<const char*> enums)
{
  ImGui::Combo(label, v, enums.data(), enums.size());
}

void imgui_tablet_config(Config& tb_config)
{
  (void)tb_config;
  ImGui::Text("Tool Mode");
  ImGui::Indent();
  {
    static int tool = 0;
    imgui_enum_combo("Tool", &tool, { "pen", "eraser", "brush", "pencil", "airbrush" });
    static int pos = 1;
    imgui_enum_combo("Position", &pos, { "absolute", "relative" });
  }
  ImGui::Unindent();
}

void imgui_map_config(Config& map_config)
{
  (void)map_config;
  ImGui::Text("Map to region");
  ImGui::Indent();
  {
    static int pos[2] = {};
    static int size[2] = {};
    ImGui::InputInt2("Top left corner", pos);
    ImGui::InputInt2("Size", size);
    if (ImGui::Button("Select..")) {
      FILE* fp = popen("/sbin/slurp -f \"%x %y %w %h\"", "r");
      if (fp) {
        fscanf(fp, "%i %i %i %i", pos, pos + 1, size, size + 1);
        pclose(fp);
      }
    }
  }
  ImGui::Unindent();
  ImGui::Text("Map from region");
  ImGui::Indent();
  {
    static int X1Y1[2] = {};
    static int X2Y2[2] = {};
    ImGui::InputInt2("X1 Y1", X1Y1);
    ImGui::InputInt2("X2 Y2", X2Y2);
  }
  ImGui::Unindent();
}

void imgui_libin_config(Config& libin_config)
{
  (void)libin_config;
  static int input_profile = 0;
  imgui_enum_combo("input_profile", &input_profile, { "adaptive", "flat" });
  ImGui::Text("Calibration matrix");
  ImGui::Indent();
  {
    static float cal_mat[6] = {};
    ImGui::InputFloat3("##cal_mat1", cal_mat);
    ImGui::InputFloat3("##cal_mat2", cal_mat + 3);
  }
  ImGui::Unindent();
  static int click_method = 0;
  imgui_enum_combo("click_method", &click_method, { "none", "button_areas", "clickfinger" });
  static bool drag = false;
  ImGui::Checkbox("drag", &drag);
  static bool drag_lock = false;
  ImGui::Checkbox("drag_lock", &drag_lock);
  static bool dwt = false;
  ImGui::Checkbox("dwt", &dwt);
  static int events = 0;
  imgui_enum_combo("events", &events, { "enabled", "disabled", "disabled_on_external_mouse", "toggle" });
  static bool left_handed = false;
  ImGui::Checkbox("left_handed", &left_handed);
  static bool middle_emulation = false;
  ImGui::Checkbox("middle_emulation", &middle_emulation);
  static bool natural_scroll = false;
  ImGui::Checkbox("natural_scroll", &natural_scroll);
  static int pointer_accel = 0;
  imgui_enum_combo("pointer_accel", &pointer_accel, { "-1", "1" });
  ImGui::Text("scroll_button");
  ImGui::Indent();
  {
    std::string scroll_button_val = "";
    ImGui::InputTextWithHint("##event-code-or-name", "button[1-3,8,9]|<event-code-or-name>", &scroll_button_val);
    static bool scroll_button_disable = false;
    ImGui::Checkbox("Disable", &scroll_button_disable);
  }
  ImGui::Unindent();
  static float scroll_factor = 1.0f;
  ImGui::InputFloat("scroll_factor", &scroll_factor);
  static int scroll_method = 0;
  imgui_enum_combo("scroll_method", &scroll_method, { "none", "two_finger", "edge", "on_button_down" });
  static bool tap = false;
  ImGui::Checkbox("tap", &tap);
  static int tap_button_map = 0;
  imgui_enum_combo("tap_button_map", &tap_button_map, { "lrm", "lmr" });
}

void imgui_input_settings(Input& input)
{
  ImGui::LabelText("Identifier", "%s", input.id.c_str());
  ImGui::LabelText("Type", "%s", input.type.c_str());

  ImGui::BeginTabBar("Input Commands");
  if (ImGui::BeginTabItem("Keyboard")) {
    imgui_kb_config(input.kb_conf);
    ImGui::EndTabItem();
  }
  
  if (ImGui::BeginTabItem("Tablet")) {
    imgui_tablet_config(input.tablet_conf);
    ImGui::EndTabItem();
  }

  if (ImGui::BeginTabItem("Mapping")) {
    imgui_map_config(input.map_conf);
    ImGui::EndTabItem();
  }

  if (ImGui::BeginTabItem("LibInput")) {
    imgui_libin_config(input.libin_conf);
    ImGui::EndTabItem();
  }
  ImGui::EndTabBar();
}

void on_imgui(std::vector<Input>& inputs)
{
  if (ImGui::BeginMainMenuBar()) {
    if (ImGui::BeginMenu("File")) {
      ImGui::MenuItem("Open..", "Ctrl+O");
      ImGui::MenuItem("Save", "Ctrl+S");
      ImGui::MenuItem("Save as..");
      if (ImGui::MenuItem("Exit", "Esc"))
        Context::Instance()->Close();
      ImGui::EndMenu();
    }
    ImGui::EndMainMenuBar();
  }

  ImGuiWindowFlags main_win_flags = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoSavedSettings;
  const ImGuiViewport* viewport = ImGui::GetMainViewport();
  ImGui::SetNextWindowPos(viewport->WorkPos);
  ImGui::SetNextWindowSize(viewport->Size);

  ImGui::Begin("Sway input configurator", nullptr, main_win_flags);

  static int selected_input = 0;
  ImGui::Combo("Input", &selected_input, input_getter, &inputs, inputs.size());
  if (ImGui::Button("Refresh")) {
    get_inputs(inputs);
    selected_input = 0;
  }
  ImGui::Separator();
  imgui_input_settings(inputs[selected_input]);
  ImGui::Separator();

  ImGui::Button("Apply");
  ImGui::End();

  ImGui::Begin("Test");
  ImGui::End();
}

struct Core
{
  std::vector<Input> inputs{};

  void OnCreate()
  {
    get_inputs(inputs);
  }
  void OnUpdate(float dt)
  {
    (void)dt;
    on_imgui(inputs);

    static bool show_demo = false;
    if (ImGui::IsKeyPressed(ImGuiKey_F1, false))
      show_demo = !show_demo;
    if (show_demo)
      ImGui::ShowDemoWindow();
  }
  void OnClose() {}
};

int main()
{
  ContextDefinition def;
  def.window_width = WIN_WIDTH;
  def.window_height = WIN_HEIGHT;
  def.window_title = WIN_TITLE;
  def.window_hints[GLFW_RESIZABLE] = GLFW_FALSE;
  def.imgui_config_flags &= ~ImGuiConfigFlags_DockingEnable;

  Context* context = nullptr;
  Core c;

  try {
    context = Context::Create(def);
    run(context, c);
  } catch (const std::runtime_error& e) {
    std::cerr << e.what() << std::endl;
    return 1;
  }

  Context::Destroy(context);
  return 0;
}
