#include "gui.h"

#include <spdlog/spdlog.h>
#include <utility>

namespace caldera {
Gui::Gui(GLFWwindow *window, std::shared_ptr<spdlog::sinks::ringbuffer_sink_mt> g_ring_sink) {
   this->window = window;
   this->g_ring_sink = std::move(g_ring_sink);

   IMGUI_CHECKVERSION();
   ImGui::CreateContext();
   ImGui::StyleColorsDark();

   ImGui_ImplGlfw_InitForOpenGL(this->window, true);
   SPDLOG_INFO("ImGui_ImplGlfw_InitForOpenGL true");

   const char *glsl_version = nullptr;
   ImGui_ImplOpenGL3_Init(glsl_version);

   SPDLOG_INFO("imgui glsl_version: {}", glsl_version);
}
Gui::~Gui() {
   // cleanup
   // IMPORTANT: make sure this is run prior to glfw getting shutdown
   ImGui_ImplOpenGL3_Shutdown();
   ImGui_ImplGlfw_Shutdown();
   ImGui::DestroyContext();

   // TODO: setup imgui flags

   glfwDestroyWindow(window);
   glfwTerminate();
}

void Gui::render() {
   ImGui_ImplOpenGL3_NewFrame();
   ImGui_ImplGlfw_NewFrame();
   ImGui::NewFrame();

   // ImGui::ShowDemoWindow(); // demo window
   draw_log_overlay();

   ImGui::Render();
   ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void Gui::draw_log_overlay() {
   const ImGuiViewport *vp = ImGui::GetMainViewport();
   ImGui::SetNextWindowPos({vp->WorkPos.x + 10.0f, vp->WorkPos.y + vp->WorkSize.y - 10.0f}, ImGuiCond_Always, {0.0f, 1.0f});

   ImGuiWindowFlags flags = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoSavedSettings |
                            ImGuiWindowFlags_NoNav | ImGuiWindowFlags_NoInputs;
   ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
   ImGui::Begin("log", nullptr, flags);
   // ImGui::SetWindowFontScale(0.7f);

   for (const auto &msg : g_ring_sink->last_raw()) {
      ImVec4 color = msg.level == spdlog::level::err    ? ImVec4(1.0f, 0.3f, 0.3f, 1.0f)
                     : msg.level == spdlog::level::warn ? ImVec4(1.0f, 0.8f, 0.2f, 1.0f)
                                                        : ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
      ImGui::TextColored(color, "%s", std::string(msg.payload.data(), msg.payload.size()).c_str());
   }

   ImGui::End();
   ImGui::PopStyleVar();
}

}  // namespace caldera
