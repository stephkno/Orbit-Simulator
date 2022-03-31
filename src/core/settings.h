#pragma once
#include <renderer.h>
#include <glm/glm.hpp>

class Camera;
//class Terrain;

class Settings{
  static Settings * settings;

private:
  uint width, height;
  GLFWwindow * window;
  int mainCamera;
  std::vector<Camera*> cameras;
  bool cameraLocked;
  int frame;
  // viewport scale factor for retina display
  float scale = 1.0f;
//  Terrain * terrain;

public:
  Settings();
  ~Settings();
  static void Init(GLFWwindow * _window=nullptr);
  static Settings * Get();
  static GLFWwindow * GetWindow();
  static void SetCamera(int cam);
  static void AddCamera(Camera * cam);
  static void SetMainCamera(Camera * cam);
  static void NextCamera();
  static Camera * GetCamera(int cam);
  static uint NumCameras();
  static Camera * GetMainCamera();
  static uint GetWidth();
  static uint GetHeight();
  static int GetScale();
  static void SetWidth(uint _width);
  static void SetHeight(uint _height);
  static void SetCameraLocked(bool c);
  static bool CameraLocked();
//  static Terrain * GetTerrain();
//  static float GetTerrainHeight(glm::vec3 point);
//  static void SetTerrain(Terrain* t);
  static glm::mat4 ProjectionMatrix();
  static glm::mat4 ProjectionMatrix(glm::vec2 viewport);
  static glm::mat4 ViewMatrix();
  static void NewFrame();
  static int CurrentFrame();

};
