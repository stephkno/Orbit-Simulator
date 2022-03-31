#pragma once
#include <render/shader.h>
#include <settings.h>
#include <util/IO.h>
#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_opengl3.h>
#include <cmath>
#include <util/bullet_physics.h>
#include <util/ImGuiLayer.h>
#include <util/Debug.h>
#include <util/assets.h>
#include <nlohmann/json.hpp>
#include <iostream>
#include <ImGuiFileBrowser.h>
#include <render/free_cam.h>
#include <cmath>

#define TWO_PI 6.28318530718

class Planet{
public:
  Mesh mesh;
  Transform transform;
  Shader shader;
  float radius=4.0f;
  bool is_static = false;
  float mass = 5.0f;
  float max = 100.0f;
  glm::vec3 velocity = glm::vec3(0.0f);
  glm::vec3 color = glm::vec3(1.0f);

  Planet(float x, float y, float m, glm::vec3 c){
    mass = m;
    transform.position.z = x;
    transform.position.y = y;
    color = c;
    GetRadius();
    BuildMesh();
  }
  Planet(float x, float y, float m){
    mass = m;
    transform.position.z = x;
    transform.position.y = y;
    GetRadius();
    BuildMesh();
  }
  Planet(float x, float y){
    radius = MassToRadius(mass);
    transform.position.z = x;
    transform.position.y = y;
    GetRadius();
    BuildMesh();
  }
  Planet(Planet * p){
    mesh = p->mesh;
    is_static = p->is_static;
    mass = p->mass;
    velocity = p->velocity;
    transform.position = p->transform.position;
    GetRadius();
    BuildMesh();
  }
  ~Planet(){

  }
  float MassToRadius(float mass){
    return pow((3.0f * mass) / (4.0f * 3.14159265f * 0.1f), (1.0f / 3.0f));
  }
  float RadiusToMass(float rad){
    return (0.4f*3.14159265f*rad*rad*rad)/3.0f;
  }
  void BuildMesh(){
    shader.Load("shaders/line.vs", "shaders/line.fs");

    std::vector<Vertex> vertices;
    std::vector<uint> indices;

    vertices.push_back(glm::vec3(0.0f));
    int segments = 16;

    for(int t=0;t<=segments;t+=1)
    {
      float th = (float)TWO_PI/segments;
      vertices.push_back(Vertex(0.0f, radius*sin(th*t), radius*cos(th*t)));
      if(t>0){
        indices.push_back(0);
        indices.push_back(vertices.size());
        indices.push_back(vertices.size()-1);
      }
      indices.push_back(0);
      indices.push_back(vertices.size());
      indices.push_back(1);
    }

    mesh.vertices=vertices;
    mesh.indices=indices;
    mesh.baseColor = color;
    mesh.Refresh();
  }
  void GetRadius(){
    radius = MassToRadius(mass);
    BuildMesh();
  }
  void Step(Planet * sun, std::vector<Planet*>* planets, float g, float deltaTime){
    // if planet is stationary
    mesh.baseColor = color;
    if(is_static){
      return;
    }

    // define acceleration vector
    glm::vec3 acceleration = glm::vec3(0.0f);

    // get distance vector
    glm::vec3 d;
    d.z = sun->transform.position.z - this->transform.position.z;
    d.y = this->transform.position.y - sun->transform.position.y;

    // acceleration = G * M / d.x^2 + d.y^2
    // G is gravity constant
    // M is mass
    // d is distance vector
    //

    float force = (g * sun->mass) / ((d.z*d.z)+(d.y*d.y));
    if(force > max){
      force = max;
    }
    if(force < -max){
      force = -max;
    }
    float angle = atan2(d.y,d.z);
    acceleration += force * glm::vec3(0.0f, -sin(angle), cos(angle));

    // acceleration = G * M / d.x^2 + d.y^2
    // G is gravity constant
    // M is mass
    // d is distance vector
    //

    // for each planet in solar system
    for(Planet * p : *planets){
      // if it's not me
      if(p!=this){
        // get distance vector
        glm::vec3 d;
        d.z = p->transform.position.z - this->transform.position.z;
        d.y = this->transform.position.y - p->transform.position.y;

        float force = (g * p->mass) / ((d.z*d.z)+(d.y*d.y));
        if(force > max){
          force = max;
        }
        if(force < -max){
          force = -max;
        }
        float angle = atan2(d.y,d.z);
        acceleration += force * glm::vec3(0.0f, -sin(angle), cos(angle));
      }
    }
    // apply acceleration
    this->velocity += acceleration * deltaTime;
    this->velocity.x = 0.0f;
    // apply velocity
    this->transform.position += this->velocity * deltaTime;
    this->transform.position.x = 0.0f;

    // draw velocity vector
    Renderer::DrawLine(this->transform.position, this->transform.position-velocity);
  }

