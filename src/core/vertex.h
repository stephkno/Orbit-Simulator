#pragma once
#include <glm/glm.hpp>
#include <GL/glew.h>
#include <glm/serialize.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/serialization/serialization.hpp>
#include <deps/glm/serialize.hpp>
#include <glm/gtx/string_cast.hpp>
#include <iostream>

const int MAX_BONE_INFLUENCE = 4;

class Vertex {
private:
  // Allow serialization to access non-public data members.
  friend class boost::serialization::access;

  // Allow serialization to access non-public data members.
  template<class Archive>
  void serialize(Archive & ar, const unsigned int version)
  {
    ar & Position & Normal & TexCoords & Color & Tangent & Bitangent;// & m_BoneIDs & m_Weights;
  }

public:
    // position
    glm::vec3 Position;
    // normal
    glm::vec3 Normal;
    // texCoords
    glm::vec2 TexCoords;
    // colors
    glm::vec3 Color;
    // tangent
    glm::vec3 Tangent;
    // bitangent
    glm::vec3 Bitangent;
    //bone indexes which will influence this vertex
  	int m_BoneIDs[MAX_BONE_INFLUENCE];
  	//weights from each bone
  	float m_Weights[MAX_BONE_INFLUENCE];

    bool selected=false;

    void PrintVertex(){
      std::cout << "Print vertex" << std::endl;
      std::cout << glm::to_string(Position) << std::endl;
      std::cout << glm::to_string(Normal) << std::endl;
      std::cout << glm::to_string(TexCoords) << std::endl;
      std::cout << glm::to_string(Color) << std::endl;
      std::cout << glm::to_string(Tangent) << std::endl;
      std::cout << glm::to_string(Bitangent) << std::endl;

    }

    Vertex(Vertex vert, float u, float v){
      Position = vert.Position;
      Normal = vert.Normal;
      TexCoords = glm::vec2(u,v);
      Color = vert.Color;
      Tangent = vert.Tangent;
      Bitangent = vert.Bitangent;
    }
    Vertex(Vertex vert, float u, float v, float nx, float ny, float nz){
      Position = vert.Position;
      Normal = glm::vec3(nx, ny, nz);
      TexCoords = glm::vec2(u,v);
      Color = vert.Color;
      Tangent = vert.Tangent;
      Bitangent = vert.Bitangent;
    }
    Vertex(float x, float y, float z, float u, float v){
      Position = glm::vec3(x,y,z);
      Normal = glm::vec3(0.0f, 0.0f, 0.0f);
      TexCoords = glm::vec2(u,v);
      Color = glm::vec3(0.0f);
      Tangent = glm::vec3(0.0f);
      Bitangent = glm::vec3(0.0f);
    }
    Vertex(float x, float y, float z){
      Position = glm::vec3(x,y,z);
      Normal = glm::vec3(0.0f, 0.0f, 0.0f);
      TexCoords = glm::vec2(0.0f);
      Color = glm::vec3(0.0f);
      Tangent = glm::vec3(0.0f);
      Bitangent = glm::vec3(0.0f);
    }
    Vertex(glm::vec3 p, glm::vec3 n, glm::vec2 uv, glm::vec3 t, glm::vec3 b){
      Position = p;
      Normal = n;
      TexCoords = uv;
      Tangent = t;
      Bitangent = b;
      Color = glm::vec3(0.0f);
    }
    Vertex(glm::vec3 p, glm::vec3 n){
      Position = p;
      Normal = n;
      TexCoords = glm::vec2(0.0f,0.0f);
      Color = glm::vec3(0.0f);
      Tangent = glm::vec3(0.0f);
      Bitangent = glm::vec3(0.0f);
    }
    Vertex(glm::vec3 p, glm::vec3 n, float u, float v){
      Position = p;
      Normal = n;
      TexCoords = glm::vec2(u,v);
      Color = glm::vec3(0.0f);
      Tangent = glm::vec3(0.0f);
      Bitangent = glm::vec3(0.0f);
    }
    Vertex(glm::vec3 p, glm::vec3 n, glm::vec3 c){
      Position = p;
      Normal = n;
      TexCoords = glm::vec2(0.0f,0.0f);
      Color = c;
      Tangent = glm::vec3(0.0f);
      Bitangent = glm::vec3(0.0f);
    }
    Vertex(glm::vec3 p, glm::vec2 uv){
      Position = p;
      TexCoords = uv;
      Normal = glm::vec3(0.0f, 0.0f, 0.0f);
      Color = glm::vec3(0.0f);
      Tangent = glm::vec3(0.0f);
      Bitangent = glm::vec3(0.0f);
    }
    Vertex(glm::vec3 p, float u, float v){
      Position = p;
      TexCoords = glm::vec2(u,v);
      Normal = glm::vec3(0.0f, 0.0f, 0.0f);
      Color = glm::vec3(0.0f);
      Tangent = glm::vec3(0.0f);
      Bitangent = glm::vec3(0.0f);
    }
    Vertex(glm::vec3 p, glm::vec3 n, glm::vec2 uv){
      Position = p;
      TexCoords = uv;
      Normal = n;
      Color = glm::vec3(0.0f);
      Tangent = glm::vec3(0.0f);
      Bitangent = glm::vec3(0.0f);
    }
    Vertex(glm::vec3 p){
      Position = p;
      Normal = glm::vec3(0.0f, 0.0f, 0.0f);
      TexCoords = glm::vec2(0.0f);
      Color = glm::vec3(0.0f);
      Tangent = glm::vec3(0.0f);
      Bitangent = glm::vec3(0.0f);
    }
    Vertex(int x, int y, int z){
      Position = glm::vec3((float)x, (float)y, (float)z);
      Normal = glm::vec3(0.0f, 0.0f, 0.0f);
      TexCoords = glm::vec2(0.0f);
      Color = glm::vec3(0.0f);
      Tangent = glm::vec3(0.0f);
      Bitangent = glm::vec3(0.0f);
    }
    Vertex(){
      Position = glm::vec3(0.0f);
      Normal = glm::vec3(0.0f, 0.0f, 0.0f);
      TexCoords = glm::vec2(0.0f);
      Color = glm::vec3(0.0f);
      Tangent = glm::vec3(0.0f);
      Bitangent = glm::vec3(0.0f);
    }
    void MoveTo(glm::vec3 pos){
      glm::vec3 d = Position - pos;
      Position += d;
    }
    void Scale(glm::vec3 s){
      Position.x = Position.x * s.x;
      Position.y = Position.y * s.y;
      Position.z = Position.z * s.z;
    }
};
