#include "renderer.h"
#define NOMINMAX
#include <Windows.h>
#include <cstring>
#include <algorithm>

#include "state.h"

namespace caldera {
Renderer::Renderer() {
   glEnable(GL_DEPTH_TEST);
   glEnable(GL_PROGRAM_POINT_SIZE);

   // get shader executable path since this won't work with relative paths
   char exe_path[MAX_PATH];
   GetModuleFileNameA(nullptr, exe_path, MAX_PATH);
   std::string dir(exe_path, strrchr(exe_path, '\\') + 1 - exe_path);

   shader = std::make_unique<Shader>(dir + "assets/vertex.glsl", dir + "assets/fragment.glsl");
   bbox_gizmo = std::make_unique<BoundingBoxGizmo>();
}
Renderer::~Renderer() {
   glDeleteVertexArrays(1, &vao);
   glDeleteBuffers(1, &vbo);
}

void Renderer::render(GLFWwindow *window, Scene &scene, Camera &camera) {
   int width, height;
   glfwGetFramebufferSize(window, &width, &height);

   shader->bind();
   shader->setMat4("view", camera.get_view_matrix());
   shader->setMat4("proj", camera.get_proj_matrix(width, height));
   shader->setUInt("pointSize", state.point_size);


   if (scene.pcd) {
      bool moved = first_frame || camera.position != last_cam_pos;

      unsigned int floor_count = std::min<unsigned int>(state.init_points, scene.pcd->vertex_count);

      if (moved)
         visible_count = floor_count;
      else if (visible_count < scene.pcd->vertex_count)
         visible_count = std::min(scene.pcd->vertex_count, static_cast<unsigned int>(visible_count * state.growth_rate) + 1);

      last_cam_pos = camera.position;
      first_frame = false;

      glBindVertexArray(scene.pcd->vao);
      glDrawArrays(GL_POINTS, 0, visible_count);

      bbox_gizmo->set_bounds(scene.pcd->bbox_min, scene.pcd->bbox_max);
      bbox_gizmo->draw(camera.get_view_matrix(), camera.get_proj_matrix(width, height));
   }
}
}  // namespace caldera