  void Draw(){
    Camera * cam = Settings::GetMainCamera();
    cam->SetUniforms(&shader);
    mesh.Draw(&shader, &transform);
    //for(int i = 1; i < mesh.vertices.size(); i++){
      //Renderer::DrawLine(mesh.vertices[i-1].Position+transform.position, mesh.vertices[i].Position+transform.position);
    //}
  }
  void ToString(){
    Print << "Pos: " <<  glm::to_string(transform.position) << Endl;
    Print << "Vel: " << glm::to_string(velocity) << Endl;
  }
};

class Particle{
public:
  Mesh mesh;
  Transform transform;
  Shader shader;
  float radius=4.0f;
  bool is_static = false;
  float mass = 5.0f;
  float max = 100.0f;
  glm::vec3 velocity = glm::vec3(0.0f);
  glm::vec3 color = glm::vec3(1.0f);

  Particle(float x, float y){
    mass = 0.1f;
    radius = MassToRadius(mass);
    transform.position.z = x;
    transform.position.y = y;
    shader.Load("shaders/line.vs", "shaders/line.fs");
    BuildMesh();
  }
  Particle(Planet * p){
    mesh = p->mesh;
    is_static = p->is_static;
    mass = 0.1f;
    velocity = p->velocity;
    transform.position = p->transform.position;
    shader.Load("shaders/line.vs", "shaders/line.fs");
    GetRadius();
  }
  float MassToRadius(float mass){
    return pow((3.0f * mass) / (4.0f * 3.14159265f * 0.1f), (1.0f / 3.0f));
  }
  void BuildMesh(){
    std::vector<Vertex> vertices;
    vertices.push_back(glm::vec3(0.0f));
    mesh.vertices=vertices;
    mesh.Refresh();
  }
  void GetRadius(){
    radius = MassToRadius(mass);
    BuildMesh();
  }
  void Step(Planet * sun, std::vector<Planet*> * planets, float g,  float deltaTime){

    // if planet is stationary
    if(is_static){
      return;
    }

    // define acceleration vector
    glm::vec3 acceleration = glm::vec3(0.0f);

    // get distance vector
    glm::vec3 d;
    d.z = sun->transform.position.z - this->transform.position.z;
    d.y = this->transform.position.y - sun->transform.position.y;

    // acceleration = G * M / d.x^2 + d.y^2
    // G is gravity constant
    // M is mass
    // d is distance vector
    //

    float force = (g * sun->mass) / ((d.z*d.z)+(d.y*d.y));
    if(force > max){
      force = max;
    }
    if(force < -max){
      force = -max;
    }
    float angle = atan2(d.y,d.z);
    acceleration += force * glm::vec3(0.0f, -sin(angle), cos(angle));

    for(Planet * p : *planets){
      // get distance vector
      glm::vec3 d;
      d.z = p->transform.position.z - this->transform.position.z;
      d.y = this->transform.position.y - p->transform.position.y;

      // acceleration = G * M / d.x^2 + d.y^2
      // G is gravity constant
      // M is mass
      // d is distance vector
      //

      float force = (g *  p->mass) / ((d.z*d.z)+(d.y*d.y));
      if(force > max){
        force = max;
      }
      if(force < -max){
        force = -max;
      }
      float angle = atan2(d.y,d.z);
      acceleration += force * glm::vec3(0.0f, -sin(angle), cos(angle));
    }
    mesh.baseColor = glm::normalize(glm::abs(glm::vec3(acceleration.z*acceleration.y)));
    // apply acceleration
    this->velocity += acceleration * deltaTime;
    this->velocity.x = 0.0f;
    // apply velocity
    this->transform.position += this->velocity * deltaTime;
    this->transform.position.x = 0.0f;

    // draw velocity vector
    //Renderer::DrawLine(this->transform.position, this->transform.position-velocity);
  }

