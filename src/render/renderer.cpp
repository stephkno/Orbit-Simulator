#include <renderer.h>
#include <settings.h>
#include <camera.h>
#include <framebuffer.h>

Renderer * Renderer::renderer = nullptr;


GLfloat Renderer::SampleTexture(GLuint * texture, int size, int p){
  // sample scene for ambient light value
  glBindTexture(GL_TEXTURE_2D, *texture);
  GLfloat* pixels = new GLfloat[size];
  glGetTexImage( GL_TEXTURE_2D, 0, GL_RED, GL_FLOAT, pixels);
  return pixels[p];
}

void Renderer::Render(Camera * camera){

  float maxPos = 10.0f;
  float width = Settings::GetWidth();
  float height = Settings::GetHeight();
  ImGuizmo::SetRect(0,0,width,height);

  //uint shadowmapTexture = sun.Render(this);
  DeferredRenderPass(camera, Settings::GetWidth(), Settings::GetHeight(), 0.0f);
  //ForwardRenderPass(camera, 0.0f,0.0f, width, height, 0.0f);
  // render orthographic viewport right view
  //RenderViewport(0,0,width/2.0f,height/2.0f,glm::vec3(1,0,0));
  //RenderViewport(0,width/2.0f,width/2.0f,height/2.0f,glm::vec3(0,0,1));
  //RenderViewport(width/2.0f,0,width/2.0f,height/2.0f,glm::vec3(0,1,0));

  FinishRender();
}

void Renderer::DeferredRenderPass(Camera * camera, int width, int height, float deltaTime){

  // 1. geometry pass: render scene's geometry/color data into gbuffer
  glBindFramebuffer(GL_FRAMEBUFFER, renderer->gBuffer);
  glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glViewport(0, 0, Settings::GetWidth(), Settings::GetHeight());

  int i=0;
  int light=0;

  // begin world render pass
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
  glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glViewport(0, 0, Settings::GetWidth()*Settings::GetScale(), Settings::GetHeight()*Settings::GetScale());

  //RenderLines(camera);
  //renderer->skybox.Render();

  // set universal shader uniforms
  renderer->PostLightingShader.use();
  /*
  for(Entity * object : scene->children){
    if(object->light){
      SetLightUniforms(light++, object);
    }
  }
  */
  //renderer->sun.SetSunUniforms(&renderer->PostLightingShader);
  //renderer->PostLightingShader.setFloat("t", time);
  //renderer->PostLightingShader.setFloat("specular_intensity", spec_i);
  //renderer->PostLightingShader.setVec3("brightness", brightness);
  //renderer->PostLightingShader.setVec3("ambient", ambient);
  renderer->PostLightingShader.setBool("lit", renderer->renderMode==GL_FILL);
  renderer->PostLightingShader.setVec3("viewPos", Settings::GetMainCamera()->Position);

  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, renderer->gPosition);
  glActiveTexture(GL_TEXTURE1);
  glBindTexture(GL_TEXTURE_2D, renderer->gNormal);
  glActiveTexture(GL_TEXTURE2);
  glBindTexture(GL_TEXTURE_2D, renderer->gAlbedoSpec);
  glActiveTexture(GL_TEXTURE3);
  glBindTexture(GL_TEXTURE_2D, renderer->gSunLightMap);

  //time += deltaTime;

  //viewport = glm::vec2(wsize.x, wsize.y);
  // Render dear imgui into screen
  glViewport(0, 0, Settings::GetWidth()*Settings::GetScale(), Settings::GetHeight()*Settings::GetScale());
  renderQuad();

}

void Renderer::ForwardRenderPass(Camera * camera, float x, float y, float width, float height, float deltaTime){

  glFrontFace(GL_CW);
  // begin world render pass
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
  glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
  glPolygonMode(GL_FRONT, GL_FILL);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  //glViewport(0, 0, Settings::GetWidth()*Settings::GetScale()/2.0f, Settings::GetHeight()*Settings::GetScale()/2.0f);
  glViewport(x,y,width,height);

  int i=0;
  int light=0;

  //RenderLines(camera);

  ImGuiLayer::EndFrame();
}


