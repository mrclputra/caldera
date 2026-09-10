#pragma once
#include <imgui.h>
#include <spdlog/spdlog.h>
#include <spdlog/sinks/ringbuffer_sink.h>

namespace caldera {
class Log {
 public:
   Log(std::shared_ptr<spdlog::sinks::ringbuffer_sink_mt> g_ring_sink) {
      this->g_ring_sink = std::move(g_ring_sink);
   }

   void draw() const {
      const ImGuiViewport *vp = ImGui::GetMainViewport();
      ImGui::SetNextWindowPos({vp->WorkPos.x + 10.0f, vp->WorkSize.y - 10.0f}, ImGuiCond_Always, {0.0f, 1.0f});

      ImGuiWindowFlags flags = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoSavedSettings |
                               ImGuiWindowFlags_NoNav | ImGuiWindowFlags_NoInputs;
      ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
      ImGui::Begin("log", nullptr, flags);
      // ImGui::SetWindowFontScale(0.7f);

      for (const auto &msg : g_ring_sink->last_raw()) {
         ImVec4 color = msg.level == spdlog::level::err    ? ImVec4(1.0f, 0.3f, 0.3f, 1.0f)
                        : msg.level == spdlog::level::warn ? ImVec4(1.0f, 0.8f, 0.2f, 1.0f)
                                                           : ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
         ImGui::TextColored(color, "%s", std::string(msg.payload.data(), msg.payload.size()).c_str());
      }
      ImGui::End();
      ImGui::PopStyleVar();
   }

 private:
   std::shared_ptr<spdlog::sinks::ringbuffer_sink_mt> g_ring_sink;
};
}  // namespace caldera