  void Draw(){
    Camera * cam = Settings::GetMainCamera();
    cam->SetUniforms(&shader);
    mesh.PointDraw(&shader, &transform);
  }
  void ToString(){
    Print << "Pos: " <<  glm::to_string(transform.position) << Endl;
    Print << "Vel: " << glm::to_string(velocity) << Endl;
  }
};

class Stars{
public:
  Mesh mesh;
  Transform transform;
  Shader shader;
  int number = 1000;

  Stars(){
    shader.Load("shaders/line.vs", "shaders/line.fs");
    BuildMesh();
  }

  void BuildMesh(){
    std::vector<Vertex> vertices;
    vertices.push_back(glm::vec3(0.0f));

    for(int t=0;t<=number;t+=1)
    {
      vertices.push_back(Vertex(0.0f, (float)(rand()%2000)-1000.0f, (float)(rand()%2000)-1000.0f));
    }
    mesh.vertices=vertices;
    mesh.Refresh();
  }

  void Draw(){
    Camera * cam = Settings::GetMainCamera();

    shader.use();
    cam->SetUniforms(&shader);
    mesh.PointDraw(&shader, &transform);
    //for(int i = 1; i < mesh.vertices.size(); i++){
    //  Renderer::DrawPoint(mesh.vertices[i].Position);
    //}
  }
};

class Game{
public:
  float deltaTime=0.0f;
  float lastFrame=0.0f;
  GLFWwindow * window=nullptr;
  Camera * maincam=nullptr;
  FreeCam  * cam;
  std::vector<Planet*> planets;
  std::vector<Particle*> particles;
  Stars * stars;
  float timeScale = 4.0f;
  Planet * sun;
  bool create=false;
  bool gui = true;
  float gravity = 5.85773727315914;
  bool render = true;

  Transform t;
  glm::vec3 velocity=glm::vec3(0.0f, 100.0f, 100.0f);
  float mass=25000.0f;
  Mesh crossHair;
  Shader crosshairshader;

  Game(){
  }
  ~Game(){
  }

  void Init(GLFWwindow * w){

    // init glfw callback functions
    window = w;
    glfwSetFramebufferSizeCallback(window, IO::framebuffer_size_callback);
    //glfwSetCursorPosCallback(window, IO::mouse_callback);
    glfwSetScrollCallback(window, IO::scroll_callback);
    glfwSetKeyCallback(window, IO::key_callback);
    glfwSetDropCallback(window, IO::drop_callback);

    // initialize static IO manager
    IO::Get(window);
    //Renderer::SetRenderMode(GL_LINE);
    //camera.perspective_ortho = true;
    //camera.Pitch = 0.0f;
    //camera.Yaw = 0.0f;
    // model load static function that loads model and puts meshes into Brush e
    //Model::Load("assets/objects/static/mineral/mineral.obj", e);

    cam = new FreeCam();
    cam->orthographic = true;

    //cam.free_movement = false;
    maincam = (Camera*)&cam;

    stars = new Stars();
    AddSun();

    std::vector<Vertex> v = {
      Vertex(0.0f, 0.0f, -10.0f),
      Vertex(0.0f, 0.0f, 10.0f),
      Vertex(0.0f, -10.0f, 0.0f),
      Vertex(0.0f, 10.0f, 0.0f),
    };
    std::vector<uint> i = {
      0, 1, 2, 3
    };
    crossHair.vertices = v;
    crossHair.indices = i;
    crossHair.Refresh();
    crosshairshader.Load("shaders/line.vs", "shaders/line.fs");

  }

  void AddSun(){
    sun = new Planet(0,0,1000000,glm::vec3(1.0f, 1.0f, 0.0f));
    sun->is_static=true;
  }

