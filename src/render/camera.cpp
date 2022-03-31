#include <render/camera.h>
#include <settings.h>
#include <util/IO.h>
#include <render/shader.h>


Camera::Camera(glm::vec3 position, glm::vec3 up, float yaw, float pitch) : Front(glm::vec3(0.0f, 0.0f, -1.0f)), Zoom(ZOOM)
{
  Position = position;
  SystemUp = up;
  Yaw = 0.0f;
  Pitch = 0.0f;
  Fov = 0.25f * 3.1415f;
  Zoom = ZOOM;
  Near = NEAR;
  Far = FAR;
  updateCameraVectors();
  Settings::AddCamera(this);

}
// constructor with scalar values
Camera::Camera(float posX, float posY, float posZ, float upX, float upY, float upZ, float yaw, float pitch) : Front(glm::vec3(0.0f, 0.0f, -1.0f)), Zoom(ZOOM)
{
  Position = glm::vec3(posX, posY, posZ);
  SystemUp = glm::vec3(upX, upY, upZ);
  Yaw = 0.0f;
  Pitch = 0.0f;
  Fov = 0.25f * 3.1415f;
  Zoom = ZOOM;
  Near = NEAR;
  Far = FAR;
  updateCameraVectors();
  Settings::AddCamera(this);
}
glm::mat4 Camera::ProjectionMatrix()
{
  if(orthographic){
    float width = (float)Settings::GetWidth();
    float height = (float)Settings::GetHeight();

    return glm::ortho((-width/Zoom), (width/Zoom), (-height/Zoom), (height/Zoom), -1000.0f, 1000.0f);
  }else{
    return glm::perspective(glm::radians(Zoom), (float)Settings::GetWidth() / (float)Settings::GetHeight(), Near, Far);
  }
}
glm::mat4 Camera::ViewMatrix()
{
  return glm::lookAt(Position, Position + Front, Up);
//  return glm::lookAt(-LookAt, LookAt, Up);
}
glm::mat4 Camera::ViewMatrix(glm::vec3 direction)
{
  return glm::lookAt(-direction, direction, Up);
}
glm::vec3 Camera::TransformDirection(glm::vec3 v){
  return v * Front;
}
void Camera::SetPosition(glm::vec3 _pos){
  Position = _pos;
}
void Camera::SetLocalPosition(glm::vec3 _pos){
  localPosition = _pos;
}
void Camera::Move(glm::vec3 _pos){
  Position += _pos;
}
void Camera::SetRotation(glm::vec2 _rot){
  Yaw   = _rot.x;
  Pitch = _rot.y;

  // make sure that when pitch is out of bounds, screen doesn't get flipped
  if (Pitch > 89.0f){
    Pitch = 89.0f;
  }
  if (Pitch < -89.0f){
    Pitch = -89.0f;
  }

  // update Front, Right and Up Vectors using the updated Euler angles
  updateCameraVectors();
}

// calculates the front vector from the Camera's (updated) Euler Angles
void Camera::updateCameraVectors(){
  glm::vec3 front;
  front.x = cos(glm::radians(Yaw)) * cos(glm::radians(Pitch));
  front.y = sin(glm::radians(Pitch));
  front.z = sin(glm::radians(Yaw)) * cos(glm::radians(Pitch));
  Front = glm::normalize(front);
  Right = glm::normalize(glm::cross(Front, SystemUp));  // normalize the vectors, because their length gets closer to 0 the more you look up or down which results in slower movement.
  Up    = glm::normalize(glm::cross(Right, Front));
}
void Camera::SetUniforms(Shader * shader){
  shader->use();
  shader->setMat4("projection", ProjectionMatrix());
  shader->setMat4("view", ViewMatrix());
}
/*
void Camera::updateCameraVectors(glm::vec3 front){
  Front = glm::normalize(front);
  Right = glm::normalize(glm::cross(Front, SystemUp));  // normalize the vectors, because their length gets closer to 0 the more you look up or down which results in slower movement.
  Up    = glm::normalize(glm::cross(Right, Front));
}
*/
