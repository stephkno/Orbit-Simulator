#include <render/free_cam.h>

// processes input received from any keyboard-like input system. Accepts input parameter in the form of camera defined ENUM (to abstract it from windowing systems)
void FreeCam::ProcessKeyboard(Camera_Movement direction, float deltaTime)
{
  if(!perspective_ortho){
    PerspectiveMove(direction, deltaTime);
  }
  else{
    OrthoMove(direction, deltaTime);
  }
}
void FreeCam::PerspectiveMove(Camera_Movement direction, float deltaTime){
  float velocity = MovementSpeed * deltaTime;

  if (direction == FORWARD){
    Position += Front * velocity;
  }
  if (direction == BACKWARD){
    Position -= Front * velocity;
  }
  if (direction == LEFT){
    Position -= Right * velocity;
  }
  if (direction == RIGHT){
    Position += Right * velocity;
  }
}
void FreeCam::OrthoMove(Camera_Movement direction, float deltaTime){
  float velocity = MovementSpeed * deltaTime;
  glm::vec3 f = Front * glm::vec3(1.0f, 0.0f, 1.0f);
  if (direction == FORWARD){
    Position += f * velocity;
  }
  if (direction == BACKWARD){
    Position -= f * velocity;
  }
  if (direction == LEFT){
    Position -= Right * velocity;
  }
  if (direction == RIGHT){
    Position += Right * velocity;
  }
  if(direction==UP){
    Position += glm::vec3(0.0f, 1.0f, 0.0f) * velocity;
  }
  if(direction==DOWN){
    Position -= glm::vec3(0.0f, 1.0f, 0.0f) * velocity;
  }
}

void FreeCam::Step(GLFWwindow * window, float deltaTime){

  if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS){
    ProcessKeyboard(UP, deltaTime);
  }
  if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS){
    ProcessKeyboard(DOWN, deltaTime);
  }
  if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS){
    ProcessKeyboard(LEFT, deltaTime);
  }
  if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS){
    ProcessKeyboard(RIGHT, deltaTime);
  }

  ProcessMouseScroll(IO::GetScrollY());

}
