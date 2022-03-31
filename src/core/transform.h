#pragma once

#include <glm/ext/matrix_transform.hpp>
#include <vector>
#include <glm/glm.hpp>
#include <string>
#include <sstream>
#include <box.h>

#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>


class Handle;
class Transform{
private:
  // Allow serialization to access non-public data members.
  friend class boost::serialization::access;

  // Serialize the std::vector member of Info
  template<class Archive>
  void serialize(Archive & ar, const unsigned int version)
  {
    ar & position & localPosition & rotation & localRotation & scale;
  }
public:
  glm::vec3 position;
  glm::vec3 localPosition;
  glm::vec3 rotation;
  glm::vec3 localRotation;
  glm::vec3 scale;
  Transform() :
  position(glm::vec3(0.0f, 0.0f, 0.0f)),
  localPosition(glm::vec3(0.0f, 0.0f, 0.0f)),
  rotation(glm::vec3(0.0f, 0.0f, 0.0f)),
  localRotation(glm::vec3(0.0f, 0.0f, 0.0f)),
  scale(glm::vec3(1.0f, 1.0f, 1.0f)) {
  }

  Transform(Transform * t) :
  position(glm::vec3(0.0f, 0.0f, 0.0f)),
  localPosition(glm::vec3(0.0f, 0.0f, 0.0f)),
  rotation(glm::vec3(0.0f, 0.0f, 0.0f)),
  localRotation(glm::vec3(0.0f, 0.0f, 0.0f)),
  scale(glm::vec3(1.0f, 1.0f, 1.0f)) {
    position = t->position;
    rotation = t->rotation;
    scale = t->scale;
  }


  ~Transform(){
  }

  std::string to_string(){
    std::stringstream ss;

    ss << "\n| Position: " << position.x << ", " << position.y << ", " << position.z
       << "\n| Rotation: " << rotation.x << ", " << rotation.y << ", " << rotation.z
       << "\n| Scale: " << scale.x << ", " << scale.y << ", " << scale.z;

       return ss.str();
  }
  void Rotate(glm::vec3 dR){
    rotation += dR;
  }
  void Translate(glm::vec3 dT){
    position += dT;
  }
  void Scale(glm::vec3 dS){
    scale += dS;
  }
  glm::vec3 GetRotation(){
    return rotation;
  }
  void SetPosition(glm::vec3 pos){
    position = pos;
  }
  void SetRotation(glm::vec3 rot){
    rotation = rot;
  }
  void LerpPos(glm::vec3 pos, float t){
    position.x = lerp(position.x, pos.x, t);
    position.y = lerp(position.y, pos.y, t);
    position.z = lerp(position.z, pos.z, t);
  }
  float lerp(float a, float b, float t){
    return a + (b - a) * t;
  }
  void MoveTo(glm::vec3 pos){
    glm::vec3 d = pos - position;
    position += d;
  }
  glm::mat4 ModelMatrix(){
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, position);
    model = glm::rotate(model, rotation.x, glm::vec3(1.0f, 0.0f, 0.0f));
    model = glm::rotate(model, rotation.y, glm::vec3(0.0f, 1.0f, 0.0f));
    model = glm::rotate(model, rotation.z, glm::vec3(0.0f, 0.0f, 1.0f));
    model = glm::scale(model, scale);

    model = glm::translate(model, localPosition);
    model = glm::rotate(model, localRotation.x, glm::vec3(1.0f, 0.0f, 0.0f));
    model = glm::rotate(model, localRotation.y, glm::vec3(0.0f, 1.0f, 0.0f));
    model = glm::rotate(model, localRotation.z, glm::vec3(0.0f, 0.0f, 1.0f));

    model = glm::translate(model, localPosition);

    return model;
  }

};


class TransformGizmo{
public:
  Handle * handles[3];

  // constructor
  TransformGizmo();
  // destructor
  ~TransformGizmo();
  glm::vec3 MouseDown(glm::vec2 mouse, glm::vec3 position);
  glm::vec3 MouseDrag(glm::vec2 mouse, glm::vec3 position);
  void Render(glm::vec3 _position);

};
