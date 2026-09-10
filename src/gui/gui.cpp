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

   histogram = std::make_unique<Histogram>();
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

   // ImGui::ShowDemoWindow(); // demo window
   draw_log_overlay();
   draw_info_overlay(ms, pixels, depth, width, height);

   ImGui::Render();
   ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void Gui::draw_log_overlay() {
   const ImGuiViewport *vp = ImGui::GetMainViewport();
   ImGui::SetNextWindowPos({vp->WorkPos.x + 10.0f, vp->WorkSize.y - 10.0f}, ImGuiCond_Always, {0.0f, 1.0f});

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

void Gui::draw_info_overlay(double ms, const unsigned char *pixels, const float *depth, int width, int height) {
   const ImGuiViewport *vp = ImGui::GetMainViewport();

   // todo: set window flags here
   // todo: set window style here

   // profiler stuff
   static const int FRAME_HIST_COUNT = 2000;
   static float frame_time_history[FRAME_HIST_COUNT] = {0};
   static int frame_time_offset = 0;
   static int frame_time_count = 0;

   frame_time_history[frame_time_offset] = (float)ms;
   frame_time_offset = (frame_time_offset + 1) % FRAME_HIST_COUNT;
   if (frame_time_count < FRAME_HIST_COUNT) frame_time_count++;

   float sum = 0.0f, max = 0.0f;
   for (float v : frame_time_history) {
      sum += v;
      if (v > max) max = v;
   }
   float avg = sum / frame_time_count;

   ImGui::Begin("profiler", nullptr, NULL);

   ImGui::Text("Current: %.2f ms", ms);
   ImGui::Text("Average: %.2f ms (%.1f fps)", avg, (avg > 0.0f) ? 1000.0f / avg : 0.0f);

   ImGui::PlotLines("", frame_time_history, FRAME_HIST_COUNT, frame_time_offset, nullptr, 0.0f, max + 2.0f, ImVec2(ImGui::GetContentRegionAvail().x, 80));

   ImGui::TextColored(ImVec4(0.5f, 0.5f, 0.5f, 1.0f), "Max: %.1f ms", max);

   histogram->compute(pixels, depth, width, height);
   histogram->draw();

   ImGui::End();
}

}  // namespace caldera
