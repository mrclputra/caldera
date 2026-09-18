#pragma once

#include <glm/glm.hpp>
#include <vector>

#include "shader.h"

namespace caldera {
class Gizmo {
 public:
   virtual ~Gizmo();
   virtual void draw(const glm::mat4 &view, const glm::mat4 &proj) = 0;

   glm::vec3 color{1.0f};
   bool visible = true;

 protected:
   void set_lines(const std::vector<glm::vec3> &verts, const std::vector<unsigned int> &indices);
   void draw_lines(const glm::mat4 &model, const glm::mat4 &view, const glm::mat4 &proj);

 private:
   unsigned int vao = 0;
   unsigned int vbo = 0;
   unsigned int ebo = 0;
   unsigned int index_count = 0;

   static Shader &shader();
};
}  // namespace caldera
