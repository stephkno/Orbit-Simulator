#include <settings.h>
#include <camera.h>

Settings * Settings::settings = nullptr;

Settings::Settings(){
}
Settings::~Settings(){
}
void Settings::Init(GLFWwindow * _window){
  if(!settings){
    Debug::msg("init settings");
    settings = new Settings();
  }
  settings->window = _window;
}
Settings * Settings::Get(){
  if(!settings){
    Init();
  }
  return settings;
}
GLFWwindow * Settings::GetWindow(){
  if(!settings){
    Get();
  }
  return settings->window;
}
void Settings::SetCamera(int cam){
  if(!settings){
    Get();
  }
  settings->mainCamera = cam;
}
void Settings::AddCamera(Camera * cam){
  if(!settings){
    Get();
  }
  if(settings->cameras.size()==0){
    settings->mainCamera=settings->cameras.size();
  }
  settings->cameras.push_back(cam);
}
Camera * Settings::GetMainCamera(){
  if(!settings){
    Get();
  }
  return settings->cameras.at(settings->mainCamera);
}
void Settings::NextCamera(){
  if(!settings){
    Get();
  }
  settings->mainCamera += 1;
}
Camera * Settings::GetCamera(int cam){
  if(!settings){
    Get();
  }
  return settings->cameras.at(cam);
}
uint Settings::NumCameras(){
  return settings->cameras.size();
}
/*
Terrain * Settings::GetTerrain(){
  if(!settings){
    Get();
  }
  return settings->terrain;
}
float Settings::GetTerrainHeight(glm::vec3 point){
  if(!settings){
    Get();
  }
  return settings->GetTerrain()->GetCurrentChunk()->GetHeight(point);
}
void Settings::SetTerrain(Terrain * t){
  if(!settings){
    Get();
  }
  settings->terrain = t;
}*/
uint Settings::GetWidth(){
  if(!settings){
    Get();
  }else{
    return settings->width;
  }
}
uint Settings::GetHeight(){
  if(!settings){
    Get();
  }else{
    return settings->height;
  }
}
int Settings::GetScale(){
  return settings->scale;
}
void Settings::SetWidth(uint _width){
  if(!settings){
    Get();
  }else{
    settings->width = _width;
  }
}
void Settings::SetHeight(uint _height){
  if(!settings){
    Get();
  }else{
    settings->height = _height;
  }
}
void Settings::SetCameraLocked(bool c){
  if(!settings){
    Get();
  }
  settings->cameraLocked = c;
}
bool Settings::CameraLocked(){
  if(!settings){
    Get();
  }
  return settings->cameraLocked;
}
glm::mat4 Settings::ProjectionMatrix(){
  if(!settings){
    Get();
  }
  return settings->cameras[settings->mainCamera]->ProjectionMatrix();
}
/*
glm::mat4 Settings::ProjectionMatrix(glm::vec2 viewport){
  if(!settings){
    Get();
  }
  return settings->cameras[settings->mainCamera]->ProjectionMatrix(viewport);
}
*/
glm::mat4 Settings::ViewMatrix(){

  if(!settings){
    Get();
  }
  return settings->cameras[settings->mainCamera]->ViewMatrix();
}
void Settings::NewFrame(){
  if(!settings){
    Get();
  }
  settings->frame++;
}
int Settings::CurrentFrame(){
  if(!settings){
    Get();
  }
  return settings->frame;
}