void Renderer::renderQuad()
{
    if (renderer->quadVAO == 0)
    {
        float quadVertices[] = {
            // positions        // texture Coords
            -1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
            -1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
             1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
             1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
        };
        // setup plane VAO
        glGenVertexArrays(1, &renderer->quadVAO);
        glGenBuffers(1, &renderer->quadVBO);
        glBindVertexArray(renderer->quadVAO);
        glBindBuffer(GL_ARRAY_BUFFER, renderer->quadVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    }

    glBindVertexArray(renderer->quadVAO);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glBindVertexArray(0);

}

void Renderer::FinishRender(){
  ImGui::Render();
  ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
  //ImGuiLayer::EndFrame();
}
/*
void Renderer::SetLightUniforms(Light * light){
  //uint sm = lights[i]->CalculateShadowMap(this);
  renderer->PostLightingShader.setVec3("lights[" + std::to_string(light) + "].Position", light->position);
  renderer->PostLightingShader.setVec3("lights[" + std::to_string(light) + "].Color", light->lightColor);
  renderer->PostLightingShader.setFloat("lights[" + std::to_string(light) + "].Linear", light->lightLinear);//scene->lights[i]->linear);
  renderer->PostLightingShader.setFloat("lights[" + std::to_string(light) + "].Quadratic", light->lightQuadratic);//scene->lights[i]->quadratic);
  renderer->PostLightingShader.setFloat("lights[" + std::to_string(light) + "].Brightness", light->lightBrightness);//scene->lights[i]->quadratic);
}
*/
void Renderer::RenderLines(Camera * camera){

  std::vector<Vertex> lineVerts;
  std::vector<uint> lines;
  std::vector<Vertex> pointVerts;
  uint i = 0;

  for(Line& line : renderer->lines){
    uint i = lineVerts.size();

    lineVerts.push_back(Vertex(line.pointa, glm::vec3(0.0f), line.colorb));
    lineVerts.push_back(Vertex(line.pointb, glm::vec3(0.0f), line.colorb));

    lines.push_back(i);
    lines.push_back(i+1);
  }

  for(Point& p : renderer->points){
    pointVerts.push_back(Vertex(p.point, glm::vec3(0.0f), p.color));
  }

  renderer->lineShader.use();
  renderer->lineShader.setBool("wireframe", true);
  renderer->lineShader.setMat4("view", camera->ViewMatrix());
  renderer->lineShader.setMat4("projection", camera->ProjectionMatrix());
  renderer->lineShader.setMat4("model", glm::mat4(1.0f));

  Mesh m;
  m.vertices=lineVerts;
  m.indices=lines;
  m.Refresh();
  m.LineDraw(&renderer->lineShader, new Transform());

  Mesh pm;
  pm.vertices=pointVerts;;
  pm.Refresh();
  pm.PointDraw(&renderer->lineShader, new Transform());

  renderer->points.clear();
  renderer->lines.clear();

}
void Renderer::SetRenderMode(GLuint rm){
  renderer->renderMode = rm;
}
GLuint Renderer::GetRenderMode(){
  return renderer->renderMode;
}

void Renderer::InitImgui(GLFWwindow * window){
  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  //setimguistyle(highdpiscalefactor);
  ImGui_ImplGlfw_InitForOpenGL(window, true);
  std::string glsl_version = "#version 330";
  ImGui_ImplOpenGL3_Init(glsl_version.c_str());
  //ImGui::StyleColorsDark();
}

Renderer::Renderer(GLFWwindow * _window){
  window = _window;
  //sun = new Sun();
  // build and compile shaders
  PostLightingShader.Load("shaders/lightingShader.vs", "shaders/lightingShader.fs");
  forwardshader.Load("shaders/default.vs", "shaders/default.fs");

  // configure g-buffer framebuffer
  glGenFramebuffers(1, &gBuffer);
  glBindFramebuffer(GL_FRAMEBUFFER, gBuffer);
  // position color buffer
  glGenTextures(1, &gPosition);
  glBindTexture(GL_TEXTURE_2D, gPosition);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, Settings::GetWidth(), Settings::GetHeight(), 0, GL_RGBA, GL_FLOAT, NULL);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, gPosition, 0);
  // normal color buffer
  glGenTextures(1, &gNormal);
  glBindTexture(GL_TEXTURE_2D, gNormal);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, Settings::GetWidth(), Settings::GetHeight(), 0, GL_RGBA, GL_FLOAT, NULL);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, gNormal, 0);
  // color + specular color buffer
  glGenTextures(1, &gAlbedoSpec);
  glBindTexture(GL_TEXTURE_2D, gAlbedoSpec);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, Settings::GetWidth(), Settings::GetHeight(), 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, gAlbedoSpec, 0);
  // tell OpenGL which color attachments we'll use (of this framebuffer) for rendering

  glGenTextures(1, &gSunLightMap);
  glBindTexture(GL_TEXTURE_2D, gSunLightMap);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, Settings::GetWidth(), Settings::GetHeight(), 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT3, GL_TEXTURE_2D, gSunLightMap, 0);
  // tell OpenGL which color attachments we'll use (of this framebuffer) for rendering
  glGenTextures(1, &gPickerMap);
  glBindTexture(GL_TEXTURE_2D, gPickerMap);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, Settings::GetWidth(), Settings::GetHeight(), 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT4, GL_TEXTURE_2D, gPickerMap, 0);

  glDrawBuffers(4, attachments);

  // create and attach depth buffer (renderbuffer)
  glGenRenderbuffers(1, &rboDepth);
  glBindRenderbuffer(GL_RENDERBUFFER, rboDepth);
  glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, Settings::GetWidth(), Settings::GetHeight());
  glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rboDepth);
  // finally check if framebuffer is complete
  if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE){
    Debug::msg("Framebuffer not complete!");
  }
  glBindFramebuffer(GL_FRAMEBUFFER, 0);

  // set shader configuration
  PostLightingShader.Load("shaders/lightingShader.vs", "shaders/lightingShader.fs");
  PostLightingShader.use();
  PostLightingShader.setInt("gPosition", 0);
  PostLightingShader.setInt("gNormal", 1);
  PostLightingShader.setInt("gAlbedoSpec", 2);
  PostLightingShader.setInt("gSunLightMap", 3);
  PostLightingShader.setInt("gPickerMap", 4);
  PostLightingShader.setInt("gSkyboxMap", 5);
}

