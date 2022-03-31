#pragma once
#include <glm/glm.hpp>
#include <GL/glew.h>
#include <vector>
#include <util/Polygon.h>
#include <iostream>

#include <glm/serialize.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/xml_oarchive.hpp>
#include <boost/archive/xml_iarchive.hpp>
#include <vertex.h>
//
// Box is defined by two bounding points
//
//    B.________.
//    / \      /|
//   /___\___./ |
//   |    \  |  .
//   |     \ | /
//   .______\./
//           A
// z
// ^   7________6
// |  /|       /|
// | 4_|_____5/ |     y
// | | 3_____|__2    /
// | |/      | /    /
// | 0_______1/    /
// <--------------->x
//
// geometry corners:
//
/*
top
0 1 2 3
4,5,6,7
bottom
4 5 6 7
0,1,2,3
front
8 91011
2,3,7,6
back
12131415
 0,1,5,4
right
16171819
 1,2,6,5
left
20212223
 3,0,4,7
//
triangles
top
0     1
0,1,2,0,2,3
bottom
4,5,6,4,6,7
front
8,11,8,9,10,11
back
12,13,14,12,14,15
right
16,17,19,19,17,18
left
20,22,23,20,21,22
*/
//
//
class Box {
private:
  // Allow serialization to access non-public data members.
  friend class boost::serialization::access;

  // Serialize the std::vector member of Info
  template<class Archive>
  void serialize(Archive & ar, const unsigned int version)
  {
    ar & position & a & b & box;
  }
public:
  glm::vec3 position;
  // a should always be min
  glm::vec3 a;
  glm::vec3 b;
  glm::vec3 box;
  Polygons polygons;

  Box(glm::vec3 _a=glm::vec3(0.0f), glm::vec3 _b=glm::vec3(0.0f)){
    a = _a;
    b = _b;
    position = glm::vec3(0.0f);
  }

  Polygons GetPolygons();

  // get mesh vertices for new csg brushes
  std::vector<Vertex> GetMeshVertices(){

    float uvx;
    float uvy;

    box = b - a;

    if(box.x < 0){
      box.x *= -1;
    }
    if(box.y < 0){
      box.y *= -1;
    }
    if(box.z < 0){
      box.z *= -1;
    }

    polygons.Clear();

    std::vector<Vertex> v;
    // y
    // ^   7________6
    // |  /|       /|
    // | 4_|_____5/ |     z
    // | | 3_____|__2    /
    // | |/      | /    /
    // | 0_______1/    /
    // <--------------->x
    //
    // geometry corners:

    // back, poly 0
    uvx = box.x;
    uvy = box.y;
    v.push_back(Vertex(GetCorner(0), glm::vec2(0.0f, 0.0f)));
    v.push_back(Vertex(GetCorner(5), glm::vec2(uvx, uvy)));
    v.push_back(Vertex(GetCorner(4), glm::vec2(0.0f, uvy)));
    v.push_back(Vertex(GetCorner(0), glm::vec2(0.0f, 0.0f)));
    v.push_back(Vertex(GetCorner(1), glm::vec2(uvx, 0.0f)));
    v.push_back(Vertex(GetCorner(5), glm::vec2(uvx, uvy)));

    polygons.AddTri(0);
    polygons.AddTri(1);
    polygons.AddPoly();

    // right
    uvx = box.z;
    uvy = box.y;
    v.push_back(Vertex(GetCorner(1), glm::vec2(0.0f, 0.0f)));
    v.push_back(Vertex(GetCorner(6), glm::vec2(uvx, uvy)));
    v.push_back(Vertex(GetCorner(5), glm::vec2(0.0f, uvy)));
    v.push_back(Vertex(GetCorner(1), glm::vec2(0.0f, 0.0f)));
    v.push_back(Vertex(GetCorner(2), glm::vec2(uvx, 0.0f)));
    v.push_back(Vertex(GetCorner(6), glm::vec2(uvx, uvy)));

    polygons.AddTri(2);
    polygons.AddTri(3);
    polygons.AddPoly();

    // front
    uvx = box.x;
    uvy = box.y;
    v.push_back(Vertex(GetCorner(2), glm::vec2(0.0f, 0.0f)));
    v.push_back(Vertex(GetCorner(7), glm::vec2(uvx, uvy)));
    v.push_back(Vertex(GetCorner(6), glm::vec2(0.0f, uvy)));
    v.push_back(Vertex(GetCorner(2), glm::vec2(0.0f, 0.0f)));
    v.push_back(Vertex(GetCorner(3), glm::vec2(uvx, 0.0f)));
    v.push_back(Vertex(GetCorner(7), glm::vec2(uvx, uvy)));

    polygons.AddTri(4);
    polygons.AddTri(5);
    polygons.AddPoly();

    // left
    uvx = box.z;
    uvy = box.y;
    v.push_back(Vertex(GetCorner(3), glm::vec2(0.0f, 0.0f)));
    v.push_back(Vertex(GetCorner(4), glm::vec2(uvx, uvy)));
    v.push_back(Vertex(GetCorner(7), glm::vec2(0.0f, uvy)));
    v.push_back(Vertex(GetCorner(3), glm::vec2(0.0f, 0.0f)));
    v.push_back(Vertex(GetCorner(0), glm::vec2(uvx, 0.0f)));
    v.push_back(Vertex(GetCorner(4), glm::vec2(uvx, uvy)));
    polygons.AddTri(6);
    polygons.AddTri(7);
    polygons.AddPoly();

    // top
    uvx = box.x;
    uvy = box.z;
    v.push_back(Vertex(GetCorner(4), glm::vec2(0.0f, 0.0f)));
    v.push_back(Vertex(GetCorner(6), glm::vec2(uvx, uvy)));
    v.push_back(Vertex(GetCorner(7), glm::vec2(0.0f, uvy)));
    v.push_back(Vertex(GetCorner(4), glm::vec2(0.0f, 0.0f)));
    v.push_back(Vertex(GetCorner(5), glm::vec2(uvx, 0.0f)));
    v.push_back(Vertex(GetCorner(6), glm::vec2(uvx, uvy)));
    polygons.AddTri(8);
    polygons.AddTri(9);
    polygons.AddPoly();

    // bottom
    uvx = box.x;
    uvy = box.z;
    v.push_back(Vertex(GetCorner(3), glm::vec2(0.0f, uvy)));
    v.push_back(Vertex(GetCorner(2), glm::vec2(uvx, uvy)));
    v.push_back(Vertex(GetCorner(0), glm::vec2(0.0f, 0.0f)));
    v.push_back(Vertex(GetCorner(2), glm::vec2(uvx, uvy)));
    v.push_back(Vertex(GetCorner(1), glm::vec2(uvx, 0.0f)));
    v.push_back(Vertex(GetCorner(0), glm::vec2(0.0f, 0.0f)));
    polygons.AddTri(10);
    polygons.AddTri(11);
    polygons.AddPoly();

    return v;
  }

