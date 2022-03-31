#ifndef MESH_H
#define MESH_H

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <string>
#include <vector>
#include <iostream>
#include <map>
#include <vertex.h>

#include <boost/serialization/vector.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <glm/serialize.hpp>
#include <render/material.h>

class Transform;
class Shader;

#define MAX_INSTANCE 10000

using namespace std;

class Mesh {
private:
  // Allow serialization to access non-public data members.
  friend class boost::serialization::access;

  // Serialize the std::vector member of Info
  template<class Archive>
  void serialize(Archive & ar, const unsigned int version)
  {
    // serialize vertices
    if(!Archive::is_loading::value){
      n_vertices=vertices.size();
    }
    ar & n_vertices;

    if(Archive::is_loading::value){
      for(int i = 0; i < n_vertices; i++){
        Vertex v;
        ar & v;
        vertices.push_back(v);
      }
    }else{
      for(int i = 0; i < n_vertices; i++){
        ar & vertices.at(i);
      }
    }

    // serialize indices
    if(!Archive::is_loading::value){
      n_indices=indices.size();
    }
    ar & n_indices;

    if(Archive::is_loading::value){
      for(int i = 0; i < n_indices; i++){
        uint v;
        ar & v;
        indices.push_back(v);
      }
    }else{
      for(int i = 0; i < n_indices; i++){
        ar & indices.at(i);
      }
    }

    if(!Archive::is_loading::value){
      n_materials=materials.size();
    }
    ar & n_materials;

    if(Archive::is_loading::value){
      for(int i = 0; i < n_materials; i++){
        Material * mat;
        ar & mat;
        materials.push_back(mat);
      }
    }
    else{
      for(int i = 0; i < n_materials; i++){
        ar & materials[i];
      }
    }

    if(Archive::is_loading::value) {
      CalculateNormals();
      setupMesh();
    }
  }
  // serialize
  // vertices
  // indices
  // name
  // local transform eventually
  //

public:
    // mesh Data
    vector<Vertex> vertices;
    vector<uint> indices;

    Transform * localTransform=nullptr;
    vector<Material*> materials = vector<Material*>();
    Shader shader;

    int n_vertices;
    int n_indices;
    int n_materials;

    glm::vec3 baseColor = glm::vec3(1.0f);

    unsigned int VAO=0;
    glm::vec2 textureScale = glm::vec2(1.0f);
    bool is_static = false;
    bool selected = false;
    glm::vec3 centerPoint = glm::vec3(0.0f);

    GLenum renderMode = GL_TRIANGLES;
    ~Mesh(){
      for(Material * mat : materials){
        delete mat;
      }
      glDeleteVertexArrays(1,&VAO);
    }

    // constructor
    Mesh(vector<Vertex> vertices, vector<unsigned int> indices, Material * material)
    {
        this->vertices = vertices;
        this->indices = indices;
        this->materials.push_back(material);

        //translations.push_back(glm::vec3(0.0f));
        //translations.push_back(glm::vec3(10.0f, 0.0f, 10.0f));

        CalculateTangents();
        CalculateNormals();
        // now that we have all the required data, set the vertex buffers and its attribute pointers.
        setupMesh();
    }
    // constructor
    Mesh(vector<Vertex> vertices, vector<unsigned int> indices)
    {
        this->vertices = vertices;
        this->indices = indices;

        //translations.push_back(glm::vec3(0.0f));
        //translations.push_back(glm::vec3(10.0f, 0.0f, 10.0f));

        CalculateTangents();
        CalculateNormals();

        // now that we have all the required data, set the vertex buffers and its attribute pointers.
        //setupMesh();
    }
    Mesh(Mesh * mesh){
      for(Vertex v : mesh->vertices){
        vertices.push_back(v);
      }
      for(uint i : mesh->indices){
        indices.push_back(i);
      }
      for(Material * mat : mesh->materials){
        materials.push_back(mat);
      }
      materials.size();
      setupMesh();
    }
    Mesh()
    {
    }

