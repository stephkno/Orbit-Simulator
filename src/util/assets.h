#pragma once
#include <vector>
#include <string>
#include <iostream>
#include <filesystem>
#include <map>
#include <render/material.h>
#include <render/mesh.h>
#include <imgui/imgui.h>

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <stb_image.h>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <ImGuiFileBrowser.h>

#include <boost/serialization/map.hpp>

#include <render/shader.h>
#include <fstream>
#include <sstream>

using namespace std;

// serialize assets:
// save list of model paths
// model class
class Model{
private:
  // Allow serialization to access non-public data members.
  friend class boost::serialization::access;

  // Serialize the std::vector member of Info
  template<class Archive>
  void serialize(Archive & ar, const unsigned int version)
  {
    ar & name & path & scale;

    if(Archive::is_loading::value) {

    }

  }
public:
  std::string name;
  std::string path;
  glm::vec3 scale;

  std::vector<Mesh> meshes;
  int n_meshes=0;

  Model(){
    meshes = std::vector<Mesh>();
  }
  Model(std::string n, std::vector<Mesh> m){
    meshes = m;
    name = n;
    n_meshes = meshes.size();
  }
  void AddMesh(Mesh m){
    meshes.push_back(m);
    n_meshes++;
  }
  void Add(Mesh m){
    meshes.push_back(m);
    meshes.back().CalculateNormals();
  }
  void AddQuad(Vertex a, Vertex b, Vertex c, Vertex d, float u, float v, glm::vec3 normal, Material * mat){

    std::vector<Vertex> verts;
    verts.push_back(Vertex(a.Position, normal, u, -v));
    verts.push_back(Vertex(b.Position, normal, -u, -v));
    verts.push_back(Vertex(c.Position, normal, -u, v));
    verts.push_back(Vertex(d.Position, normal, u, v));
    std::vector<uint> inds = {0,1,2,3,0,2};//a,b,c,d,a,c};
    Mesh m(verts, inds, mat);
    m.CalculateNormals();
    meshes.push_back(m);
  }
};

// import file struct
struct ImportFile{
  std::filesystem::path path;
  bool selected;
  ImportFile(std::filesystem::path p){
    path = p;
    selected = false;
  }
};

// static class for loading Models from files
// using assimp
class ModelLoader
{
public:
    // model data
    string directory;
    bool gammaCorrection;

    // constructor, expects a filepath to a 3D model.
    ModelLoader() : gammaCorrection(gamma)
    {

    }

    // loads a model with supported ASSIMP extensions from file and stores the resulting meshes in the meshes vector.
    static Model Load(string const &path)
    {
      Model model = Model();
      model.path = path;

      // read file via ASSIMP
      Assimp::Importer importer;
      const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);
      // check for errors
      if(!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) // if is Not Zero
      {
          cout << "ERROR::ASSIMP:: " << importer.GetErrorString() << Endl;
          assert(false);
      }
      // retrieve the directory path of the filepath
      string directory = path.substr(0, path.find_last_of('/'));

      // process ASSIMP's root node recursively
      processNode(scene->mRootNode, directory, scene, &model.meshes);

