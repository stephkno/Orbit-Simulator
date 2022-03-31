#pragma once;
#include <iostream>
#include <sstream>
#include <imgui/imgui.h>
#include <vector>
#include <string>
#include <glm/glm.hpp>

class GLFWwindow;

struct ConsoleMessage{
  std::string content;
  bool error = false;
  ConsoleMessage(std::string  c, bool e){
    content = c;
    error = e;
  }
};

class Debug{
  static Debug * editor;
public:
  std::vector<ConsoleMessage> consoleLines;

  Debug();
  ~Debug(){

  }
  static Debug * Get();
  static void Step();

  template <typename... Ts>
  static void msg(const Ts&... args){
    Get();
    std::ostringstream oss;
    (oss << ... << args);
    //std::string s = std::string(location.file_name()) + "::" + std::to_string(location.line()) + "::[" + oss.str() + "]";
    ConsoleMessage m = ConsoleMessage(oss.str(), false);
    editor->consoleLines.push_back(m);

    //std::cout << "[Debug " << editor->consoleLines.size() << ": ";
    //std::cout << oss.str() << "]" << std::endl;

  }
  template <typename... Ts>
  static void err(const Ts&... args){
    Get();
    std::ostringstream oss;
    (oss << ... << args);
    //std::string s = std::string(location.file_name()) + "::" + std::to_string(location.line()) + "::[" + oss.str() + "]";
    ConsoleMessage m = ConsoleMessage(oss.str(), true);
    editor->consoleLines.push_back(m);
  }

  static void UpdateMouse(float x, float y);

};

//log(1, 'a', "hello world!");   // CTAD
