#pragma once
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <stb_image/stb_image.h>
#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_opengl3.h>
#include <vector>
#include <render/shader.h>
#include <util/ImGuiLayer.h>
#include <util/IO.h>
#include <render/mesh.h>
#include <render/texture.h>
#include <vertex.h>
#include <render/framebuffer.h>
#include <transform.h>
#include <boost/serialization/vector.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/xml_oarchive.hpp>
#include <boost/archive/xml_iarchive.hpp>
#include <glm/serialize.hpp>

class World;
class Settings;

#define GLEW_STATIC
#define MAX_LIGHTS 5

#define ASSERT(x) if (!(x)) asm("int $3")
#define glCall(x) clearError(#x, __FILE__, __LINE__);\
 x;\
 ASSERT(logCall(#x, __FILE__, __LINE__))


// error handling functions
static void clearError(const char* function, const char* file, int line)
{
  GLenum  error;
  while ( ( error = glGetError() != GL_NO_ERROR ) );
}

static bool logCall(const char* function, const char* file, int line)
{
  GLenum error;
  while ( (error = glGetError() ) ){
    // display GL error messages
    std::cerr << error << " with " << function << " in " << file << " @ (or near) < line " << line << ">::" << glewGetErrorString(error);

    return false;
  }
  return true;
}

struct Line{
    glm::vec3 pointa;
    glm::vec3 pointb;
    glm::vec3 colora;
    glm::vec3 colorb;
    Line(glm::vec3 a, glm::vec3 b, glm::vec3 ca, glm::vec3 cb){
      pointa = a;
      pointb = b;
      colora = ca;
      colorb = cb;
    }
};
struct Point{
    glm::vec3 point;
    glm::vec3 color;
    Point(glm::vec3 p, glm::vec3 c){
      point = p;
      color = c;
    }
};
enum CameraMode{
  PERSPECTIVE,
  FRONTVIEW,
  RIGHTVIEW,
  TOPVIEW
};
enum RenderMode{
  LIT,
  FULLBRIGHT,
  WIREFRAME,
  DEPTH,
  NORMAL
};


class Settings;
class Entity;
class Model;

class Renderer{


public:
  static Renderer * renderer;

  uint quadVAO = 0;
  uint quadVBO = 0;
  uint cubeVAO = 0;
  uint cubeVBO = 0;
  uint lineVBO = 0;
  uint lineVAO = 0;
  uint lineEBO = 0;

  uint gBuffer, gSkyboxBuffer;
  uint gPosition, gNormal, gAlbedoSpec, gPickerMap, gSkyboxMap;
  uint gSunLightMap;
  uint depthrenderbuffer;

  uint attachments[5] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3, GL_COLOR_ATTACHMENT4 };
  uint rboDepth;
  const uint NR_LIGHTS = 1;

  Shader PostLightingShader;
  float time;

  uint depthCubeMap;
  GLFWwindow * window;
  GLuint renderMode=GL_FILL;
  Shader lineShader;
  Shader forwardshader;
  Shader cubeShader;
  Mesh * cubeMesh=nullptr;

  Framebuffer fb;

  std::vector<Line> lines;
  std::vector<Point> points;

  Renderer(GLFWwindow * window);
  static GLFWwindow * Init();

  // brush
  static void RenderViewport(float x, float y, float width, float height, glm::vec3 direction);
  static void Render(Camera * camera);
  static void DeferredRenderPass(Camera * camera, int width, int height, float deltaTime);
  static void ForwardRenderPass(Camera * camera, float x, float y, float width, float height, float deltaTime);

  GLfloat SampleTexture(GLuint* texture, int size, int p);
  void CreateBuffer(GLuint* buffer, GLuint* texture, int w, int h);

  static void SetRenderMode(GLuint rm);
  static GLuint GetRenderMode();

  static Texture RenderSceneToTexture(Entity * scene, Camera * camera, int width, int height, float deltaTime);

  static void InitImgui(GLFWwindow * window);

  static void FinishRender();
  static void renderQuad();

  static void DrawLine(glm::vec3 a, glm::vec3 b, glm::vec3 colora=glm::vec3(1.0f), glm::vec3 colorb=glm::vec3(1.0f)){
    renderer->lines.push_back(Line(a,b,colora,colorb));
  }
  static void DrawPoint(glm::vec3 p, glm::vec3 color=glm::vec3(1.0f)){
    renderer->points.push_back(Point(p, color));
  }
  static void RenderLines(Camera * camera);
  static void renderCube(glm::vec3 position);

  static void DrawGrid(){
  }
};
