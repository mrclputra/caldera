#include "gui.h"

#include <spdlog/spdlog.h>

namespace caldera {
Gui::Gui(
    GLFWwindow *window,
    std::shared_ptr<spdlog::sinks::ringbuffer_sink_mt> g_ring_sink) {

   this->window = window;

   IMGUI_CHECKVERSION();
   ImGui::CreateContext();
   ImGui::StyleColorsDark();

   ImGui_ImplGlfw_InitForOpenGL(this->window, true);
   SPDLOG_INFO("ImGui_ImplGlfw_InitForOpenGL true");

   const char *glsl_version = nullptr;
   ImGui_ImplOpenGL3_Init(glsl_version);

   SPDLOG_INFO("imgui glsl_version: {}", glsl_version);

   log = std::make_unique<Log>(g_ring_sink);
   histogram = std::make_unique<Histogram>();
   profiler = std::make_unique<Profiler>();
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

void Gui::render(double ms, const unsigned char *pixels, const float *depth, int width, int height) {
   ImGui_ImplOpenGL3_NewFrame();
   ImGui_ImplGlfw_NewFrame();
   ImGui::NewFrame();

   log->draw();
   profiler->draw(ms);

   // histogram->compute(pixels, depth, width, height);
   // histogram->draw();

   ImGui::Render();
   ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

}  // namespace caldera
