#pragma once

#include <imgui.h>
#include <imgui_impl_opengl3.h>
#include <imgui_impl_glfw.h>
#include <GLFW/glfw3.h>
#include <spdlog/sinks/ringbuffer_sink.h>

#include "histogram.h"

// TODO:
//    - add an info panel and buttons to load new point clouds and stuff
//    - panel docking? also make it static
//    - custom color theme (creamy)

namespace caldera {
// this is a wrapper class for imgui
class Gui {
 public:
   Gui(GLFWwindow *window, std::shared_ptr<spdlog::sinks::ringbuffer_sink_mt> g_ring_sink);
   ~Gui();
   void render(double ms, const unsigned char *pixels, const float *depth, int width, int height);  // draw the gui
 private:
   std::shared_ptr<spdlog::sinks::ringbuffer_sink_mt> g_ring_sink;
   GLFWwindow *window;

   std::unique_ptr<Histogram> histogram;

   void draw_log_overlay();
   void draw_info_overlay(double ms, const unsigned char *pixels, const float *depth, int width, int height);
};
}  // namespace caldera
