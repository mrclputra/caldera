#include "renderer.h"

namespace caldera {
Renderer::Renderer() {
   glEnable(GL_DEPTH_TEST);
   // glEnable(GL_PROGRAM_POINT_SIZE); // to be configured, so i dont forget

   shader = std::make_unique<Shader>("assets/vertex.glsl", "assets/fragment.glsl");
}
Renderer::~Renderer() {
   glDeleteVertexArrays(1, &vao);
   glDeleteBuffers(1, &vbo);
}

void Renderer::render(GLFWwindow *window, Scene &scene, Camera &camera) {
   glClearColor(0.05f, 0.05f, 0.05f, 1.0f);
   glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

   int width, height;
   glfwGetFramebufferSize(window, &width, &height);

   shader->bind();
   shader->setMat4("view", camera.get_view_matrix());
   shader->setMat4("proj", camera.get_proj_matrix(width, height));

   if (scene.pcd) {
      glBindVertexArray(scene.pcd->vao);
      glDrawArrays(GL_POINTS, 0, scene.pcd->vertex_count);
   }

   glfwSwapBuffers(window);
   glfwPollEvents();
}
}  // namespace caldera
