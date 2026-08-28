#pragma once

#include <GLFW/glfw3.h>
#include <glad/glad.h>
#include <spdlog/spdlog.h>

#include <memory>

#include "renderer.h"

namespace caldera {
// TODO: use event system to bind glfw actions

// basic app class for high-level application management and stuff
// events and bindings go here, callbacks should be sourced from here
class App {
 public:
   App();
   void start();

 private:
   void shutdown();

   GLFWwindow *window;
   std::unique_ptr<Renderer> renderer;
};
}  // namespace caldera
