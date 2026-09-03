#pragma once

#include <tinyply.h>
using namespace tinyply;
#include <spdlog/spdlog.h>
#include <glad/glad.h>
#include <glm/glm.hpp>

#include <vector>
#include <fstream>
#include <cstring>

#include <spanstream>  // ++23

#include "scene.h"
#include "pointcloud.h"

namespace caldera {

struct Vertex {
   glm::vec3 position{0.0f};
   glm::vec3 normal{0.0f};
   glm::vec3 tangent{0.0f};
   glm::vec3 bitangent{0.0f};
   glm::vec3 color{0.0f};
};


// todo: need to rewrite this class to make it as adaptable as possible
class Loader {
 public:
   void read_ply_file(Scene &scene, const std::string &path, const bool preload) {
      SPDLOG_INFO("reading ply file: {}", path);

      std::unique_ptr<std::istream> file_stream;
      std::vector<uint8_t> byte_buffer;

      try {
         if (preload) {
            byte_buffer = read_file_binary(path);
            file_stream = std::make_unique<std::ispanstream>(
                std::span<char>(reinterpret_cast<char *>(byte_buffer.data()), byte_buffer.size()));
         } else {
            file_stream = std::make_unique<std::ifstream>(path, std::ios::binary);
         }
         if (!file_stream || file_stream->fail())
            throw std::runtime_error("file_stream failed to open " + path);

         file_stream->seekg(0, std::ios::end);
         // const float size_mb = file_stream->tellg() * float(1e-6);  // bytes to mb
         file_stream->seekg(0, std::ios::beg);

         PlyFile file;
         file.parse_header(*file_stream);

         SPDLOG_INFO("\t[ply_header] type: {}", (file.is_binary_file() ? "binary" : "ascii"));
         for (const auto &c : file.get_comments())
            SPDLOG_INFO("\t[ply_header] comment: {}", c);
         for (const auto &c : file.get_info())
            SPDLOG_INFO("\t[ply_header] info: {}", c);

         for (const auto &e : file.get_elements()) {
            SPDLOG_INFO("\t[ply_header] element: {} ({})", e.name, e.size);
            for (const auto &p : e.properties) {
               SPDLOG_INFO("\t[ply_header] \tproperty: {} (type={})", p.name, tinyply::PropertyTable[p.propertyType].str);
               if (p.isList)
                  SPDLOG_INFO("\t\t\t(list_type={})", tinyply::PropertyTable[p.listType].str);
            }
         }

         // to be filled
         std::shared_ptr<PlyData> vertices, normals, colors, texcoords, faces, tristrip;

         // note from tinyply docs:
         // the header information can be used to programmatically extract properties on elements known to exist
         // in the header prior to reading the data
         try {
            vertices = file.request_properties_from_element("vertex", {"x", "y", "z"});
         } catch (const std::exception &e) {
            SPDLOG_WARN("loader: {}", e.what());
         }
         try {
            normals = file.request_properties_from_element("vertex", {"nx", "ny", "nz"});
         } catch (const std::exception &e) {
            SPDLOG_WARN("loader: {}", e.what());
         }
         try {
            colors = file.request_properties_from_element("vertex", {"red", "green", "blue"});
         } catch (const std::exception &e) {
            SPDLOG_WARN("loader: {}", e.what());
         }
         try {
            texcoords = file.request_properties_from_element("vertex", {"u", "v"});
         } catch (const std::exception &e) {
            SPDLOG_WARN("loader: {}", e.what());
         }
         // important performance optimization:
         // providing a list size hint is a 2x performance optimization, if arbitrary user-imported, set to 0
         try {
            faces = file.request_properties_from_element("face", {"vertex_indices"}, 3);
         } catch (const std::exception &e) {
            SPDLOG_WARN("loader: {}", e.what());
         }
         // tristrips must use 0
         try {
            tristrip = file.request_properties_from_element("tristrips", {"vertex_indices"}, 0);
         } catch (const std::exception &e) {
            SPDLOG_WARN("loader: {}", e.what());
         }

         file.read(*file_stream);

         // confirm with logs
         if (vertices)
            SPDLOG_INFO("\tread {} vertices", vertices->count);
         if (normals)
            SPDLOG_INFO("\tread {} vertex normals", normals->count);
         if (colors)
            SPDLOG_INFO("\tread {} vertex colors", colors->count);
         if (texcoords)
            SPDLOG_INFO("\tread {} vertex texcoords", texcoords->count);
         if (faces)
            SPDLOG_INFO("\tread {} faces (triangles)", faces->count);
         if (tristrip)
            SPDLOG_INFO("\tread {} indices (tristrip)", tinyply::PropertyTable[tristrip->t].stride);
         if (faces && faces->list_sizes.size())
            SPDLOG_INFO("\tread {} variable-length indices", faces->list_sizes.size());

         // TODO: convert to my internal project datatypes
         std::vector<Vertex> verts(vertices->count);
         double *pos = reinterpret_cast<double *>(vertices->buffer.get());
         for (size_t i = 0; i < vertices->count; i++)
            verts[i].position = {(float)pos[i * 3], (float)pos[i * 3 + 1], (float)pos[i * 3 + 2]};
         if (normals) {
            float *nor = reinterpret_cast<float *>(normals->buffer.get());
            for (size_t i = 0; i < vertices->count; i++)
               verts[i].normal = {nor[i * 3], nor[i * 3 + 1], nor[i * 3 + 2]};
         }
         if (colors)
            SPDLOG_INFO("\tcolor type: {}", tinyply::PropertyTable[colors->t].str);
         if (colors) {
            uint8_t *col = colors->buffer.get();
            for (size_t i = 0; i < vertices->count; i++)
               verts[i].color = {col[i * 3] / 255.f, col[i * 3 + 1] / 255.f, col[i * 3 + 2] / 255.f};
         }

         SPDLOG_INFO("done loading: {}", path);

         SPDLOG_INFO("uploading loaded model to gpu");

         // uploads to gpu
         PointCloud pcd;
         // note that this is temporary:
         //    upload directly to gpu buffer
         glGenVertexArrays(1, &pcd.vao);
         glGenBuffers(1, &pcd.vbo);
         glBindVertexArray(pcd.vao);
         glBindBuffer(GL_ARRAY_BUFFER, pcd.vbo);
         glBufferData(GL_ARRAY_BUFFER, verts.size() * sizeof(Vertex), verts.data(), GL_STATIC_DRAW);

         // attributes--
         glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)offsetof(Vertex, position));
         glEnableVertexAttribArray(0);
         glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)offsetof(Vertex, normal));
         glEnableVertexAttribArray(1);
         glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)offsetof(Vertex, color));
         glEnableVertexAttribArray(2);

         pcd.vertex_count = verts.size();
         glBindVertexArray(0);

         scene.pcd = std::make_unique<PointCloud>(pcd);

      } catch (const std::exception &e) {
         SPDLOG_ERROR("caught loader exception: {}", e.what());
      }
   }

   void upload() {
   }

 private:
   std::vector<uint8_t> read_file_binary(const std::string &path) {
      std::ifstream file(path, std::ios::binary);
      std::vector<uint8_t> buffer_bytes;

      if (file.is_open()) {
         file.seekg(0, std::ios::end);
         size_t size_bytes = file.tellg();
         file.seekg(0, std::ios::beg);
         buffer_bytes.resize(size_bytes);
         if (file.read((char *)buffer_bytes.data(), size_bytes))
            return buffer_bytes;
      } else
         throw std::runtime_error("could not open binary ifstream to path: " + path);
      return buffer_bytes;
   }
};
}  // namespace caldera