    void SnapVertices(){
      int i;
      for(Vertex v : vertices){
        v.Position.x = round(v.Position.x);
        v.Position.y = round(v.Position.y);
        v.Position.z = round(v.Position.z);
      }
    }
    void Flip(){
      for(Vertex & v : vertices){
        v.Normal *= -1.0f;
      }
      std::reverse(indices.begin(), indices.end());
      Refresh();
    }
    void Refresh(){
      setupMesh();
    }
    void CalculateUVs(){
      for(int i = 0; i < indices.size(); i+=3){
        glm::vec2 a = CalculateUVForVertex(i, i+1, i+2, i);
        glm::vec2 b = CalculateUVForVertex(i, i+1, i+2, i+1);
        glm::vec2 c = CalculateUVForVertex(i, i+1, i+2, i+2);
        vertices[indices[i]].TexCoords = a;
        vertices[indices[i+1]].TexCoords = b;
        vertices[indices[i+2]].TexCoords = c;
      }
    }
    glm::vec2 CalculateUVForVertex(uint apoint, uint bpoint, uint cpoint, uint point){
      glm::vec2 bary;
      glm::vec3 p = vertices[indices[point]].Position;
      glm::vec3 normal = vertices[indices[point]].Normal;
      glm::vec3 a = vertices[indices[apoint]].Position;
      glm::vec3 b = vertices[indices[bpoint]].Position;
      glm::vec3 c = vertices[indices[cpoint]].Position;

      float areaABC = glm::dot( normal, glm::cross( (b-a), (c-a) ) );
      float areaPBC = glm::dot( normal, glm::cross( (b-p), (c-p) ) );
      float areaPCA = glm::dot( normal, glm::cross( (c-p), (a-p) ) );

      bary.x = areaPBC / areaABC;
      bary.y = areaPCA / areaABC;
      //z = 1.0f - bary.x / bary.y;
      return bary;
    }
    // scale normals with transform
    void CalculateNormals(){
      if(indices.size() < 3 || vertices.size() < 3){
        return;
      }
      //find normal of triangle//
      //assign normal to each vertex//

      // for each vertex, v:
      // find list of vertices that are sharing triangle with vertex v
      // find average normal of vertices
      // set v.normal to average

      std::vector<glm::vec3> buff;

      for(GLuint a = 0; a < indices.size(); a+=3){

        glm::vec3 p3 = vertices.at(indices.at(a+2)).Position;
        glm::vec3 p2 = vertices.at(indices.at(a+1)).Position;
        glm::vec3 p1 = vertices.at(indices.at(a)).Position;

        // calculate normal of triangle from three vertex positions
        glm::vec3 v1 = p2 - p1;
        glm::vec3 v2 = p3 - p1;
        glm::vec3 normal = glm::normalize(glm::cross(v1,v2));

        // set normal for vertices
        vertices.at(indices.at(a)).Normal = normal;
        vertices.at(indices.at(a+1)).Normal = normal;
        vertices.at(indices.at(a+2)).Normal = normal;
      }
      // for each vertex
      // find 4 or 8 neighboring vertices
      // set normal = avg normals
    }
    void CalculateTangents(){

      // for each triangle
      for(int i = 0; i < indices.size()-1; i+=3){

        Vertex a = vertices.at(indices.at(i));
        Vertex b = vertices.at(indices.at(i+1));
        Vertex c = vertices.at(indices.at(i+2));

        glm::vec3 pos1 = a.Position;
        glm::vec3 pos2 = b.Position;
        glm::vec3 pos3 = c.Position;
        glm::vec2 uv1 = a.TexCoords;
        glm::vec2 uv2 = b.TexCoords;
        glm::vec2 uv3 = c.TexCoords;

        glm::vec3 tangent;
        glm::vec3 bitangent;

        glm::vec3 edge1 = pos2 - pos1;
        glm::vec3 edge2 = pos3 - pos1;

        glm::vec2 deltaUV1 = uv2 - uv1;
        glm::vec2 deltaUV2 = uv3 - uv1;

        float f = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV2.x * deltaUV1.y);

        tangent.x = f * (deltaUV2.y * edge1.x - deltaUV1.y * edge2.x);
        tangent.y = f * (deltaUV2.y * edge1.y - deltaUV1.y * edge2.y);
        tangent.z = f * (deltaUV2.y * edge1.z - deltaUV1.y * edge2.z);

        bitangent.x = f * (-deltaUV2.x * edge1.x + deltaUV1.x * edge2.x);
        bitangent.y = f * (-deltaUV2.x * edge1.y + deltaUV1.x * edge2.y);
        bitangent.z = f * (-deltaUV2.x * edge1.z + deltaUV1.x * edge2.z);

        vertices.at(indices.at(i)).Tangent = tangent;
        vertices.at(indices.at(i)).Bitangent = bitangent;
        vertices.at(indices.at(i+1)).Tangent = tangent;
        vertices.at(indices.at(i+1)).Bitangent = bitangent;
        vertices.at(indices.at(i+2)).Tangent = tangent;
        vertices.at(indices.at(i+2)).Bitangent = bitangent;

        for(int j = 0; j<3;j++){
          if(glm::all(glm::isnan(vertices.at(indices.at(i+j)).Tangent))){
            vertices.at(indices.at(i+j)).Tangent=glm::vec3(0.0f);
          }
          if(glm::all(glm::isnan(vertices.at(indices.at(i+j)).Bitangent))){
            vertices.at(indices.at(i+j)).Bitangent=glm::vec3(0.0f);
          }
        }

      }
    }
    // render the mesh
    void Draw(Shader *shader, Transform *transform);
    void LineDraw(Shader *shader, Transform * transform);
    void PointDraw(Shader *shader, Transform * transform);

    void DrawThumbnail(Shader *shader);

    uint AddVertex(Vertex v){
      int i = vertices.size();
      indices.push_back(i);
      vertices.push_back(v);
      return i;
    }
    Vertex * GetVertex(int i){
      if(i < 0 || i >= vertices.size()){
        return nullptr;
      }
      return &vertices.at(i);
    }
    void ScaleVertices(glm::vec3 s){
      for(Vertex& v : vertices){
        v.Scale(s);
      }
      setupMesh();
    }
    void SetInstances(){
      int index = 0;
      float offset = 0.1f;
      for (int y = -10; y < 10; y += 2)
      {
          for (int x = -10; x < 10; x += 2)
          {
              glm::vec3 translation;
              translation.z = (float)x / 10.0f + offset;
              translation.y = (float)y / 10.0f + offset;
              translations.push_back(translation);
          }
      }
    }
