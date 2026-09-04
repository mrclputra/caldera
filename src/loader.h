#pragma once

#include <tinyply.h>
// using namespace tinyply;

#include <spdlog/spdlog.h>
#include <glad/glad.h>
#include <glm/glm.hpp>

#include <vector>
#include <fstream>     // for reading files
#include <spanstream>  // ++23
#include <chrono>

#include "scene.h"
#include "pointcloud.h"

namespace caldera {

struct Vertex {
   glm::vec3 position{0.0f};
   glm::vec3 normal{0.0f};
   glm::vec3 color{1.0f, 0.0f, 1.0f};
};

class Loader {
 public:
   // call this function to load a ply file
   void read_ply_file(const std::string &path, const bool preload) {
      SPDLOG_INFO("reading .ply file: {}", path);
      auto start = std::chrono::steady_clock::now();

      std::unique_ptr<std::istream> file_stream;
      std::vector<uint8_t> byte_buffer;

      try {
         if (preload) {
            byte_buffer = read_file_binary(path);
            file_stream = std::make_unique<std::ispanstream>(std::span<char>(reinterpret_cast<char *>(byte_buffer.data()), byte_buffer.size()));
         } else {
            file_stream = std::make_unique<std::ifstream>(path, std::ios::binary);
         }
         if (!file_stream || file_stream->fail())
            throw std::runtime_error("file_stream failed to open " + path);

         file_stream->seekg(0, std::ios::end);
         float size_mb = file_stream->tellg() * float(1e-6);  // bytes to mb
         SPDLOG_INFO("filesize: {}mb", size_mb);
         file_stream->seekg(0, std::ios::beg);

         tinyply::PlyFile file;
         file.parse_header(*file_stream);

         // print file information
         SPDLOG_INFO("[ply_header] type: {}", (file.is_binary_file() ? "binary" : "ascii"));
         for (const auto &c : file.get_comments())
            SPDLOG_INFO("[ply_header] comment: {}", c);
         for (const auto &c : file.get_info())
            SPDLOG_INFO("[ply_header] info: {}", c);
         for (const auto &e : file.get_elements()) {
            SPDLOG_INFO("[ply_header] element: {} ({})", e.name, e.size);
            for (const auto &p : e.properties) {
               SPDLOG_INFO("[ply_header]\tproperty : {} (type={})", p.name, tinyply::PropertyTable[p.propertyType].str);
               if (p.isList)
                  SPDLOG_INFO("[ply_header]\t\t(list_type={})", tinyply::PropertyTable[p.listType].str);
            }
         }

         // TODO: ideally based on the header information, we want to save parse the data accordingly
         //    - check what kind of data is available
         //    - check that datatypes match
         //       - see if we can use type erasure?
         //  note from tinyply documentation:
         //  the header information can and should be used to programmatically extract properties on elements known to exist
         //  in the header prior to reading the actual data; we should test and verify this with different datasets
         //
         //  or maybe it would be better to enforce a specific convention? i.e. Blender?

         // to be filled
         // std::shared_ptr<tinyply::PlyData> vertices, normals, colors, texcoords, faces, tristrip;  // todo: i believe this should be module-level

         std::shared_ptr<tinyply::PlyData> tinyply_vertices;
         std::shared_ptr<tinyply::PlyData> tinyply_normals;
         std::shared_ptr<tinyply::PlyData> tinyply_colors;

         try {
            tinyply_vertices = file.request_properties_from_element("vertex", {"x", "y", "z"});
            SPDLOG_INFO("loaded vertices in x y z");
         } catch (const std::exception &e) {
            SPDLOG_ERROR("no vertices: {}", e.what());
         }
         try {
            tinyply_normals = file.request_properties_from_element("vertex", {"nx", "ny", "nz"});
            SPDLOG_INFO("loaded normals in nx ny nz");
         } catch (const std::exception &e) {
            SPDLOG_WARN("no normals: {}", e.what());
         }
         try {
            tinyply_colors = file.request_properties_from_element("vertex", {"red", "green", "blue"});
            SPDLOG_INFO("loaded colors in red green blue");
         } catch (const std::exception &e) {
            SPDLOG_WARN("no colors in red green blue: {}", e.what());
            try {
               tinyply_colors = file.request_properties_from_element("vertex", {"r", "g", "b"});
               SPDLOG_INFO("loaded colors in r g b");
            } catch (const std::exception &e) {
               SPDLOG_WARN("no colors in rgb either: {}", e.what());
            }
         }

         file.read(*file_stream);

         // log confirmation
         if (tinyply_vertices)
            SPDLOG_INFO("read {} vertices", tinyply_vertices->count);
         if (tinyply_normals)
            SPDLOG_INFO("read {} normals", tinyply_normals->count);
         if (tinyply_colors)
            SPDLOG_INFO("read {} colors", tinyply_colors->count);

         // convert to own datatype
         vertices.resize(tinyply_vertices->count);  // set size
         for (size_t i = 0; i < vertices.size(); i++) {
            vertices[i].position = to_vec3(tinyply_vertices, i);
            if (tinyply_normals)
               vertices[i].normal = to_vec3(tinyply_normals, i);
            if (tinyply_colors)
               vertices[i].color = to_vec3(tinyply_colors, i);
         }

         auto end = std::chrono::steady_clock::now();
         double ms = std::chrono::duration<double, std::milli>(end - start).count();
         SPDLOG_INFO("loaded {:.2f}mb in {:.2f}ms", size_mb, ms);

      } catch (const std::exception &e) {
         SPDLOG_ERROR("caught loader exception: {}", e.what());
      }
   }