      return model;
    }

    // processes a node in a recursive fashion. Processes each individual mesh located at the node and repeats this process on its children nodes (if any).
    static void processNode(aiNode *node, string directory, const aiScene *scene, std::vector<Mesh> * meshes)
    {
      // process each mesh located at the current node
      for(unsigned int i = 0; i < node->mNumMeshes; i++)
      {
          // the node object only contains indices to index the actual objects in the scene.
          // the scene contains all the data, node is just to keep stuff organized (like relations between nodes).
          aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
          Mesh m = processMesh(mesh, directory, scene);
          m.CalculateNormals();
          meshes->push_back(m);
      }
      // after we've processed all of the meshes (if any) we then recursively process each of the children nodes
      for(unsigned int i = 0; i < node->mNumChildren; i++)
      {
        // process ASSIMP's root node recursively
        processNode(node->mChildren[i], directory, scene, meshes);
      }
    }

    static Mesh processMesh(aiMesh *mesh, string directory, const aiScene *scene)
    {
        // data to fill
        vector<Vertex> vertices;
        vector<unsigned int> indices;
        vector<Texture> textures;

        // walk through each of the mesh's vertices
        for(unsigned int i = 0; i < mesh->mNumVertices; i++)
        {
            Vertex vertex;
            glm::vec3 vector; // we declare a placeholder vector since assimp uses its own vector class that doesn't directly convert to glm's vec3 class so we transfer the data to this placeholder glm::vec3 first.
            // positions
            vector.x = mesh->mVertices[i].x;
            vector.y = mesh->mVertices[i].y;
            vector.z = mesh->mVertices[i].z;
            vertex.Position = vector;
            // normals
            if (mesh->HasNormals())
            {
                vector.x = mesh->mNormals[i].x;
                vector.y = mesh->mNormals[i].y;
                vector.z = mesh->mNormals[i].z;
                vertex.Normal = vector;
            }
            // texture coordinates
            if(mesh->mTextureCoords[0]) // does the mesh contain texture coordinates?
            {
                glm::vec2 vec;
                // a vertex can contain up to 8 different texture coordinates. We thus make the assumption that we won't
                // use models where a vertex can have multiple texture coordinates so we always take the first set (0).
                vec.x = mesh->mTextureCoords[0][i].x;
                vec.y = mesh->mTextureCoords[0][i].y;
                vertex.TexCoords = vec;
                // tangent
                vector.x = mesh->mTangents[i].x;
                vector.y = mesh->mTangents[i].y;
                vector.z = mesh->mTangents[i].z;
                vertex.Tangent = vector;
                // bitangent
                vector.x = mesh->mBitangents[i].x;
                vector.y = mesh->mBitangents[i].y;
                vector.z = mesh->mBitangents[i].z;
                vertex.Bitangent = vector;
            }
            else
                vertex.TexCoords = glm::vec2(0.0f, 0.0f);

            vertices.push_back(vertex);
        }
        // now wak through each of the mesh's faces (a face is a mesh its triangle) and retrieve the corresponding vertex indices.
        for(unsigned int i = 0; i < mesh->mNumFaces; i++)
        {
            aiFace face = mesh->mFaces[i];
            // retrieve all indices of the face and store them in the indices vector
            for(unsigned int j = 0; j < face.mNumIndices; j++)
                indices.push_back(face.mIndices[j]);
        }
        // process materials
        aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
        // we assume a convention for sampler names in the shaders. Each diffuse texture should be named
        // as 'texture_diffuseN' where N is a sequential number ranging from 1 to MAX_SAMPLER_NUMBER.
        // Same applies to other texture as the following list summarizes:
        // diffuse: texture_diffuseN
        // specular: texture_specularN
        // normal: texture_normalN

        Material * mat;
        // 1. diffuse maps
        mat->diffuseTexture = loadMaterialTexture(material, directory, aiTextureType_DIFFUSE, "texture_diffuse");
        // 2. specular maps
        mat->specularTexture = loadMaterialTexture(material, directory, aiTextureType_SPECULAR, "texture_specular");
        // 3. normal maps
        mat->normalTexture = loadMaterialTexture(material, directory, aiTextureType_HEIGHT, "texture_normal");
        // 4. height maps
        //mat->metalness.push_back(loadMaterialTexture(material, directory, aiTextureType_METALNESS, "texture_metalness"));

        //mat->heightmap.push_back(loadMaterialTexture(material, directory, aiTextureType_AMBIENT, "texture_height"));
        //mat->emissive.push_back(loadMaterialTexture(material, directory, aiTextureType_EMISSIVE, "texture_emissive"));

        // return a mesh object created from the extracted mesh data
        return Mesh(vertices, indices, mat);
    }

    // checks all material textures of a given type and loads the textures if they're not loaded yet.
    // the required info is returned as a Texture struct.
    static Texture * loadMaterialTexture(aiMaterial *mat, string directory, aiTextureType type, string typeName)
    {
      Texture * texture = new Texture();
      aiString str;
      mat->GetTexture(type, 0, &str);
      texture->id = File::TextureFromFile(directory, str.C_Str());
      texture->type = typeName;
      return texture;
    }
};

class AssetManager{
public:

  friend class boost::serialization::access;

  // assets for a game
  std::string name = "";
  std::string path = "";

  // assets library
  //
  std::vector<std::string> texture_names;
  std::vector<std::string> material_names;
  std::vector<std::string> model_names;

  std::map<std::string, Texture> textures;
  std::map<std::string, Material> materials;
  std::map<std::string, Model> models;

  std::vector<ImportFile> importfiles;
  std::vector<ImportFile> importfilesToImport;
  std::string assetfile;

  int nTextures;
  int nMaterials;
  int nModels;

  int currentModel=-1;
  int currentMaterial=-1;
  Texture ** selectionTexture;

  bool materialWindow = false;
  bool textureWindow = false;

