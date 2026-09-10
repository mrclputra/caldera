#pragma once

#include <imgui.h>
#include <imgui_impl_opengl3.h>
#include <imgui_impl_glfw.h>
#include <GLFW/glfw3.h>
#include <spdlog/sinks/ringbuffer_sink.h>

#include "histogram.h"
#include "profiler.h"
#include "log.h"

// todo:
//    - figure out how to plug loader into the gui
//    - would this require some kind of event system to be implemented?

namespace caldera {
// this is a wrapper class for imgui
class Gui {
 public:
   Gui(GLFWwindow *window, std::shared_ptr<spdlog::sinks::ringbuffer_sink_mt> g_ring_sink);
   ~Gui();
   void render(double ms, const unsigned char *pixels, const float *depth, int width, int height);  // draw the gui
 private:
   GLFWwindow *window;

   std::unique_ptr<Log> log;
   std::unique_ptr<Histogram> histogram;
   std::unique_ptr<Profiler> profiler;
};
}  // namespace caldera
