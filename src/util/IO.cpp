#include <util/IO.h>

IO * IO::io = nullptr;

IO::IO(GLFWwindow * _window){
  window = _window;
  glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
  mouseHidden = true;
  scrollX=1.0f;
  scrollY=1.0f;
}

IO * IO::Get(GLFWwindow * window){
  if(!io){
    io = new IO(window);
  }
  return io;
}

IO * IO::Get(){
  return io;
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void IO::framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}

void IO::key_callback(GLFWwindow* window, int button, int scancode, int action, int mods)
{
  Get();
  if(action == 0){
    io->keyButtons[button] = 0;
  }
}

// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
/*
void IO::mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
  if(!io){
    Get();
  }
  if (io->firstMouse)
  {
      io->firstMouse = false;
  }

  io->xoffset = xpos - io->lastX;
  io->yoffset = io->lastY - ypos; // reversed since y-coordinates go from bottom to top

  io->lastX = xpos;
  io->lastY = ypos;

  io->xpos = xpos;
  io->ypos = ypos;

  if(io->mouseHidden){
  //  Settings::GetMainCamera()->ProcessMouseMovement(io->xoffset, io->yoffset);
  }
}
*/
// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void IO::scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
  if(!io){
    Get();
  }
  //Settings::GetMainCamera()->ProcessMouseScroll(yoffset);
  io->scrollX+=xoffset;
  io->scrollY+=yoffset;

  io->scrollXoffset = xoffset;
  io->scrollYoffset = yoffset;

}
// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void IO::processInput(GLFWwindow *window, float deltaTime)
{
  glfwSetInputMode(window, GLFW_RAW_MOUSE_MOTION, GLFW_TRUE);
  FreeCam * cam = (FreeCam*)Settings::GetMainCamera();

  if (KeyDown(GLFW_KEY_Z)){
    if(!io->mouseHidden){
      glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
      glfwSetInputMode(window, GLFW_RAW_MOUSE_MOTION, GLFW_TRUE);
      io->mouseHidden = true;
      cam->enable = false;
    }else{
      glfwSetInputMode(window, GLFW_RAW_MOUSE_MOTION, GLFW_FALSE);
      glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
      io->mouseHidden = false;
      cam->enable = true;
    }
  }
}