  static AssetManager * assman;
  // Serialize the std::vector member of Info
  template<class Archive>
  void serialize(Archive & ar, const unsigned int version)
  {
    // load texture names
    if(!Archive::is_loading::value){
      nTextures = texture_names.size();
    }
    ar & nTextures;
    if(!Archive::is_loading::value){
      for(int i = 0; i < nTextures; i++){
        ar & texture_names[i];
      }
    }else{
      for(int i = 0; i < nTextures; i++){
        std::string texname;
        ar & texname;
        texture_names.push_back(texname);
      }
    }

    ar & textures;

    // load material names
    if(!Archive::is_loading::value){
      nMaterials = material_names.size();
    }
    ar & nMaterials;
    if(!Archive::is_loading::value){
      for(int i = 0; i < nMaterials; i++){
        ar & material_names[i];
      }
    }else{
      for(int i = 0; i < nMaterials; i++){
        std::string name;
        ar & name;
        material_names.push_back(name);
      }
    }

    ar & materials;

    // load model names
    if(!Archive::is_loading::value){
      nModels = model_names.size();
    }
    ar & nModels;
    if(!Archive::is_loading::value){
      for(int i = 0; i < nModels; i++){
        ar & model_names[i];
      }
    }else{
      for(int i = 0; i < nModels; i++){
        std::string name;
        ar & name;
        model_names.push_back(name);
        Print << "Loading model: " << name << Endl;
        models[name] = ModelLoader::Load(name);
      }
    }
  }

  AssetManager(){
    assman = this;
  }

  // int id = hashlist.push_back(model.name, model);
  // model = hashlist.getItem(int id);
  // model = hashlist.getItem(string name);
  //
  // when deleting an object the int id remains
  // hashlist.erase(int id);
  // hashlist.erase(string name)
  //
  // next object added takes first free id
  //

  void ShowGUI(){

    ImGui::Begin("Assets");
    int selectModel = -1;
    bool b;

    ImGui::Text("Models");
    b = (currentModel == -1);
    if(ImGui::Selectable("None", &b)){
      currentModel = -1;
    }

    for(int i = 0; i < models.size(); i++){
      b = currentModel==i;
      if(ImGui::Selectable(models[model_names[i]].name.c_str(), &b)){
        currentModel = i;
      }
    }

    ImGui::Text("Materials");

    // current material
    if(assman->materials.size()>0){
      int id = 0;
      if(currentMaterial > -1){
        Material cm = assman->materials[assman->material_names[currentMaterial]];
        if(cm.diffuseTexture!=nullptr){
          id = cm.diffuseTexture->id;
        }
      }
      if(ImGui::ImageButton((void*)id, ImVec2(100,100))){
        materialWindow = true;
      }
    }else{
      if(ImGui::ImageButton((void*)0, ImVec2(100,100))){
        materialWindow = true;
      }
    }


    ImGui::End();

/*
    b = importfiles.size()>0;

    // when there are files to import
    if(importfiles.size()>0){

      ImGui::OpenPopup("Modal window");
      if (ImGui::BeginPopupModal("Modal window", &b))
      {
        const int n = importfiles.size();
        bool selected[n];

        if (ImGui::Button("Close")){
          ImGui::CloseCurrentPopup();
          importfiles.clear();
        }
        if (ImGui::Button("OK")){
          importfiles.clear();

          // import selected files
          for(ImportFile file : importfilesToImport){
            ManageImport(file);
          }
          importfilesToImport.clear();
          ImGui::CloseCurrentPopup();

        }
        ImGui::Text("Files");

        int i=0;
        for(ImportFile file : importfiles){
          if(ImGui::Selectable(file.path.filename().c_str(), &file.selected, ImGuiSelectableFlags_DontClosePopups)){
            importfiles.erase(importfiles.begin()+i);
            importfilesToImport.push_back(file);
          }
          i++;
        }
        ImGui::Text("Files to Import");
        i=0;
        for(ImportFile file : importfilesToImport){
          if(ImGui::Selectable(file.path.filename().c_str(), &file.selected, ImGuiSelectableFlags_DontClosePopups)){
            importfilesToImport.erase(importfiles.begin()+i);
            importfiles.push_back(file);
          }
          i++;
        }

        ImGui::EndPopup();
      }
    }
    */


    // material window
    if(materialWindow){
      ImGui::Begin("Materials");
      ImGui::Text("Materials");

      for(int i = 0; i < material_names.size(); i++){

        b = (currentMaterial == i);

        if(ImGui::Selectable(materials[material_names[i]].name.c_str(), &b)){
          currentMaterial = i;
          //materialWindow = false;
        }
      }

      if(currentMaterial > -1){
        uint id=0;
        if(materials[material_names[currentMaterial]].diffuseTexture != nullptr){
          //Print << materials[material_names[currentMaterial]].diffuseTexture->id << Endl;
          id = materials[material_names[currentMaterial]].diffuseTexture->id;
        }

        if(ImGui::ImageButton((void*)id, ImVec2(100,100))){
          //OpenTextureWindow(materials[material_names[currentMaterial]].diffuseTexture);
          selectionTexture = &materials[material_names[currentMaterial]].diffuseTexture;
          textureWindow = true;
        }
        id=0;
        if(materials[material_names[currentMaterial]].normalTexture != nullptr){
          id = materials[material_names[currentMaterial]].normalTexture->id;
        }

        if(ImGui::ImageButton((void*)id, ImVec2(100,100))){
          //OpenTextureWindow(materials[material_names[currentMaterial]].normalTexture);
          selectionTexture = &materials[material_names[currentMaterial]].normalTexture;
          textureWindow = true;
        }
      }
      if(ImGui::Button("+")){
        std::string name = "Material " + std::to_string(material_names.size());
        material_names.push_back(name);
        materials[name] = Material(name);
        currentMaterial = material_names.size()-1;
      }
      if(ImGui::Button("Cancel")){
        materialWindow = false;
      }
      ImGui::End();
    }
    if(textureWindow){
      ImGui::Begin("Textures");
      ImGui::Text("Textures");

      for(int i = 0; i < texture_names.size(); i++){
        ImGui::Text(texture_names[i].c_str());
        if(ImGui::ImageButton((void*)textures[texture_names[i]].id, ImVec2(100,100))){
          //OpenTextureWindow(materials[material_names[currentMaterial]].diffuseTexture);
          *selectionTexture = &textures[texture_names[i]];
          textureWindow = false;
        }
      }
      if(ImGui::Button("Cancel")){
        textureWindow = false;
      }
      ImGui::End();
    }
  }


