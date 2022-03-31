#pragma once


struct OrientedPoint{
  glm::vec3 pos;
  glm::quat rot;
  OrientedPoint(glm::vec3 p, glm::quat r){
    pos = p;
    rot = r;
  }
  OrientedPoint(glm::vec3 p, glm::vec3 f){
    pos = p;
    rot = Quaternion.LookRotation(forward);
  }
  glm::vec3 LocalToWorldPos(glm::vec3 localSpacePos){
    return pos + rot * localSpacePos;
  }
  glm::vec3 LocalToWorldVec(glm::vec3 localSpacePos){
    return rot * localSpacePos;
  }
};

class Bezier{
public:
  glm::mat4 controlPoints[4];

  Bezier(){

  }
  ~Bezier(){

  }
  void Test(){
    float tTest = 0.4f;
    glm::vec3 testPoint = GetBezierPoint(tTest);
    glm::quat testOrientation = GetBezierOrientation(tTest);
  }
  OrientedPoint GetBezierPoint(float t){
    glm::vec3 p0 = GetPos(0);
    glm::vec3 p1 = GetPos(1);
    glm::vec3 p2 = GetPos(2);
    glm::vec3 p3 = GetPos(3);
    glm::vec3 a = glm::lerp(p0, p1, t);
    glm::vec3 b = glm::lerp(p1, p2, t);
    glm::vec3 c = glm::lerp(p2, p3, t);
    glm::vec3 d = glm::lerp(a, b, t);
    glm::vec3 e = glm::lerp(b, c, t);
    glm::vec3 p = glm::lerp(d,e,t);
    glm::vec3 r = glm::normal(e - d);
    OrientedPoint op;
    op.pos = p;
    op.rot = r;
    return op;
  }
  glm::quat GetBezierOrientation(float t){
    glm::vec3 tangent = GetTangent(0);
    // ????
    return quaternion.LookRotation(tangent);
  }
  void GenerateMesh(){

    mesh.Clear();
    std::vector<Vertex> verts = std::vector<Vertex>();
    // vertices
    for(int ring = 0; ring < edgeRingCount; ring++){
      float t = ring/(edgeRingCount-1.0f);
      OrientedPoint op = GetBezierPoint(t);
      for(int i = 0; i < shape2D.vertices.size(); i++){
        verts.push_back(op.LocalToWorld(shape2D.vertices[i]));
      }
    }
    // triangles
    for(int ring = 0; ring < edgeRingCount-1; ring++){
      int rootIndex = ring * shape2D.vertices.size();
      int rootindexNext = (ring+1) * shape2D.vertices.size();
      for(int line = 0; line < shape2D.lineCount; line++){
        int lineIndexA = shape2D.lineIndices[line];
        int lineIndexB = shape2D.lineIndices[line+1];
        int currentA = rootIndex + lineIndexA;
        int currentB = rootIndex + lineIndexB;
        int nextA = rootIndexNext + lineIndexA;
        int nextB = rootIndexNext + lineIndexB;
        triIndices.push_back(currentA);
        triIndices.push_back(nextA);
        triIndices.push_back(nextB);
        triIndices.push_back(currentA);
        triIndices.push_back(nextB);
        triIndices.push_back(currentB);
      }
    }
    mesh.vertices = verts;
    mesh.indices = triIndices;
    return mesh;
  }
}