private:
    // render data
    unsigned int VBO, EBO;
    unsigned int instanceVBO;
    int n_instances = 0;
    std::vector<glm::vec3> translations;

    // initializes all the buffer objects/arrays
    void setupMesh()
    {
      if(VAO != 0){
        glDeleteVertexArrays(1, &VAO);
      }

      // create buffers/arrays
      glGenVertexArrays(1, &VAO);
      glGenBuffers(1, &VBO);
      glGenBuffers(1, &EBO);

      glBindVertexArray(VAO);

      // store instance data in an array buffer
      // --------------------------------------
      glGenBuffers(1, &instanceVBO);
      glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);
      glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * n_instances, &translations[0], GL_STATIC_DRAW);
      glBindBuffer(GL_ARRAY_BUFFER, 0);

      // load data into vertex buffers
      glBindBuffer(GL_ARRAY_BUFFER, VBO);
      // A great thing about structs is that their memory layout is sequential for all its items.
      // The effect is that we can simply pass a pointer to the struct and it translates perfectly to a glm::vec3/2 array which
      // again translates to 3/2 floats which translates to a byte array.
      glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);

      glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
      glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

      // set the vertex attribute pointers
      // vertex Positions
      glEnableVertexAttribArray(0);
      glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
      // vertex normals
      glEnableVertexAttribArray(1);
      glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Normal));
      // vertex texture coords
      glEnableVertexAttribArray(2);
      glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, TexCoords));
      // vertex colors
      glEnableVertexAttribArray(3);
      glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Color));
      // vertex tangent
      glEnableVertexAttribArray(4);
      glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Tangent));
      // vertex bitangent
      glEnableVertexAttribArray(5);
      glVertexAttribPointer(5, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Bitangent));

      glEnableVertexAttribArray(6);
      glVertexAttribIPointer(6, 4, GL_INT, sizeof(Vertex), (void*)offsetof(Vertex, m_BoneIDs));

      glEnableVertexAttribArray(7);
      glVertexAttribPointer(7, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, m_Weights));

      // also set instance data
      glEnableVertexAttribArray(8);
      glBindBuffer(GL_ARRAY_BUFFER, instanceVBO); // this attribute comes from a different vertex buffer
      glVertexAttribPointer(8, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
      glBindBuffer(GL_ARRAY_BUFFER, 0);
      glVertexAttribDivisor(3, 1); // tell OpenGL this is an instanced vertex attribute.

    }
};
#endif
/*
bool operator==(const Vertex& v) {
  return v.Position == Position;
}
bool operator!=(const Vertex& v) {
  return v.Position != Position;
}
Vertex operator+(const Vertex& v) {
  Vertex vout;
  vout.Position = v.Position + Position;
  return vout;
}
Vertex operator-(const Vertex& v) {
  Vertex vout;
  vout.Position = v.Position - Position;
  return vout;
}
Vertex operator*(const Vertex& v) {
  Vertex vout;
  vout.Position = v.Position * Position;
  return vout;
}
Vertex operator/(const Vertex& v) {
  Vertex vout;
  vout.Position = v.Position / Position;
  return vout;
}
Vertex operator+(const float& f) {
  Vertex vout;
  vout.Position = Position + f;
  return vout;
}
Vertex operator-(const float& f) {
  Vertex vout;
  vout.Position = Position - f;
  return vout;
}
Vertex operator*(const float& f) {
  Vertex vout;
  vout.Position = Position * f;
  return vout;
}
Vertex operator/(const float& f) {
  Vertex vout;
  vout.Position = Position / f;
  return vout;
}
static Vertex zero(){
  return Vertex(0.0f, 0.0f, 0.0f);
}
*/
