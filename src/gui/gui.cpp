#include "gui.h"
#include "../state.h"

#include <spdlog/spdlog.h>

namespace caldera {

State state;

Gui::Gui(GLFWwindow *window, std::shared_ptr<spdlog::sinks::ringbuffer_sink_mt> g_ring_sink) {

   this->window = window;

   IMGUI_CHECKVERSION();
   ImGui::CreateContext();
   ImGui::StyleColorsDark();

   ImGui_ImplGlfw_InitForOpenGL(this->window, true);
   SPDLOG_INFO("ImGui_ImplGlfw_InitForOpenGL true");

   ImGui::GetIO().FontGlobalScale = 0.7f;

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

   ImGui::PushStyleVar(ImGuiStyleVar_Alpha, 0.5f);
   profiler->draw(ms);

   histogram->compute(pixels, depth, width, height);
   histogram->draw();

   // todo: add loaded filename
   // todo: add button to load a new file
   //    -> need to clear gpu memory
   //    -> need to make sure systems work with no object loaded
   //    -> detach loader from constructors?

   // todo: add the following controls
   // -> min monte-carlo vertex count
   //    -> max monte-carlo vertex count? (later)
   // -> monte-carlo growth rate

   // todo: add alternate methods for viewing point clouds, in preparation for PDAL loading
   //    -> elevation mode
   //    -> rgb mode
   //    -> source mode
   //    -> index mode
   //    -> composite mode

   ImGui::Begin("Controls");
   ImGui::Text("Loaded file: %s", state.loaded_filename.empty() ? "none" : state.loaded_filename.c_str());
   // if (ImGui::Button("Load File")) state.request_load = true;

   ImGui::Separator();
   ImGui::Text("Monte-Carlo");
   ImGui::Text("Initial Count");
   ImGui::SliderInt("##init_points", &state.init_points, 0, static_cast<int>(state.vertex_count / 8));
   ImGui::Text("Growth Rate");
   ImGui::SliderFloat("##growth_rate", &state.growth_rate, 1.0f, 1.5f);

   ImGui::Separator();
   ImGui::Text("View Mode");
   ImGui::Combo("##view_mode", &state.view_mode, "Elevation\0RGB\0Source\0Index\0Composite\0");

   ImGui::PopStyleVar();

   ImGui::End();

   ImGui::Render();
   ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

}  // namespace caldera
