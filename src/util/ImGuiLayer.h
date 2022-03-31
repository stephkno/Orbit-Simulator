#pragma once
#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_opengl3.h>
#include <ImGuiFileBrowser.h>
#include <iostream>
#include <fstream>
#include <settings.h>
#include <ImGuizmo/ImGuizmo.h>
#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/xml_oarchive.hpp>
#include <boost/archive/xml_iarchive.hpp>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>
#include <boost/serialization/map.hpp>
#include <boost/serialization/vector.hpp>

class Game;

static ImGuizmo::MODE mCurrentGizmoMode(ImGuizmo::LOCAL);
static imgui_addons::ImGuiFileBrowser file_dialog; // As a class member or globally
//static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_PassthruCentralNode;

static int active_window;

class ImGuiLayer{
public:

  static void Step(Game * game);
  ImGuiLayer(){
  }

  ~ImGuiLayer(){
  }

  static void Init(GLFWwindow * window);
  static void NewFrame();
  static void EndFrame();
  static void BuildDock();
  static ImVec2 RenderImGuiViewport(uint texture, std::string name);

  static bool FocusFollowsMouse(){
    if(ImGui::IsWindowHovered()){
      ImGui::SetWindowFocus();
      return true;
    }
    return false;
  }

  static int GetActiveWindow(){
    return active_window;
  }

  static void SetStyle();

};
