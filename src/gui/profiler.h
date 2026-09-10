#pragma once

#include <imgui.h>

namespace caldera {
class Profiler {
 public:
   void draw(double ms) {
      // constants, todo: need to make these class-level
      static const int FRAME_HIST_COUNT = 2000;
      static float frame_time_history[FRAME_HIST_COUNT] = {0};
      static int frame_time_offset = 0;
      static int frame_time_count = 0;

      frame_time_history[frame_time_offset] = (float)ms;
      frame_time_offset = (frame_time_offset + 1) % FRAME_HIST_COUNT;
      if (frame_time_count < FRAME_HIST_COUNT) frame_time_count++;

      float sum = 0.0f, max = 0.0f;
      for (float v : frame_time_history) {
         sum += v;
         if (v > max) max = v;
      }
      float avg = sum / frame_time_count;

      ImGui::SetNextWindowSize(ImVec2(0, 200), ImGuiCond_FirstUseEver);
      ImGui::Begin("profiler");

      ImGui::Text("Current: %.2f ms", ms);
      ImGui::Text("Average: %.2f ms (%.1f fps)", avg, (avg > 0.0f) ? 1000.0f / avg : 0.0f);

      ImGui::PlotLines("", frame_time_history, FRAME_HIST_COUNT, frame_time_offset, nullptr, 0.0f, max + 2.0f, ImVec2(ImGui::GetContentRegionAvail().x, 80));
      // ImGui::PlotLines("", frame_time_history, FRAME_HIST_COUNT, frame_time_offset, nullptr, 0.0f, max);

      ImGui::TextColored(ImVec4(0.5f, 0.5f, 0.5f, 1.0f), "Max: %.1f ms", max);
      ImGui::End();
   }

 private:
};
}  // namespace caldera
