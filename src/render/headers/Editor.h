#pragma once
#include <GL/glew.h>
#include "imgui.h"
#include <stdarg.h>
#include <stdio.h>
#include <sstream>
#include <cstring>
#include <string>
#include <vector>
#include "Object.h"
#include "ImGuizmo/ImGuizmo.h"
#include "Box.h"
#include "IO.h"
#include "Shader.h"
#include "Transform.h"
#include "Camera.h"

class LineRenderer;
class RayCast;
class Terminal;
class SubMesh;
class BrushTool;
class RaycastHit;


enum EditorState{
  TRANSFORM,
  ROTATE,
  SCALE
};

class Editor{
private:
  static Editor * ed;
public:
  bool enabled = false;
  bool drag;
  Terminal * terminal;
  std::stringstream ss;
  SubMesh * m;
  BrushTool * brushTool;
  Camera * editorCamera[3];

  // current object context
  Object * currentObject;

  Editor();
  ~Editor();
  // static functions
  static Editor * InitEditor();
  static bool IsEnabled();

  static void Step(float deltaTime);
  static void Debug(char *,...);
  void RenderLines();
  void UpdateTool(RaycastHit * rch);

  // transform snap
  float snap[3] = { 0.1f, 0.1f, 0.1f };
  // default bounds size
  float bounds[6] = { -0.5f, -0.5f, -0.5f, 0.5f, 0.5f, 0.5f };
  // bound size snap
  float boundsSnap[3] = { snap[0]/2.0f,  snap[1]/2.0f, snap[2]/2.0f };

  static float * Snap(){
    return &ed->snap[0];
  }
  static float * BrushBounds(){
    return &ed->bounds[0];
  }
  static float * BrushBoundsSnap(){
    return &ed->boundsSnap[0];
  }
  static void SetSnap(float s){
    for(int i = 0; i < 3; i++){
      ed->snap[i] = s;
      ed->boundsSnap[i] = s/2.0f;
    }
  }
  static Camera * GetEditorCamera(int c){
    if(!ed){
      ed = new Editor();
    }
    return ed->editorCamera[c];
  }

  static void Debug(int level, char *,...);
  static char * convert(unsigned int num, int base){
    static char rep[] = "0123456789ABCDEF";
    static char buffer[50];
    char *ptr;

    ptr = &buffer[49];
    *ptr = '\0';

    do{
      *--ptr=rep[num%base];
      num/=base;
    }while(num!=0);

    return ptr;
  }

  // ----
  void EditorStep(float deltaTime);
  void Render(){
    RenderBrushTool();
  }
  void  RenderBrushTool();
};