  std::vector<GLuint> GetMeshIndices(){
    std::vector<GLuint> indices;
    for(int i = 0; i < 36; i++){
      indices.push_back(i);
    }
    return indices;
  }

  //  get vertices for line renderer
  std::vector<Vertex> GetVertices(){
    std::vector<Vertex> v = {
      GetCorner(0),
      GetCorner(1),
      GetCorner(2),
      GetCorner(3),
      GetCorner(4),
      GetCorner(5),
      GetCorner(6),
      GetCorner(7)
    };
    return v;
  }
  std::vector<GLuint> GetIndices(){
    std::vector<GLuint> i = {
      0, 1, 1, 2, 2, 3, 3, 0,
      4, 5, 5, 6, 6, 7, 7, 4,
      0, 4, 1, 5, 2, 6, 3, 7
    };
    return i;
  }
  std::vector<GLuint> GetTriIndices(){
    std::vector<GLuint> i;

    if(box.x<0 && box.y<0 && box.z<0){
      i = std::vector<GLuint>{
       2, 6, 3, 6, 7, 3,
       4, 0, 3, 7, 4, 3,
       6, 5, 7, 7, 5, 4,
       6, 2, 1, 5, 6, 1,
       5, 1, 0, 4, 5, 0,
       2, 3, 1, 1, 3, 0
     };
    }else{
       i = std::vector<GLuint>{
        0, 3, 1, 1, 3, 2,
        0, 5, 4, 0, 1, 5,
        1, 6, 5, 1, 2, 6,
        4, 5, 7, 7, 5, 6,
        3, 4, 7, 3, 0, 4,
        3, 7, 6, 3, 6, 2
      };
    }
    return i;
  }

  // click triangle -> polygon
  // polygon selected -> hilight triangles of polygon

  //
  // y (A)
  // ^   7________6
  // |  /|       /|
  // | 4_|_____5/ |     z
  // | | 3_____|__2    /
  // | |/      | /    /
  // | 0_______1/    /s
  // ----------(B)--->x
  //
  glm::vec3 GetCorner(int i){
    switch(i){
      case 0:{
        return glm::vec3(a.x, b.y, b.z);
        break;
      }
      case 1:{
        return glm::vec3(b.x, b.y, b.z);
        break;
      }
      case 2:{
        return glm::vec3(b.x, b.y, a.z);
        break;
      }
      case 3:{
        return glm::vec3(a.x, b.y, a.z);
        break;
      }
      case 4:{
        return glm::vec3(a.x, a.y, b.z);
        break;
      }
      case 5:{
        return glm::vec3(b.x, a.y, b.z);
        break;
      }
      case 6:{
        return glm::vec3(b.x, a.y, a.z);
        break;
      }
      case 7:{
        return glm::vec3(a.x, a.y, a.z);
        break;
      }
    }
  }

};
