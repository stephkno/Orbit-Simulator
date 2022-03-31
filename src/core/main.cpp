#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <shader.h>
#include <renderer.h>
#include <game.h>
#include <settings.h>
#include <iostream>

// settings
float lastX = (float)0;
float lastY = (float)0;
bool firstMouse = true;

// timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;
Game game;
GLFWwindow * window;

void ImGuiInit(GLFWwindow * window){
  IMGUI_CHECKVERSION();

  ImGui::CreateContext();
  ImGui_ImplGlfw_InitForOpenGL(window, true);

  ImGuiIO& io = ImGui::GetIO();
  io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
  //    io.BackendFlags |= ImGuiBackendFlags_HasSetMousePos;

  std::string glsl_version = "#version 330";
  //ImGui::StyleColorsDark();
  // set up keycodes here
  ImGui_ImplOpenGL3_Init(glsl_version.c_str());
  //ImGui::StyleColorsDark();
}

int main(int argc, char ** argv)
{
  Settings::Init(window);
  GLFWwindow * window = Renderer::Init();
  game.Init(window);
  ImGuiInit(window);
  int i = 0;

  while (!glfwWindowShouldClose(window))
  {
    float currentFrame = glfwGetTime();
    deltaTime = currentFrame - lastFrame;
    lastFrame = currentFrame;

    IO::Step(window);
    if(IO::KeyDown(GLFW_KEY_ESCAPE)){
      glfwTerminate();
    }

    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
    Settings::NewFrame();

    game.Step(deltaTime);
    game.Render(deltaTime);
    ImGui::EndFrame();

    glfwPollEvents();

  }

  game.End();

  glfwTerminate();
  return 0;
}