  static void ImportFiles(std::vector<ImportFile> filenames){
    //assman->importfiles = filenames;
    int f = filenames.size();
    int i = 0;

    for(ImportFile file : filenames){
      i++;
      Print << i << "/" << f << Endl;

      if(std::filesystem::is_directory(file.path)){
        //
      }
      else if(file.path.extension()==".dae" || file.path.extension()==".fbx" || file.path.extension()==".obj"){
        //ImportModel(file);
      }
      else if(file.path.extension()==".jpg" || file.path.extension()==".png" ||  file.path.extension()==".bmp"){
        ImportTexture(file);
      }
    }
  }

  void ManageImport(ImportFile file){

    if(std::filesystem::is_directory(file.path)){
      // import selected files
    }

    if(file.path.extension()==""){
      return;
    }
    if(file.path.extension()==".dae" || file.path.extension()==".fbx" || file.path.extension()==".obj"){
      ImportModel(file);
      return;
    }
    if(file.path.extension()==".jpg" || file.path.extension()==".png" ||  file.path.extension()==".bmp"){
      ImportTexture(file);
    }
  }

  static void ImportTexture(ImportFile file){
    if(assman->textures.contains(file.path)){
      return;
    }
    assman->texture_names.push_back(file.path);
    Texture tex(file.path);
    assman->textures[file.path] = tex;
  }

  static int GetCurrentMaterial(){
    return assman->currentMaterial;
  }

  static Material * GetMaterial(int id){
    Print << "assman get material " << id << ". nMaterials: " << assman->materials.size() << Endl;
    if(id==-1){
      return nullptr;
    }
    if(id > assman->materials.size()){
      Print << "no material" << Endl;
      return nullptr;
    }
    return &assman->materials[assman->material_names[id]];
  }
  static Material * GetMaterial(std::string name){
    return &assman->materials[name];
  }
  static int GetCurrentModel(){
    return assman->currentModel;
  }
  static Model GetModel(int id){
    Print << "assman get model " << id << ". Nmodels: " << assman->models.size() << Endl;
    if(id==-1){
      return Model();
    }
    if(id > assman->models.size()){
      Print << "no model" << Endl;
      return Model();
    }
    Print << assman << " " << id << " nmeshes: " << assman->models[assman->model_names[id]].meshes.size() << Endl;
    return assman->models[assman->model_names[id]];
  }
  static void ImportModel(ImportFile file);
};

/*
  static void ShowGUI(){
      ImGui::Begin("Materials");
      for(std::string matname : matman->material_names){
      //if (ImGui::ImageButton(my_tex_id, size, uv0, uv1, frame_padding, bg_col, tint_col))
        ImVec4 bg_col = ImVec4(0.0f, 0.0f, 0.0f, 1.0f);         // Black background
        ImVec4 tint_col = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);       // No tint

        glBindTexture(GL_TEXTURE_2D, matman->materials[matname].diffuseTexture->id);
        if(ImGui::ImageButton((void*)matman->materials[matname].diffuseTexture->id, ImVec2(128,128))){
          matman->currentMaterial = &matman->materials[matname];
        }
      }
      ImGui::End();
  }
  */

  /*
  static void Init(){
    for(auto& p : std::filesystem::recursive_directory_iterator("assets/textures/surface")){
      if (!p.is_directory()){
        if(!(p.path().extension()==".jpg")&&!(p.path().extension()==".png")){
          continue;
        }
        Material m = *GetMaterial(p.path().parent_path());
        matman->materials[p.path().parent_path()]=m;
      }
    }
    matman->currentMaterial = &matman->materials[matman->material_names[0]];
  }
};
*/