  void Step(float deltaTime){

    ImGuizmo::BeginFrame();
    ImGuiLayer::Step(this);

    cam->Step(window, deltaTime);

    float max = 100.0f;

    if(!IO::MouseHidden()){
      glm::vec2 mouse = IO::GetMousePos();
      t.position.z = (mouse.x);
      t.position.y = -(mouse.y);
    }

    std::string fps = std::to_string(1.0f/deltaTime);

    if(gui){
      ImGui::Begin("Universe");
      ImGui::SliderFloat("time scale", &timeScale, 1.0f, 10.0f);
      ImGui::SliderFloat("velocity x", &velocity.z, -max, max);
      ImGui::SliderFloat("velocity y", &velocity.y, -max, max);
      ImGui::SliderFloat("mass", &mass, 0.01f, 100000.0f);
      ImGui::SliderFloat("gravity constant", &gravity, 0.0f, 10.0f);
    }

    ImGuiIO& io = ImGui::GetIO();

    if(IO::MouseDown(0)&&!io.WantCaptureMouse){
      AddPlanet();
    }else if(IO::MouseHeld(1)&&!io.WantCaptureMouse){
      AddStardust();
    }
    if(ImGui::Button("Clear Planets")){
      planets.clear();
    }
    if(ImGui::Button("Clear Particles")){
      particles.clear();
    }

    std::string n_planets = std::to_string(planets.size());
    std::string n_particles = std::to_string(particles.size());

    ImGui::Text("Planets:");
    ImGui::SameLine();
    ImGui::Text(n_planets.c_str());

    ImGui::Text("Particles:");
    ImGui::SameLine();
    ImGui::Text(n_particles.c_str());

    ImGui::Text("Z to switch mouse");
    ImGui::Text("RMB to create stardust");
    ImGui::Text("LMB to create planet");
    ImGui::Text("Esc to quit");

    ImGui::Text("Fps:");
    ImGui::SameLine();
    ImGui::Text(fps.c_str());

    ImGui::End();

    for(Planet* p : planets){
      p->Step(sun, &planets, gravity, deltaTime*timeScale);
      // if planet is outside universe
      glm::vec3 point = p->transform.position;

      if(point.z > 2000 ||
        point.z < -2000 ||
        point.y > 2000 ||
        point.y < -2000){
          planets.erase(std::remove(planets.begin(), planets.end(), p), planets.end());
      }
    }
    for(Particle* p : particles){
      p->Step(sun, &planets, gravity, deltaTime*timeScale);
      // if planet is outside universe
      glm::vec3 point = p->transform.position;

      if(point.z > 1000 ||
        point.z < -1000 ||
        point.y > 1000 ||
        point.y < -1000){
          particles.erase(std::remove(particles.begin(), particles.end(), p), particles.end());
      }
    }

    glm::vec2 p = IO::GetMousePos();

  }
  void AddPlanet(){
    Planet * newplanet = new Planet(t.position.z,t.position.y,mass);
    newplanet->velocity = velocity;
    planets.push_back(newplanet);
  }
  void AddStardust(){
    Particle * part = new Particle(t.position.z,t.position.y);
    part->velocity = velocity;
    particles.push_back(part);
  }
  void Render(float deltaTime){
    // inputs
    // -----
    glFrontFace(GL_CW);
    IO::processInput(window, deltaTime);

    if(render){
      //Renderer::Render((Camera*)cam);
      // begin world render pass
      glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
      //glViewport(0, 0, Settings::GetWidth()*Settings::GetScale()/2.0f, Settings::GetHeight()*Settings::GetScale()/2.0f);

      for(Planet* p : planets){
        p->Draw();
      }
      for(Particle* p : particles){
        p->Draw();
      }

    }
    stars->Draw();
    sun->Draw();


    Camera * cam = Settings::GetMainCamera();
    cam->SetUniforms(&crosshairshader);
    crossHair.LineDraw(&crosshairshader, &t);
    //Renderer::RenderLines(Settings::GetMainCamera());

    ImGuiLayer::EndFrame();
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
    // -------------------------------------------------------------------------------
    glfwSwapBuffers(window);
  }
  void End(){

  }

};