   void upload(Scene &scene) {
      PointCloud pcd;
      // upload directly to the gpu buffer
      // note that this might be better to be moved to renderer class
      glGenVertexArrays(1, &pcd.vao);
      glGenBuffers(1, &pcd.vbo);
      glBindVertexArray(pcd.vao);
      glBindBuffer(GL_ARRAY_BUFFER, pcd.vbo);
      glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(), GL_STATIC_DRAW);

      // TODO BUG: if normal is not present, vertex breaks for whatever reason? offsets?
      // attributes--
      glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)offsetof(Vertex, position));
      glEnableVertexAttribArray(0);
      glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)offsetof(Vertex, normal));
      glEnableVertexAttribArray(1);
      glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)offsetof(Vertex, color));
      glEnableVertexAttribArray(2);

      pcd.vertex_count = vertices.size();
      glBindVertexArray(0);

      scene.pcd = std::make_unique<PointCloud>(pcd);
   }

 private:
   std::vector<Vertex> vertices;

   glm::vec3 to_vec3(const std::shared_ptr<tinyply::PlyData> &data, size_t i) {
      if (data->t == tinyply::Type::FLOAT32) {
         const float *src = reinterpret_cast<const float *>(data->buffer.get());
         return glm::vec3(src[i * 3 + 0], src[i * 3 + 1], src[i * 3 + 2]);
      }
      if (data->t == tinyply::Type::FLOAT64) {
         const double *src = reinterpret_cast<const double *>(data->buffer.get());
         return glm::vec3(
             static_cast<float>(src[i * 3 + 0]),
             static_cast<float>(src[i * 3 + 1]),
             static_cast<float>(src[i * 3 + 2]));
      }
      if (data->t == tinyply::Type::UINT8) {
         const uint8_t *src = data->buffer.get();
         return glm::vec3(src[i * 3 + 0], src[i * 3 + 1], src[i * 3 + 2]) / 255.0f;
      }
      SPDLOG_ERROR("unsupported vertex type: {}", tinyply::PropertyTable[data->t].str);
      return glm::vec3(0.0f);
   }

   // std::vector<glm::vec3> to_vec3(std::shared_ptr<tinyply::PlyData> &data) {
   //    if (!data || data->count == 0)
   //       return {};
   //
   //    std::vector<glm::vec3> out(data->count);
   //
   //    if (data->t == tinyply::Type::FLOAT32) {
   //       std::memcpy(out.data(), data->buffer.get(), data->buffer.size_bytes());
   //    } else if (data->t == tinyply::Type::FLOAT64) {
   //       const double *src = reinterpret_cast<const double *>(data->buffer.get());
   //       for (size_t i = 0; i < data->count; i++) {
   //          out[i] = glm::vec3(
   //              static_cast<float>(src[i * 3 + 0]),
   //              static_cast<float>(src[i * 3 + 1]),
   //              static_cast<float>(src[i * 3 + 2]));
   //       }
   //    } else if (data->t == tinyply::Type::UINT8) {
   //       const uint8_t *src = data->buffer.get();
   //       for (size_t i = 0; i < data->count; i++) {
   //          out[i] = glm::vec3(src[i * 3 + 0], src[i * 3 + 1], src[i * 3 + 2]) / 255.0f;
   //       }
   //    } else {
   //       SPDLOG_ERROR("unsupported vertex type: {}", tinyply::PropertyTable[data->t].str);
   //       return {};
   //    }
   //
   //    SPDLOG_INFO("converted to vec3");
   //    return out;
   // }

   std::vector<uint8_t> read_file_binary(const std::string &path) {
      std::ifstream file(path, std::ios::binary);
      std::vector<uint8_t> buffer_bytes;

      if (file.is_open()) {
         file.seekg(0, std::ios::end);
         long long size_bytes = file.tellg();
         file.seekg(0, std::ios::beg);
         buffer_bytes.resize(size_bytes);
         if (file.read((char *)buffer_bytes.data(), size_bytes))
            return buffer_bytes;
      } else {
         throw std::runtime_error("could not open binary ifstream to path: " + path);
      }
      return buffer_bytes;
   }
};
}  // namespace caldera
