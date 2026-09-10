#pragma once
#include <array>
#include <algorithm>

#include <imgui.h>

namespace caldera {
class Histogram {
 public:
   void compute(const unsigned char *pixels, const float *depth, int width, int height) {
      bins_r.fill(0);
      bins_g.fill(0);
      bins_b.fill(0);

      for (size_t i = 0; i < static_cast<size_t>(width) * height; i++) {
         if (depth[i] >= 1.0f) continue;  // cleared

         bins_r[pixels[i * 3 + 0]]++;
         bins_g[pixels[i * 3 + 1]]++;
         bins_b[pixels[i * 3 + 2]]++;
      }
   }

   void draw() const {
      auto plot = [](const char *id, const std::array<int, 256> &bins, ImVec4 color) {
         std::array<float, 256> f;
         for (int i = 0; i < 256; ++i) f[i] = static_cast<float>(bins[i]);
         int max_bin = *std::max_element(bins.begin(), bins.end());
         ImGui::PushStyleColor(ImGuiCol_PlotHistogram, color);
         ImGui::PlotHistogram(id, f.data(), 256, 0, nullptr, 0.0f,
                              max_bin > 0 ? static_cast<float>(max_bin) : 1.0f,
                              ImVec2(ImGui::GetContentRegionAvail().x, 80));
         ImGui::PopStyleColor();
      };

      plot("##hist_r", bins_r, ImVec4(1.0f, 0.3f, 0.3f, 1.0f));
      plot("##hist_g", bins_g, ImVec4(0.3f, 1.0f, 0.3f, 1.0f));
      plot("##hist_b", bins_b, ImVec4(0.3f, 0.3f, 1.0f, 1.0f));
   }

 private:
   std::array<int, 256> bins_r{};
   std::array<int, 256> bins_g{};
   std::array<int, 256> bins_b{};
};
}  // namespace caldera
