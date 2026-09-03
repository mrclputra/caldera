#pragma once

#include <tinyply.h>
// using namespace tinyply;

#include <spdlog/spdlog.h>
#include <glad/glad.h>
#include <glm/glm.hpp>

#include <vector>
#include <fstream>     // for reading files
#include <spanstream>  // ++23

// #include "scene.h"  // todo: do we really need this?
// #include "pointcloud.h"

namespace caldera {

// this is a temporary datastructure to store the loaded data,
// the idea is that a pcd would have a vector of these
// struct vertex {
//    glm::vec3 position{0.0f};
//    glm::vec3 normal{0.0f};
//    glm::vec3 tangent{0.0f};
//    glm::vec3 bitangent{0.0f};
//    glm::vec3 color{0.0f};
// };

class Loader {
 public:
   // call this function to load a ply file
   void read_ply_file(const std::string &path, const bool preload) {
      SPDLOG_INFO("reading .ply file: {}", path);

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
         SPDLOG_INFO("filesize: {}mb", file_stream->tellg() * float(1e-6));  // bytes to mb
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

         try {
            vertices = file.request_properties_from_element("vertex", {"x", "y", "z"});
            SPDLOG_INFO("loaded vertices in x y z");
         } catch (const std::exception &e) {
            SPDLOG_ERROR("no vertices: {}", e.what());
         }
         try {
            normals = file.request_properties_from_element("vertex", {"nx", "ny", "nz"});
            SPDLOG_INFO("loaded normals in nx ny nz");
         } catch (const std::exception &e) {
            SPDLOG_WARN("no normals: {}", e.what());
         }
         try {
            colors = file.request_properties_from_element("vertex", {"red", "green", "blue"});
            SPDLOG_INFO("loaded colors in red green blue");
         } catch (const std::exception &e) {
            SPDLOG_WARN("no colors in red green blue: {}", e.what());
            try {
               colors = file.request_properties_from_element("vertex", {"r", "g", "b"});
               SPDLOG_INFO("loaded colors in r g b");
            } catch (const std::exception &e) {
               SPDLOG_WARN("no colors in rgb either: {}", e.what());
            }
         }

         file.read(*file_stream);

         // log confirmation
         // if (vertices || vertices->count == 0)
         if (vertices || vertices->count == 0)
            SPDLOG_INFO("read {} vertices", vertices->count);
         if (normals)
            SPDLOG_INFO("read {} normals", normals->count);
         if (colors)
            SPDLOG_INFO("read {} colors", colors->count);

         // example converting to my own datatype
         SPDLOG_INFO("filedata:");
         std::vector<glm::vec3> verts = to_vec3(vertices);
         // for (const auto &v : verts) {
         //    SPDLOG_INFO("{},{},{}", v.x, v.y, v.z);
         // }

      } catch (const std::exception &e) {
         SPDLOG_ERROR("caught loader exception: {}", e.what());
      }
   }

   void upload();

 private:
   std::shared_ptr<tinyply::PlyData> vertices;
   std::shared_ptr<tinyply::PlyData> normals;
   std::shared_ptr<tinyply::PlyData> colors;

   std::vector<glm::vec3> to_vec3(std::shared_ptr<tinyply::PlyData> &data) {
      if (!data || data->count == 0)
         return {};

      std::vector<glm::vec3> out(data->count);

      if (data->t == tinyply::Type::FLOAT32) {
         std::memcpy(out.data(), data->buffer.get(), data->buffer.size_bytes());
      } else if (data->t == tinyply::Type::FLOAT64) {
         const double *src = reinterpret_cast<const double *>(data->buffer.get());
         for (size_t i = 0; i < data->count; i++) {
            out[i] = glm::vec3(
                  static_cast<float>(src[i*3+0]),
                  static_cast<float>(src[i*3+1]),
                  static_cast<float>(src[i*3+2]));
         }
      } else {
         SPDLOG_ERROR("unsupported vertex type: {}", tinyply::PropertyTable[data->t].str);
         return {};
      }

      return out;
   }

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
