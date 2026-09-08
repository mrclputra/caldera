#pragma once

#include <imgui.h>
#include <imgui_impl_opengl3.h>
#include <imgui_impl_glfw.h>
#include <GLFW/glfw3.h>
#include <spdlog/sinks/ringbuffer_sink.h>

namespace caldera {
// this is a wrapper class for imgui
class Gui {
 public:
   Gui(GLFWwindow *window, std::shared_ptr<spdlog::sinks::ringbuffer_sink_mt> g_ring_sink);
   ~Gui();
   void render();  // draw the gui
 private:
   std::shared_ptr<spdlog::sinks::ringbuffer_sink_mt> g_ring_sink;
   GLFWwindow *window;

   // ui functions here
   void draw_log_overlay();
};
}  // namespace caldera
