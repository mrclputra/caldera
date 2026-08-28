#include "renderer.h"

namespace caldera {
Renderer::Renderer() {
   glEnable(GL_DEPTH_TEST);
   // glEnable(GL_PROGRAM_POINT_SIZE); // to be configured, so i dont forget

   shader = std::make_unique<Shader>("assets/vertex.glsl", "assets/fragment.glsl");

   float vertices[] = {
       -0.5f, -0.5f, 0.0f,  // left
       0.5f, -0.5f, 0.0f,   // right
       0.0f, 0.5f, 0.0f     // top
   };

   glGenVertexArrays(1, &vao);
   glGenBuffers(1, &vbo);

   glBindVertexArray(vao);
   glBindBuffer(GL_ARRAY_BUFFER, vbo);
   glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

   glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);
   glEnableVertexAttribArray(0);

   glBindBuffer(GL_ARRAY_BUFFER, 0);
   glBindVertexArray(0);
}
Renderer::~Renderer() {
   glDeleteVertexArrays(1, &vao);
   glDeleteBuffers(1, &vbo);
}

void Renderer::render(GLFWwindow *window) {
   glClearColor(0.05f, 0.05f, 0.05f, 1.0f);
   glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

   shader->bind();
   glBindVertexArray(vao);
   glDrawArrays(GL_TRIANGLES, 0, 3);

   glfwSwapBuffers(window);
   glfwPollEvents();
}
}  // namespace caldera
