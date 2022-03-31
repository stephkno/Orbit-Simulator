#pragma once
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <settings.h>
#include <render/camera.h>
#include <render/free_cam.h>
#include <util/define.h>
#include <util/assets.h>
#include <filesystem>
#include <imgui/imgui.h>

class IO{
public:

  float lastX, lastY;
  float xpos, ypos;
  float deltaTime;
  float scrollX = 1.0f;
  float scrollY = 1.0f;
  float scrollXoffset, scrollYoffset;
  bool firstMouse;
  bool mouseHidden=true;
  static IO * io;
  float xoffset, yoffset;
  GLFWwindow * window;
  int mouseButton[3];
  int keyButtons[350];
  std::vector<int> keysPressed;

  IO(GLFWwindow * window);
  static IO * Get(GLFWwindow * window);
  static IO * Get();

  static bool MouseHidden(){
    return io->mouseHidden;
  }

  // glfw: whenever the window size changed (by OS or user resize) this callback function executes
  // ---------------------------------------------------------------------------------------------
  static void framebuffer_size_callback(GLFWwindow* window, int width, int height);

  // glfw: whenever the mouse moves, this callback is called
  // -------------------------------------------------------
  //static void mouse_callback(GLFWwindow* window, double xpos, double ypos);
  // glfw: whenever the mouse scroll wheel scrolls, this callback is called
  // ----------------------------------------------------------------------
  static void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
  // process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
  // ---------------------------------------------------------------------------------------------------------
  static void processInput(GLFWwindow *window, float deltaTime);

  static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);

  static void drop_callback(GLFWwindow* window, int count, const char** paths)
  {
    int i;
    List<ImportFile> filenames;

    Print << std::filesystem::current_path() << Endl;

    for (i = 0;  i < count;  i++){
      std::string s = paths[i];
      for(auto& p: std::filesystem::recursive_directory_iterator(s+"/.")){

        Print << p.path().relative_path() << Endl;
        filenames.push_back(ImportFile(p.path().relative_path()));

      }
    }
    AssetManager::ImportFiles(filenames);
  }

  static void Step(GLFWwindow * window){

    for(int button = 0; button < 3; button++){
      if(glfwGetMouseButton(io->window, button)==GLFW_PRESS){
        // mouse down
        if(io->mouseButton[button] == 0){
          io->mouseButton[button] = 1;
        }
        // mouse held
        else if(io->mouseButton[button]==1){
          io->mouseButton[button] = 2;
        }
      }else{
        if(io->mouseButton[button]==2){
          io->mouseButton[button] = 3;
        }
        else if(io->mouseButton[button]!=0){
          io->mouseButton[button] = 0;
        }
      }
    }

    double xpos;
    double ypos;
    glfwGetCursorPos(window, &xpos, &ypos);

    io->xoffset = xpos - io->lastX;
    io->yoffset = io->lastY - ypos; // reversed since y-coordinates go from bottom to top

    io->lastX = xpos;
    io->lastY = ypos;

    io->xpos = xpos;
    io->ypos = ypos;
  }
  static bool MouseDown(int button){
    Get();
    return io->mouseButton[button] == 1;
  }
  static bool MouseHeld(int button){
    Get();
    return io->mouseButton[button] == 2;
  }
  static bool MouseUp(int button){
    Get();
    return io->mouseButton[button] == 3;
  }
  static bool KeyDown(int button){
    Get();
    if(io->keyButtons[button] != 1){
      int buttonState = glfwGetKey(io->window, button);
      if(buttonState == 1){
        io->keyButtons[button] = 1;
        io->keysPressed.push_back(button);
        return true;
      }
    }
    return false;
  }
  static bool KeyHeld(int button){
    Get();
    if(io->keyButtons[button] == 1){
      int buttonState = glfwGetKey(io->window, button);
      if(buttonState == 1){
        io->keyButtons[button] = 1;
        return true;
      }else{
        io->keyButtons[button] = 0;
        return false;
      }
    }
    return false;
  }

  static float GetMouseX(){
    if(!io){
      Get();
    }
    return io->xoffset;
  }
  static float GetMouseY(){
    if(!io){
      Get();
    }
    return io->yoffset;
  }
  static glm::vec2 GetMousePos(){
    if(!io){
      Get();
    }
    return glm::vec2(io->xpos, io->ypos);
  }
  static float GetScrollY(){
    if(!io){
      Get();
    }
    return io->scrollY;
  }
  static float GetScrollDY(){
    if(!io){
      Get();
    }
    return io->scrollYoffset;
  }
  static float GetAxis(uint a){
    if(!io){
      Get();
    }
    if(!glfwJoystickPresent(GLFW_JOYSTICK_1)){
      return 0.0f;
    }
    int count = 2;
    const float * axis = glfwGetJoystickAxes(GLFW_JOYSTICK_1, &count);
    if(axis){
      if(abs(axis[a])<0.05f){
        return 0.0f;
      }
      return axis[a];
    }
    else{
      return 0.0f;
    }
  }

};
