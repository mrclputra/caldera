#include "gui.h"

#include <spdlog/spdlog.h>

namespace caldera {
Gui::Gui(GLFWwindow *window) {
   this->window = window;
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
   ImGui_ImplOpenGL3_Shutdown();
   ImGui_ImplGlfw_Shutdown();
   ImGui::DestroyContext();

   glfwDestroyWindow(window);
   glfwTerminate();
}

void Gui::render() {
   // deltatime? would be nice...

   ImGui_ImplOpenGL3_NewFrame();
   ImGui_ImplGlfw_NewFrame();
   ImGui::NewFrame();

   ImGui::ShowDemoWindow();

   ImGui::Render();
   ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

}  // namespace caldera
