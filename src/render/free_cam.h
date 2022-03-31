#pragma once
#include <render/camera.h>
#include <util/IO.h>

// Defines several possible options for camera movement. Used as abstraction to stay away from window-system specific input methods
enum Camera_Movement {
    FORWARD,
    BACKWARD,
    LEFT,
    RIGHT,
    UP,
    DOWN,
};

class FreeCam : public Camera{
public:
  // camera options
  float MovementSpeed =  50.0f;
  float MouseSensitivity = 0.03f;
  float MovementAccelerationSlow = 5.0f;
  float MovementAccelerationFast = 10.0f;
  float MovementDecay = 0.9f;
  bool enable = true;
  bool perspective_ortho = false;
  bool wireframeRender=false;
  bool RotationLock=false;

  int cameraMode=0;

  void Step(GLFWwindow * window, float deltaTime);
  // processes input received from any keyboard-like input system. Accepts input parameter in the form of camera defined ENUM (to abstract it from windowing systems)
  void ProcessKeyboard(Camera_Movement direction, float deltaTime);
  void PerspectiveMove(Camera_Movement direction, float deltaTime);
  void OrthoMove(Camera_Movement direction, float deltaTime);
  // processes input received from a mouse input system. Expects the offset value in both the x and y direction.
  void ProcessMouseMovement(float xoffset, float yoffset, GLboolean constrainPitch = true)
  {

      xoffset *= MouseSensitivity;
      yoffset *= MouseSensitivity;

      if(enable&&!RotationLock){
        if(!perspective_ortho){
          Pitch += yoffset;
        }
        Yaw += xoffset;
      }

      // make sure that when pitch is out of bounds, screen doesn't get flipped
      if (constrainPitch)
      {
          if (Pitch > 90.0f){
            Pitch = 90.0f;
          }
          if (Pitch < -90.0f){
            Pitch = -90.0f;
          }
      }

      // update Front, Right and Up Vectors using the updated Euler angles
      updateCameraVectors();
  }

  // processes input received from a mouse scroll-wheel event. Only requires input on the vertical wheel-axis
  void ProcessMouseScroll(float yoffset)
  {
      Zoom = (float)yoffset;
      if (Zoom < 1.0f){
        Zoom = 1.0f;
      }
      if (Zoom > 45.0f){
        Zoom = 45.0f;
      }
  }
};
