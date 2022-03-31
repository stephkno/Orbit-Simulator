#pragma once
#include <vector>

class Polygons{
public:
  // list of triangles
  // faces = {
  //          { 0, 1, 2, 3, 4, },
  //          { 5, 6, 7 },
  //          { 8, 9, 10 } };
  std::vector< std::vector< uint32_t > > faces;
  std::vector<uint32_t> p;

  unsigned int textureIndex;
  void Clear(){
    p.clear();
    faces.clear();
  }

  void AddTri(uint32_t tri){
    p.push_back(tri);
  }

  void AddPoly(){
    faces.push_back(p);
    p.clear();
  }
  GLuint PolyFromTri(int tri){
    int f = 0;

    for(std::vector<unsigned int> face : faces){
      for(unsigned int i :  face){
        if(i == tri){
          return f;
        }
        f++;
      }
    }
  }
};
