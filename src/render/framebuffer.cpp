#include <settings.h>
#include <render/framebuffer.h>

Framebuffer::Framebuffer(){
  screenShader = new Shader();
  screenShader->Load("shaders/postprocess.vs", "shaders/postprocess.fs");
  float quadVertices[] = { // vertex attributes for a quad that fills the entire screen in Normalized Device Coordinates.
     // positions   // texCoords
     -1.0f,  1.0f,  0.0f, 1.0f,
     -1.0f, -1.0f,  0.0f, 0.0f,
      1.0f, -1.0f,  1.0f, 0.0f,

     -1.0f,  1.0f,  0.0f, 1.0f,
      1.0f, -1.0f,  1.0f, 0.0f,
      1.0f,  1.0f,  1.0f, 1.0f
 };

  // framebuffer configuration
  // -------------------------
  glGenFramebuffers(1, &framebuffer);
  glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
  // create a color attachment tex.id
  glGenTextures(1, &tex.id);
  glBindTexture(GL_TEXTURE_2D, tex.id);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, Settings::GetWidth(), Settings::GetHeight(), 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, tex.id, 0);
  // create a renderbuffer object for depth and stencil attachment (we won't be sampling these)
  glGenRenderbuffers(1, &rbo);
  glBindRenderbuffer(GL_RENDERBUFFER, rbo);
  glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, Settings::GetWidth(), Settings::GetHeight()); // use a single renderbuffer object for both a depth AND stencil buffer.
  glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo); // now actually attach it
  // now that we actually created the framebuffer and added all attachments we want to check if it is actually complete now
  if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
  glBindFramebuffer(GL_FRAMEBUFFER, 0);

  glGenVertexArrays(1, &quadVAO);
  glGenBuffers(1, &quadVBO);
  glBindVertexArray(quadVAO);
  glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
  glEnableVertexAttribArray(1);
  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
  if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE){
    Print << "Framebuffer not complete!" << Endl;
  }
}
Framebuffer::~Framebuffer(){

}
void Framebuffer::Activate(){
  glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
}

void Framebuffer::Deactivate(){
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
  // clear all relevant buffers
}

void Framebuffer::Render(){
  glViewport(0,0,Settings::GetWidth(), Settings::GetHeight());
  screenShader->use();
  screenShader->setInt("screenTexture", 0);
  glDisable(GL_DEPTH_TEST); // disable depth test so screen-space quad isn't discarded due to depth test.

  glBindVertexArray(quadVAO);
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, tex.id);	// use the color attachment tex.id as the tex.id of the quad plane
  glDrawArrays(GL_TRIANGLES, 0, 6);

  glEnable(GL_DEPTH_TEST); // enable depth testing (is disabled for rendering screen-space quad)

  // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
  // -------------------------------------------------------------------------------
  //glfwSwapBuffers(Settings::GetWindow());
  //glfwPollEvents();
}
void Framebuffer::Render(int x, int y, int w, int h){
  glViewport(x,y,w,h);
  screenShader->use();
  screenShader->setInt("screenTexture", 0);
  glDisable(GL_DEPTH_TEST); // disable depth test so screen-space quad isn't discarded due to depth test.

  glBindVertexArray(quadVAO);
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, tex.id);	// use the color attachment tex.id as the tex.id of the quad plane
  glDrawArrays(GL_TRIANGLES, 0, 6);

  glEnable(GL_DEPTH_TEST); // enable depth testing (is disabled for rendering screen-space quad)
  glViewport(0,0,Settings::GetWidth(), Settings::GetHeight());
}
