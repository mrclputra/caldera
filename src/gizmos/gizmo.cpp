#include "gizmo.h"

#include <glad/glad.h>

#define NOMINMAX
#include <Windows.h>
#include <cstring>

namespace caldera {
Shader &Gizmo::shader() {
   static std::unique_ptr<Shader> instance = [] {
      char exe_path[MAX_PATH];
      GetModuleFileNameA(nullptr, exe_path, MAX_PATH);
      std::string dir(exe_path, strrchr(exe_path, '\\') + 1 - exe_path);
      return std::make_unique<Shader>(dir + "assets/gizmo_vertex.glsl", dir + "assets/gizmo_fragment.glsl");
   }();
   return *instance;
}

Gizmo::~Gizmo() {
   if (vao) glDeleteVertexArrays(1, &vao);
   if (vbo) glDeleteBuffers(1, &vbo);
   if (ebo) glDeleteBuffers(1, &ebo);
}


void Gizmo::set_lines(const std::vector<glm::vec3> &verts, const std::vector<unsigned int> &indices) {
     if (!vao) {
        glGenVertexArrays(1, &vao);
        glGenBuffers(1, &vbo);
        glGenBuffers(1, &ebo);
     }

     glBindVertexArray(vao);
     glBindBuffer(GL_ARRAY_BUFFER, vbo);
     glBufferData(GL_ARRAY_BUFFER, verts.size() * sizeof(glm::vec3), verts.data(), GL_DYNAMIC_DRAW);
     glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
     glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_DYNAMIC_DRAW);

     glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void *)0);
     glEnableVertexAttribArray(0);
     glBindVertexArray(0);

     index_count = static_cast<unsigned int>(indices.size());
  }

  void Gizmo::draw_lines(const glm::mat4 &model, const glm::mat4 &view, const glm::mat4 &proj) {
     Shader &s = shader();
     s.bind();
     s.setMat4("model", model);
     s.setMat4("view", view);
     s.setMat4("proj", proj);
     s.setVec3("color", color);

     glBindVertexArray(vao);
     glDrawElements(GL_LINES, index_count, GL_UNSIGNED_INT, 0);
     glBindVertexArray(0);
  }
}  // namespace caldera
