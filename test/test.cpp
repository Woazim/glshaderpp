#ifdef _WIN32
#include <Windows.h>
#endif
#define CATCH_CONFIG_MAIN // This tells Catch to provide a main() 

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <GLShaderPP/ShaderProgram.h>
#include <catch2/catch.hpp>

constexpr float pi = 3.1415926535897932384626433832795f;

#ifdef _WIN32
//This magic line is to force notebook computer that share NVidia and Intel graphics to use high performance GPU (NVidia).
//Some old intel graphics doesn't support OpenGL > 1.2
extern "C" _declspec(dllexport) unsigned long NvOptimusEnablement = 0x00000001;
#endif

const char* GetGLErrorString();
void error_callback(int error, const char* description)
{
  INFO("GLFW3 error #" << error << ": " << description << '\n');
  REQUIRE(false);
}

TEST_CASE("Create a typical GLSL program", "[shader]")
{
  const char* pText;
  constexpr int nWndWidth = 800;
  constexpr int nWndHeight = 600;

  glfwSetErrorCallback(error_callback);
  REQUIRE(glfwInit() == GLFW_TRUE);

  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
  glfwWindowHint(GLFW_DOUBLEBUFFER, GL_FALSE);
  glfwWindowHint(GLFW_VISIBLE, GL_FALSE);
  GLFWwindow* pWnd = glfwCreateWindow(nWndWidth, nWndHeight, "Hello World", nullptr, nullptr);
  REQUIRE(pWnd != nullptr);

  glfwMakeContextCurrent(pWnd);

  glewExperimental = GL_TRUE;
  GLenum err;
  err = glewInit();
  {
    INFO("glewInit error: " << (const char*)glewGetErrorString(err));
    REQUIRE(err == GLEW_OK);
  }

  //Create an OpenGL driver info string
  std::string strGLDriverInfo;
  {
    bool everythingAllright = true;
    std::stringstream ss;
    pText = (char*)glGetString(GL_VERSION);
    if (!pText)
    {
      pText = GetGLErrorString();
      everythingAllright = false;
    }
    ss << "OpenGL " << pText;

    pText = (char*)glGetString(GL_RENDERER);
    if (!pText)
    {
      pText = GetGLErrorString();
      everythingAllright = false;
    }
    ss << " running on " << pText;

    pText = (char*)glGetString(GL_VENDOR);
    if (!pText)
    {
      pText = GetGLErrorString();
      everythingAllright = false;
    }
    ss << " from " << pText;

    strGLDriverInfo = ss.str();

    INFO(strGLDriverInfo);
    CHECK(everythingAllright);
  }
  INFO(strGLDriverInfo);

  CShaderProgram program{ 
    CShader{ GL_VERTEX_SHADER, std::ifstream{ "vertex.vert" } },
    CShader{ GL_FRAGMENT_SHADER, std::ifstream{ "fragment.frag" } }
  };

  REQUIRE(program.GetLinkingStatus() == CShaderProgram::linkingOk);
  program.Use();

  GLfloat vertices[] = {
    //Positions        //Colors       
    -0.5f, -0.5f,   1.0f, 0.0f, 0.0f,   //Vertex 1
     0.5f, -0.5f,   0.0f, 1.0f, 0.0f,   //Vertex 2
     0.0f,  0.5f,   0.0f, 0.0f, 1.0f    //Vertex 3
  };
  //Vertex specifications for this shader
  // The VAO
  GLuint VAO;
  glGenVertexArrays(1, &VAO);
  glBindVertexArray(VAO);

  // The VBO
  GLuint VBO;
  glGenBuffers(1, &VBO);
  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

  // Specify vertex shader input organisation
  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), reinterpret_cast<GLvoid*>(0));
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), reinterpret_cast<GLvoid*>(2 * sizeof(GLfloat)));
  glEnableVertexAttribArray(1);

  //Create an offscreen FBO
  GLuint fbo;
  glGenFramebuffers(1, &fbo);
  glBindFramebuffer(GL_FRAMEBUFFER, fbo);
  GLuint rbo;
  glGenRenderbuffers(1, &rbo);
  glBindRenderbuffer(GL_RENDERBUFFER, rbo);
  glRenderbufferStorage(GL_RENDERBUFFER, GL_RGBA, nWndWidth, nWndHeight);
  glBindRenderbuffer(GL_RENDERBUFFER, 0);
  //Atache le render buffer au framebuffer actif
  glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, rbo);

  //Not usefull to go throug a texture since there is glReadPixels
  ////Create a texture for its color buffer
  //GLuint texColorBuffer;
  //glGenTextures(1, &texColorBuffer);
  //glBindTexture(GL_TEXTURE_2D, texColorBuffer);
  //glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, nWndWidth, nWndHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
  //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  //glBindTexture(GL_TEXTURE_2D, 0);
  ////Attach this texture to the offscreen FBO
  //glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texColorBuffer, 0);

  //Not usefull since we don't use depth-buffer
  ////Create a render buffer object for depth buffer
  //GLuint rbo;
  //glGenRenderbuffers(1, &rbo);
  //glBindRenderbuffer(GL_RENDERBUFFER, rbo);
  //glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT32F, nWndWidth, nWndHeight);
  //glBindRenderbuffer(GL_RENDERBUFFER, 0);
  ////Atache le render buffer au framebuffer actif
  //glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rbo);

  REQUIRE(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE);

  //Make a rendering in this FBO
  glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT);
  glDrawArrays(GL_TRIANGLES, 0, 3);
  glFinish();

  //"Download" pixels from the texture
  std::vector<unsigned char> pixelData(nWndWidth * nWndHeight * 3);
  glReadPixels(0, 0, nWndWidth, nWndHeight, GL_RGB, GL_UNSIGNED_BYTE, pixelData.data());

  //compare these pixels with a stored PPM
  ////The following commented code may produce this file
  //std::ofstream ofs("test.ppm", std::ios_base::binary | std::ios_base::out | std::ios_base::trunc);
  //ofs << "P6\n" << nWndWidth << ' ' << nWndHeight << ' ' << "255\n";
  //for (int i = nWndHeight - 1; i >= 0; --i)
  //{
  //  unsigned char* pPixel = &pixelData[i * nWndWidth * 3];
  //  for (int j = 0; j < nWndWidth; ++j)
  //  {
  //    ofs << *pPixel++;//R
  //    ofs << *pPixel++;//G
  //    ofs << *pPixel++;//B
  //  }
  //}

  std::ifstream ifs("test.ppm", std::ios_base::binary | std::ios_base::in);
  std::string strTmp;
  int nTmp;
  ifs >> strTmp;
  REQUIRE(strTmp == "P6");
  ifs >> nTmp;
  REQUIRE(nTmp == nWndWidth);
  ifs >> nTmp;
  REQUIRE(nTmp == nWndHeight);
  ifs >> nTmp;
  REQUIRE(nTmp == 255);
  ifs.ignore();
  std::vector<unsigned char> pixelTestData(nWndWidth * nWndHeight * 3);
  ifs.read(reinterpret_cast<char*>(pixelTestData.data()), nWndWidth * nWndHeight * 3);
  REQUIRE(ifs.gcount() == nWndWidth * nWndHeight * 3);
  unsigned char* pPixelTest = pixelTestData.data();
  for (int i = nWndHeight - 1; i >= 0; --i)
  {
    unsigned char* pPixel = &pixelData[i * nWndWidth * 3];
    for (int j = 0; j < nWndWidth; ++j)
    {
      unsigned char r, g, b;
      r = *pPixelTest++;
      g = *pPixelTest++;
      b = *pPixelTest++;
      short pixel = *pPixel++;
      {
        INFO("pixel (" << nWndHeight - i - 1 << ", " << j << ") red is " << pixel << " and should be " << (short)r);
        CHECK(std::abs(pixel - r) <= 2);//R
      }
      pixel = *pPixel++;
      {
        INFO("pixel (" << nWndHeight - i - 1 << ", " << j << ") green is " << pixel << " and should be " << (short)g);
        CHECK(std::abs(pixel - g) <= 2);//G
      }
      pixel = *pPixel++;
      {
        INFO("pixel (" << nWndHeight - i - 1 << ", " << j << ") blue is " << pixel << " and should be " << (short)b);
        CHECK(std::abs(pixel - b) <= 2);//B
      }
    }
  }

  glfwTerminate();
}


const char* GetGLErrorString()
{
  switch (glGetError())
  {
  case GL_NO_ERROR:
    return "GL_NO_ERROR";
  case GL_INVALID_ENUM:
    return "GL_INVALID_ENUM";
  case GL_INVALID_VALUE:
    return "GL_INVALID_VALUE";
  case GL_INVALID_OPERATION:
    return "GL_INVALID_OPERATION";
  case GL_STACK_OVERFLOW:
    return "GL_STACK_OVERFLOW";
  case GL_STACK_UNDERFLOW:
    return "GL_STACK_UNDERFLOW";
  case GL_OUT_OF_MEMORY:
    return "GL_OUT_OF_MEMORY";
  }
  return "Unknown error";
}
