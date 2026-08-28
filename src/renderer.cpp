#include "renderer.h"

namespace caldera {
Renderer::Renderer() {
   glEnable(GL_DEPTH_TEST);
   // glEnable(GL_PROGRAM_POINT_SIZE); // to be configured, so i dont forget

   shader = std::make_unique<Shader>("assets/vertex.glsl", "assets/fragment.glsl");

   // make sure the loader has already loaded something in advance
}
Renderer::~Renderer() {
   glDeleteVertexArrays(1, &vao);
   glDeleteBuffers(1, &vbo);
}

void Renderer::render(GLFWwindow *window, Scene &scene) {
   glClearColor(0.05f, 0.05f, 0.05f, 1.0f);
   glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

   shader->bind();
   if (scene.pcd) {
      glBindVertexArray(scene.pcd->vao);
      glDrawArrays(GL_POINTS, 0, scene.pcd->vertex_count);
   }

   glfwSwapBuffers(window);
   glfwPollEvents();
}
}  // namespace caldera
