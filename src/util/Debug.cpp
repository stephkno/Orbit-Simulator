#include <util/Debug.h>
#include <util/ImGuiLayer.h>
#include <GLFW/glfw3.h>

Debug * Debug::editor = nullptr;

Debug::Debug(){
}

Debug * Debug::Get(){
  if(!editor){
    editor = new Debug();
  }
  return editor;
}

void Debug::Step(){
  Get();
  ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
  window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
  window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
  // When using ImGuiDockNodeFlags_PassthruCentralNode, DockSpace() will render our background and handle the pass-thru hole, so we ask Begin() to not render a background.
  ImGui::Begin("Console", nullptr);
  for(ConsoleMessage msg : editor->consoleLines){
    // if error print red
    ImGui::Text(msg.content.c_str());
    ImGui::SetScrollHereY(1.0f);
  }
  ImGui::End();
}

void Debug::UpdateMouse(float x, float y){
  log(ImGuiLayer::GetActiveWindow());
}
