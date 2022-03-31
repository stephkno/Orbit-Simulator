#pragma once
#include <GL/glew.h>
#include <render/shader.h>
#include <renderer.h>

class Settings;
class Framebuffer{
public:
  Framebuffer();
  ~Framebuffer();
  void Activate();
  void Deactivate();

  void Render();
  void Render(int x, int y, int w, int h);
  Texture tex;
  unsigned int framebuffer;
  unsigned int rbo;
  unsigned int quadVAO, quadVBO;
  Shader * screenShader;

};
