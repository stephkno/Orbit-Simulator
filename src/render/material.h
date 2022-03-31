#pragma once
#include <util/define.h>
#include <render/shader.h>
#include <glm/glm.hpp>
#include <GL/glew.h>
#include <render/texture.h>
#include <util/file.h>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <boost/serialization/vector.hpp>
#include <glm/serialize.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>

#include <deps/nlohmann/json.hpp>
#include <iostream>
#include <string>

/*
  Base Color
  Metallic
  Smoothness
  Alpha Map
  Emissive
*/

class Material{
private:
  // Allow serialization to access non-public data members.
  friend class boost::serialization::access;

  // Allow serialization to access non-public data members.
  template<class Archive>
  void serialize(Archive & ar, const unsigned int version)
  {
    ar & name & diffuseTexture & normalTexture & specularTexture & metalnessTexture & heightmapTexture & alphaTexture & emissiveTexture;
  }
public:

  std::string name;
  std::string path;

  Texture * diffuseTexture=nullptr;
  Texture * normalTexture=nullptr;;
  Texture * specularTexture=nullptr;;
  Texture * metalnessTexture=nullptr;;
  Texture * heightmapTexture=nullptr;;
  Texture * alphaTexture=nullptr;;
  Texture * emissiveTexture=nullptr;;

  float normalScale=1.0f;

  Vector3 baseColorDiffuse=Vector3(1.0f, 1.0f, 1.0f);
  Vector3 baseColorSpecular=Vector3(1.0f, 1.0f, 1.0f);
  Vector3 baseColorAmbient=Vector3(0.5f);
  Vector3 baseColorEmissive=Vector3(0.0f);
  float shininess=0.0f;

  Material(){
  }
  ~Material(){
    delete diffuseTexture;
    delete normalTexture;
    delete specularTexture;
    delete metalnessTexture;
    delete heightmapTexture;
    delete alphaTexture;
    delete emissiveTexture;
  }
  Material(std::string n){
    name = n;
  }
  // do not use
  void Load(std::string p){
    std::ifstream ifs(p);
    nlohmann::json j = nlohmann::json::parse(ifs);
    this->name = j["name"];
    this->path = p;
    std::string dt = j["texture_diffuse"];
    std::string tn = j["texture_normal"];
    //
    // get material type, extension, filename
    //
    if(dt!=""){
      diffuseTexture->Load(dt);
    }
    if(tn!=""){
      normalTexture->Load(tn);
    }
  }
  void LoadTexture(){
    if(name!=""){
      AddTexture(name);
    }
  }
  void AddTexture(std::string path){
    diffuseTexture->Load(path, "texture_diffuse.jpg");
    normalTexture->Load(path, "texture_normal.jpg");
    specularTexture->Load(path, "texture_specular.jpg");
    this->path = path;
  }
  void PrintInfo(){
    /*
    Print << "Name: " << name << Endl;
    Print << "Diffuse texture: " << diffuseTexture << Endl;
    Print << "Normal texture: " << normalTexture << Endl;
    Print << "Specular texture: " << specularTexture << Endl;
    Print << "Heightmap texture: " << heightmapTexture << Endl;
    Print << "Alpha texture: " << alphaTexture << Endl;
    Print << "Emissive texture: " << emissiveTexture << Endl;
    Print << "Normal scale: " << normalScale << Endl;
    Print << "Base color: " << glm::to_string(baseColorDiffuse) << Endl;
    Print << "Specular color: " << glm::to_string(baseColorSpecular) << Endl;
    Print << "Ambient color: " << glm::to_string(baseColorAmbient) << Endl;
    Print << "Emissive color: " << glm::to_string(baseColorEmissive) << Endl;
    */
  }

  void SetUniforms(Shader * shader){
    shader->use();

    // diffuseTexture.bind(0);
    // normalTexture.bind(1);
    // specularTexture.bind(2);
    // alphaTexture.bind(3);
    // metalnessTexture.bind(4);
    // emissiveTexture.bind(5);
    uint id = 0;

    glActiveTexture(GL_TEXTURE0);
    if(diffuseTexture!=nullptr){
      id = diffuseTexture->id;
    }
    glBindTexture(GL_TEXTURE_2D, id);
    shader->setInt("material[0].diffuseTexture", 0);
    shader->setBool("texEnable", true);

    if(normalTexture!=nullptr){
      id = normalTexture->id;
    }

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, id);
    shader->setInt("material[0].normalTexture", 1);

/*
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, id);
    shader->setInt("material[0].specularTexture", 2);

    glActiveTexture(GL_TEXTURE3);
    glBindTexture(GL_TEXTURE_2D, id);
    shader->setInt("material[0].alphaTexture", 3);

    glActiveTexture(GL_TEXTURE4);
    glBindTexture(GL_TEXTURE_2D, id);
    shader->setInt("material[0].metalnessTexture", 4);

    glActiveTexture(GL_TEXTURE5);
    glBindTexture(GL_TEXTURE_2D, id);
    shader->setInt("material[0].emissiveTexture", 5);
*/
    shader->setVec3("material[0].baseColor", baseColorDiffuse);
    shader->setVec3("material[0].specular", baseColorSpecular);
    shader->setVec3("material[0].emissive", baseColorEmissive);
    shader->setVec3("material[0].ambient", baseColorAmbient);
    shader->setFloat("material[0].shininess", shininess);
    shader->setFloat("material[0].normalScale", normalScale);

  }
  void Unbind(){
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, 0);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, 0);
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, 0);
    glActiveTexture(GL_TEXTURE3);
    glBindTexture(GL_TEXTURE_2D, 0);
    glActiveTexture(GL_TEXTURE4);
    glBindTexture(GL_TEXTURE_2D, 0);
    glActiveTexture(GL_TEXTURE0);
  }

  void Activate(){
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, diffuseTexture->id);

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, normalTexture->id);

    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, specularTexture->id);

    glActiveTexture(GL_TEXTURE3);
    glBindTexture(GL_TEXTURE_2D, alphaTexture->id);

    glActiveTexture(GL_TEXTURE4);
    glBindTexture(GL_TEXTURE_2D, emissiveTexture->id);
  }
};
