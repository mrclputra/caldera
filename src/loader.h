#pragma once

#include <tinyply.h>
using namespace tinyply;
#include <spdlog/spdlog.h>

#include <vector>
#include <fstream>
#include <cstring>

#include <spanstream>  // ++23

namespace caldera {

class Loader {
 public:
   void read_ply_file(const std::string &path, const bool preload) {
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
         const float size_mb = file_stream->tellg() * float(1e-6);  // bytes to mb
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
            colors = file.request_properties_from_element("vertex", {"red", "green", "blue", "alpha"});
         } catch (const std::exception &e) {
            SPDLOG_WARN("loader: {}", e.what());
         }
         try {
            colors = file.request_properties_from_element("vertex", {"r", "g", "b", "a"});
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

         // confirm with logs
         if (vertices) SPDLOG_INFO("\tread {} vertices", vertices->count);
         if (normals) SPDLOG_INFO("\tread {} vertex normals", normals->count);
         if (colors) SPDLOG_INFO("\tread {} vertex colors", colors->count);
         if (texcoords) SPDLOG_INFO("\tread {} vertex texcoords", texcoords->count);
         if (faces) SPDLOG_INFO("\tread {} faces (triangles)", faces->count);
         if (tristrip) SPDLOG_INFO("\tread {} indices (tristrip)", tinyply::PropertyTable[tristrip->t].stride);
         if (faces && faces->list_sizes.size()) SPDLOG_INFO("\tread {} variable-length indices", faces->list_sizes.size());

         // TODO: convert to my internal project datatypes

         SPDLOG_INFO("done loading: {}", path);

      } catch (const std::exception &e) {
         SPDLOG_ERROR("caught loader exception: {}", e.what());
      }
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