GLFWwindow * Renderer::Init(){

  // glfw: initialize and configure
  // ------------------------------
  glfwInit();
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  #ifdef __APPLE__
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
  #endif

  // default window size
  int w = 1920;
  int h = 1028;
  int scalefactor = 1.0f;

  const GLFWvidmode * mode = glfwGetVideoMode(glfwGetPrimaryMonitor());

  // glfw window creation
  // --------------------
  GLFWwindow * window = glfwCreateWindow(w, h, "OpenGL", NULL, NULL);//glfwGetPrimaryMonitor(), NULL);

  Debug::msg("Screen size: ", w, " ", h);

  Settings::SetWidth(w);
  Settings::SetHeight(h);

  if (window == NULL)
  {
    Debug::msg("Failed to create GLFW window");
    glfwTerminate();
    return nullptr;
  }
  glfwMakeContextCurrent(window);

  /*
  if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
  {
    std::cout << "Failed to initialize GLAD" << std::endl;
    return nullptr;
  }
  */

  GLenum err = glewInit();
  if (GLEW_OK != err)
  {
   /* Problem: glewInit failed, something is seriously wrong. */
   Debug::msg("Error: ", glewGetErrorString(err));
  }

  // tell GLFW to capture our mouse
  glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

  //LOG_F(1, "Initializing GLEW");
  // syncronize screen updates with vsync
  // 1 = vsync on
  // 0 = vsync off
  int vsync = 0;
  //LOG_F(1, "Vsync: %s", (vsync == 1 ? "Enabled" : "Disabled"));
  glfwSwapInterval( vsync );
  glEnable(GL_MULTISAMPLE);

  // (for source take srcA, when rendering ontop of this, 1-srcA)
  // transparency enabled for PNG
  glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );

  // set openGL viewport position and scale
  // set framebuffer window resize function
  //LOG_F(1, "OpenGL v%s", glGetString( GL_VERSION ));
  bool failed = false;

  // tell stb_image.h to flip loaded texture's on the y-axis (before loading model).
  stbi_set_flip_vertically_on_load(true);

  // configure global opengl state
  glfwSwapInterval( vsync );
  glEnable(GL_MULTISAMPLE);

  // (for source take srcA, when rendering ontop of this, 1-srcA)
  // transparency enabled for PNG
  // gl parameters
  glEnable(GL_CULL_FACE);
  glEnable(GL_DEPTH_TEST);

  renderer = new Renderer(window);
  renderer->lineShader.Load("shaders/lineShader.vs", "shaders/lineShader.fs");
  renderer->cubeShader.Load("shaders/default.vs", "shaders/default.fs");

  // create buffers/arrays
  glGenVertexArrays(1, &renderer->lineVAO);
  glGenBuffers(1, &renderer->lineVBO);
  glGenBuffers(1, &renderer->lineEBO);

  return window;
}

void Renderer::renderCube(glm::vec3 position){

  if(renderer->cubeMesh == nullptr){
    renderer->cubeMesh = new Mesh();
    std::vector<Vertex> vertices = {
      Vertex(1.0f, 1.0f, -1.0f),
      Vertex(-1.0f, 1.0f, -1.0f),
      Vertex(-1.0f, 1.0f, 1.0f),
      Vertex(1.0f, 1.0f, 1.0f),

      Vertex(1.0f, -1.0f, -1.0f),
      Vertex(-1.0f, -1.0f, -1.0f),
      Vertex(-1.0f, -1.0f, 1.0f),
      Vertex(1.0f, -1.0f, 1.0f)
    };

    std::vector<uint> indices = {
      0,1,2,3,7,6,5,4,6,7,3,2,7,4,0,3,4,5,1,0,5,6,2,1
    };

    renderer->cubeMesh->vertices = vertices;
    renderer->cubeMesh->indices = indices;
    renderer->cubeMesh->Refresh();

  }

  Camera * cam = Settings::GetMainCamera();
  cam->SetUniforms(&renderer->cubeShader);
  renderer->forwardshader.setVec3("position", position);
  Transform * t = new Transform();
  t->position = position;
  renderer->cubeMesh->Draw(&renderer->cubeShader, t);
}
