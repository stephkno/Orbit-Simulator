#ifndef CAMERA_H
#define CAMERA_H

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <vector>
#include <iostream>
#include <glm/gtx/string_cast.hpp>
#include <boost/serialization/vector.hpp>;
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/xml_oarchive.hpp>
#include <boost/archive/xml_iarchive.hpp>
#include <glm/serialize.hpp>

class IO;
class Shader;

// Default camera values
const float YAW         =  0.0f;
const float PITCH       =  0.0f;
const float ZOOM        =  1.0f;
const float NEAR        =  0.1f;
const float FAR        =  1000.0f;

// Serializable
//
// An abstract camera class that processes input and calculates the corresponding Euler Angles, Vectors and Matrices for use in OpenGL
class Camera
{
private:
  // Allow serialization to access non-public data members.
  friend class boost::serialization::access;

  // Serialize the std::vector member of Info
  template<class Archive>
  void serialize(Archive & ar, const unsigned int version)
  {
    ar & Position & localPosition & Front & Up & Right & SystemUp & LookAt & Yaw & Pitch;
  }

public:
    // camera Attributes
    glm::vec3 Position;
    glm::vec3 localPosition;
    glm::vec3 Front;
    glm::vec3 Up;
    glm::vec3 Right;
    glm::vec3 SystemUp;
    glm::vec3 LookAt;
    float Zoom=1.0f;
    float Fov;
    float Near;
    float Far;
    // euler Angles
    float Yaw;
    float Pitch;
    bool orthographic=false;

    // constructor with vectors
    Camera(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f), float yaw = YAW, float pitch = PITCH);
      // constructor with scalar values
    Camera(float posX, float posY, float posZ, float upX, float upY, float upZ, float yaw, float pitch);

    // returns the view matrix calculated using Euler Angles and the LookAt Matrix
    glm::mat4 ViewMatrix();
    glm::mat4 ViewMatrix(glm::vec3 direction);
    glm::mat4 ProjectionMatrix();
    glm::mat4 ProjectionMatrix(glm::vec2 viewport);
    glm::vec3 TransformDirection(glm::vec3 v);
    void SetPosition(glm::vec3 _pos);
    void SetLocalPosition(glm::vec3 _pos);
    void Move(glm::vec3 _pos);
    void SetRotation(glm::vec2 _rot);
    // calculates the front vector from the Camera's (updated) Euler Angles
    void updateCameraVectors();

    void SetUniforms(Shader * shader);
};
#endif
