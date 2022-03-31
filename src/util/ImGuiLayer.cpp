#include <util/ImGuiLayer.h>
#include <game.h>

void ImGuiLayer::Step(Game * game){
  bool open = false, save = false;

}

void ImGuiLayer::BuildDock(){
  int mConsoleDockSpaceId;

  bool dockSpaceCreated = ImGui::DockBuilderGetNode(mConsoleDockSpaceId) != nullptr;
  if (!dockSpaceCreated) {
      mConsoleDockSpaceId = ImGui::GetID("ConsoleDockSpace");

      ImGui::DockBuilderRemoveNode(mConsoleDockSpaceId);
      ImGuiDockNodeFlags dockSpaceFlags = 0;
      dockSpaceFlags |= ImGuiDockNodeFlags_PassthruCentralNode;
      ImGui::DockBuilderAddNode(mConsoleDockSpaceId, dockSpaceFlags);

      ImGuiID dockMain = mConsoleDockSpaceId;
      ImGuiID dockLeft = ImGui::DockBuilderSplitNode(dockMain, ImGuiDir_Left, 0.40f, NULL, &dockMain);

  }
}

void ImGuiLayer::Init(GLFWwindow * window){
  IMGUI_CHECKVERSION();

  ImGui::CreateContext();
  ImGui_ImplGlfw_InitForOpenGL(window, true);

  ImGuiIO& io = ImGui::GetIO();
  io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
//    io.BackendFlags |= ImGuiBackendFlags_HasSetMousePos;

  std::string glsl_version = "#version 330";
  //ImGui::StyleColorsDark();
  // set up keycodes here
  ImGui_ImplOpenGL3_Init(glsl_version.c_str());

  //ImGui::StyleColorsDark();
}


void ImGuiLayer::NewFrame(){

  //get deltatime
  ImGui_ImplOpenGL3_NewFrame();
  ImGui_ImplGlfw_NewFrame();
  ImGui::NewFrame();

  //ImGuiWindowFlags flags = ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse;

}

void ImGuiLayer::EndFrame(){
  //ImGui::Render();
  //ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

ImVec2 ImGuiLayer::RenderImGuiViewport(uint texture, std::string name){
  ImVec2 wsize;
  ImGui::Begin(name.c_str());
  {
    Settings::SetCamera(0);
    // Using a Child allow to fill all the space of the window.
    // It also alows customization
    ImGui::BeginChild("GameRender");
    // Get the size of the child (i.e. the whole draw size of the windows).
    wsize = ImGui::GetWindowSize();
    // Because I use the texture from OpenGL, I need to invert the V from the UV.
    ImGui::Image((ImTextureID)texture, wsize, ImVec2(0, 1), ImVec2(1, 0));
    ImGui::EndChild();
  }
  ImGui::End();
  return wsize;
}
