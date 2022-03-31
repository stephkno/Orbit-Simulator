#include <render/mesh.h>
#include <transform.h>
#include <render/shader.h>
#include <render/material.h>
#include <render/renderer.h>

void Mesh::Draw(Shader *shader, Transform * transform)
{
  // apply transform to localTransform
  // apply localTransform to transform -> thisTransform
  // debug render normals
  /*
  for(Vertex & v : vertices){
    glm::vec3 p = (v.Position * transform->scale)+transform->position;// + (transform->scale/2.0f) + transform->position;
    glm::vec3 n = v.Normal;

    Renderer::DrawLine(p, p+n);
  }
  */
  shader->use();
  shader->setMat4("model", transform->ModelMatrix());
  shader->setVec3("color", baseColor);
  shader->setBool("selected", selected);
  shader->setVec2("textureScale", textureScale);

  // draw mesh
  // draw mesh
  glBindVertexArray(VAO);
  glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
  glBindVertexArray(0);

}

void Mesh::LineDraw(Shader *shader, Transform * transform)
{

//  material.SetUniforms(shader);
//  Renderer::GetSun()->SetSunUniforms(shader);
  shader->use();
  shader->setMat4("model", transform->ModelMatrix());
  shader->setVec3("color", baseColor);
  shader->setBool("selected", selected);
  shader->setVec2("textureScale", textureScale);
  shader->setBool("lit", false);

  // draw mesh
  // draw mesh
  glBindVertexArray(VAO);
  glDrawElements(GL_LINES, indices.size(), GL_UNSIGNED_INT, 0);
  glBindVertexArray(0);
  //material->Unbind();

}

void Mesh::PointDraw(Shader *shader, Transform * transform)
{

//  material.SetUniforms(shader);
//  Renderer::GetSun()->SetSunUniforms(shader);

  shader->use();
  shader->setMat4("model", transform->ModelMatrix());
  shader->setVec3("color", baseColor);
  shader->setBool("selected", selected);
  shader->setVec2("textureScale", textureScale);
  shader->setBool("lit", false);

  // draw mesh
  // draw mesh
  glBindVertexArray(VAO);
  glDrawArrays(GL_POINTS, 0, vertices.size());
  glBindVertexArray(0);

  //material->Unbind();

}
