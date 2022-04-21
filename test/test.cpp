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
#include <GLShaderPP/ShaderException.h>
#include <catch2/catch.hpp>

using namespace std::string_literals;

constexpr float pi = 3.1415926535897932384626433832795f;

#ifdef _WIN32
//This magic line is to force notebook computer that share NVidia and Intel graphics to use high performance GPU (NVidia).
//Some old intel graphics doesn't support OpenGL > 1.2
extern "C" _declspec(dllexport) unsigned long NvOptimusEnablement = 0x00000001;
#endif


struct AreSimilarShaderExceptionMatcher : Catch::MatcherBase<GLShaderPP::CShaderException>
{
  GLShaderPP::CShaderException m_shaderException;
public:
  AreSimilarShaderExceptionMatcher(const GLShaderPP::CShaderException& e) : m_shaderException{ e } {}

  bool match(const GLShaderPP::CShaderException& e) const override {
    return e.type() == m_shaderException.type();
  }

  std::string describe() const override {
    return "CShaderException are of the same type"s;
  }
};

AreSimilarShaderExceptionMatcher AreSimilarShaderException(const GLShaderPP::CShaderException& e) {
  return { e };
}


const char* GetGLErrorString();
void error_callback(int error, const char* description)
{
  INFO("GLFW3 error #" << error << ": " << description << '\n');
  REQUIRE(false);
}

std::string initWindow(const int nWndWidth, const int nWndHeight)
{
  const char* pText;

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
  return strGLDriverInfo;
}

void testTriangle(const int nWndWidth, const int nWndHeight)
{
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

}

//This test must be the first one, since others will initialise Glew.
TEST_CASE("Try to create a shader object without having called glewInit", "[no-glewinit]")
{
  CHECK_THROWS_MATCHES(
    std::make_unique<GLShaderPP::CShader>(GL_VERTEX_SHADER),
    GLShaderPP::CShaderException,
    AreSimilarShaderException(GLShaderPP::CShaderException(""s, GLShaderPP::CShaderException::ExceptionType::GlewInit))
  );
}

TEST_CASE("Create a typical GLSL program from files at construction of CShaderProgram", "[direct-shader-from-files]")
{
  constexpr int nWndWidth = 800;
  constexpr int nWndHeight = 600;

  INFO(initWindow(nWndWidth, nWndHeight));

  GLShaderPP::CShaderProgram program{
    GLShaderPP::CShader{ GL_VERTEX_SHADER, std::ifstream{ "vertex.vert" } },
    GLShaderPP::CShader{ GL_FRAGMENT_SHADER, std::ifstream{ "fragment.frag" } }
  };

  REQUIRE(program.GetLinkingStatus() == GLShaderPP::CShaderProgram::LinkingStatus::linkingOk);
  program.Use();

  testTriangle(nWndWidth, nWndHeight);

  glfwTerminate();
}

TEST_CASE("Create a typical GLSL program from files after construction of CShaderProgram", "[shader-from-files]")
{
  constexpr int nWndWidth = 800;
  constexpr int nWndHeight = 600;

  INFO(initWindow(nWndWidth, nWndHeight));

  GLShaderPP::CShaderProgram program;
  CHECK(program.GetLinkingStatus() == GLShaderPP::CShaderProgram::LinkingStatus::notLinked);

  program.AttachShader(GLShaderPP::CShader{ GL_VERTEX_SHADER, std::ifstream{ "vertex.vert" } });
  program.AttachShader(GLShaderPP::CShader{ GL_FRAGMENT_SHADER, std::ifstream{ "fragment.frag" } });
  CHECK(program.GetLinkingStatus() == GLShaderPP::CShaderProgram::LinkingStatus::notLinked);

  program.Link();
  REQUIRE(program.GetLinkingStatus() == GLShaderPP::CShaderProgram::LinkingStatus::linkingOk);
  program.Use();

  testTriangle(nWndWidth, nWndHeight);

  glfwTerminate();
}

TEST_CASE("Create a typical GLSL program from strings at construction of CShaderProgram", "[direct-shader-from-strings]")
{
  constexpr int nWndWidth = 800;
  constexpr int nWndHeight = 600;

  INFO(initWindow(nWndWidth, nWndHeight));

  std::stringstream vertexShaderStream;
  vertexShaderStream << std::ifstream{ "vertex.vert" }.rdbuf();
  std::stringstream fragmentShaderStream;
  fragmentShaderStream << std::ifstream{ "fragment.frag" }.rdbuf();

  GLShaderPP::CShaderProgram program{
    GLShaderPP::CShader{ GL_VERTEX_SHADER, vertexShaderStream.str() },
    GLShaderPP::CShader{ GL_FRAGMENT_SHADER, fragmentShaderStream.str() }
  };

  REQUIRE(program.GetLinkingStatus() == GLShaderPP::CShaderProgram::LinkingStatus::linkingOk);
  program.Use();

  testTriangle(nWndWidth, nWndHeight);

  glfwTerminate();
}

TEST_CASE("Create a vertex shader from files at construction of CShader", "[direct-vertex-from-files]")
{
  constexpr int nWndWidth = 800;
  constexpr int nWndHeight = 600;

  INFO(initWindow(nWndWidth, nWndHeight));

  GLShaderPP::CShader shader{ GL_VERTEX_SHADER, std::ifstream{ "vertex.vert" } };

  CHECK(shader.GetType() == "vertex");
  CHECK(shader.GetCompileState() == GLShaderPP::CShader::ShaderCompileState::compileOk);

  glfwTerminate();
}

TEST_CASE("Create a fragment shader from files at construction of CShader", "[direct-fragment-from-files]")
{
  constexpr int nWndWidth = 800;
  constexpr int nWndHeight = 600;

  INFO(initWindow(nWndWidth, nWndHeight));

  GLShaderPP::CShader shader{ GL_FRAGMENT_SHADER, std::ifstream{ "fragment.frag" } };

  CHECK(shader.GetType() == "fragment");
  CHECK(shader.GetCompileState() == GLShaderPP::CShader::ShaderCompileState::compileOk);

  glfwTerminate();
}

TEST_CASE("Create a vertex shader from files after construction of CShader", "[vertex-from-files]")
{
  constexpr int nWndWidth = 800;
  constexpr int nWndHeight = 600;

  INFO(initWindow(nWndWidth, nWndHeight));

  GLShaderPP::CShader shader{ GL_VERTEX_SHADER };
  CHECK(shader.GetType() == "vertex");
  CHECK(shader.GetCompileState() == GLShaderPP::CShader::ShaderCompileState::notCompiled);

  shader.SetSource(std::ifstream{ "vertex.vert" });
  CHECK(shader.GetCompileState() == GLShaderPP::CShader::ShaderCompileState::notCompiled);

  shader.Compile();
  CHECK(shader.GetCompileState() == GLShaderPP::CShader::ShaderCompileState::compileOk);

  glfwTerminate();
}

TEST_CASE("Create a fragment shader from files after construction of CShader", "[fragment-from-files]")
{
  constexpr int nWndWidth = 800;
  constexpr int nWndHeight = 600;

  INFO(initWindow(nWndWidth, nWndHeight));

  GLShaderPP::CShader shader{ GL_FRAGMENT_SHADER };
  CHECK(shader.GetType() == "fragment");
  CHECK(shader.GetCompileState() == GLShaderPP::CShader::ShaderCompileState::notCompiled);

  shader.SetSource(std::ifstream{ "fragment.frag" });
  CHECK(shader.GetCompileState() == GLShaderPP::CShader::ShaderCompileState::notCompiled);

  shader.Compile();
  CHECK(shader.GetCompileState() == GLShaderPP::CShader::ShaderCompileState::compileOk);

  glfwTerminate();
}

TEST_CASE("Create a non compilable vertex shader at construction of CShader", "[direct-faulty-vertex]")
{
  constexpr int nWndWidth = 800;
  constexpr int nWndHeight = 600;

  INFO(initWindow(nWndWidth, nWndHeight));

  CHECK_THROWS_MATCHES(
    std::make_unique<GLShaderPP::CShader>(GL_VERTEX_SHADER, "This shader won't compile"s),
    GLShaderPP::CShaderException,
    AreSimilarShaderException(GLShaderPP::CShaderException(""s, GLShaderPP::CShaderException::ExceptionType::CompilationError))
  );

  glfwTerminate();
}

TEST_CASE("Create a non compilable vertex shader after construction of CShader", "[faulty-vertex]")
{
  constexpr int nWndWidth = 800;
  constexpr int nWndHeight = 600;

  INFO(initWindow(nWndWidth, nWndHeight));

  GLShaderPP::CShader shader{ GL_VERTEX_SHADER };
  shader.SetSource("This shader won't compile"s);
  CHECK_THROWS_MATCHES(
    shader.Compile(),
    GLShaderPP::CShaderException,
    AreSimilarShaderException(GLShaderPP::CShaderException(""s, GLShaderPP::CShaderException::ExceptionType::CompilationError))
  );

  glfwTerminate();
}

TEST_CASE("Create a non linkable program shader at construction of CShaderProgram", "[direct-notready-program]")
{
  constexpr int nWndWidth = 800;
  constexpr int nWndHeight = 600;

  INFO(initWindow(nWndWidth, nWndHeight));

  GLShaderPP::CShader vertex{ GL_VERTEX_SHADER };
  CHECK_THROWS_MATCHES(
    std::make_unique<GLShaderPP::CShaderProgram>(vertex),
    GLShaderPP::CShaderException,
    AreSimilarShaderException(GLShaderPP::CShaderException(""s, GLShaderPP::CShaderException::ExceptionType::PrepareLinkError))
  );

  glfwTerminate();
}

TEST_CASE("Create a non linkable program shader after construction of CShaderProgram", "[notready-program]")
{
  constexpr int nWndWidth = 800;
  constexpr int nWndHeight = 600;

  INFO(initWindow(nWndWidth, nWndHeight));

  GLShaderPP::CShaderProgram program;
  GLShaderPP::CShader vertex{ GL_VERTEX_SHADER };
  CHECK_THROWS_MATCHES(
    program.AttachShader(vertex),
    GLShaderPP::CShaderException,
    AreSimilarShaderException(GLShaderPP::CShaderException(""s, GLShaderPP::CShaderException::ExceptionType::PrepareLinkError))
  );

  glfwTerminate();
}

TEST_CASE("Create a faulty program shader at construction of CShaderProgram", "[direct-faulty-program]")
{
  constexpr int nWndWidth = 800;
  constexpr int nWndHeight = 600;

  INFO(initWindow(nWndWidth, nWndHeight));

  GLShaderPP::CShader vertex1{ GL_VERTEX_SHADER, R"SHADER(#version 330 core
void main()
{
  gl_Position = vec4(1.0f);
}
)SHADER" };
  GLShaderPP::CShader vertex2{ GL_VERTEX_SHADER, R"SHADER(#version 330 core
void main()
{
  gl_Position = vec4(1.0f);
}
)SHADER" };
  
  //This will fails since there are two main functions in vertex shader
  CHECK_THROWS_MATCHES(
    std::make_unique<GLShaderPP::CShaderProgram>(vertex1, vertex2),
    GLShaderPP::CShaderException,
    AreSimilarShaderException(GLShaderPP::CShaderException(""s, GLShaderPP::CShaderException::ExceptionType::LinkError))
  );

  glfwTerminate();
}

TEST_CASE("Create a faulty program shader after construction of CShaderProgram", "[faulty-program]")
{
  constexpr int nWndWidth = 800;
  constexpr int nWndHeight = 600;

  INFO(initWindow(nWndWidth, nWndHeight));

  GLShaderPP::CShaderProgram program;
  GLShaderPP::CShader vertex1{ GL_VERTEX_SHADER, R"SHADER(#version 330 core
void main()
{
  gl_Position = vec4(1.0f);
}
)SHADER" };
  GLShaderPP::CShader vertex2{ GL_VERTEX_SHADER, R"SHADER(#version 330 core
void main()
{
  gl_Position = vec4(1.0f);
}
)SHADER" };

  program.AttachShader(vertex1);
  program.AttachShader(vertex2);

  //This will fails since there are two main functions in vertex shader
  CHECK_THROWS_MATCHES(
    program.Link(),
    GLShaderPP::CShaderException,
    AreSimilarShaderException(GLShaderPP::CShaderException(""s, GLShaderPP::CShaderException::ExceptionType::LinkError))
  );

  glfwTerminate();
}

TEST_CASE("Try to read source from a bad stream", "[bad-source-stream]")
{
  constexpr int nWndWidth = 800;
  constexpr int nWndHeight = 600;

  INFO(initWindow(nWndWidth, nWndHeight));

  GLShaderPP::CShader vertex{ GL_VERTEX_SHADER };

  //This will fails since there are two main functions in vertex shader
  CHECK_THROWS_MATCHES(
    vertex.SetSource(std::ifstream("This file should not exists", std::ios_base::in)),
    GLShaderPP::CShaderException,
    AreSimilarShaderException(GLShaderPP::CShaderException(""s, GLShaderPP::CShaderException::ExceptionType::BadSourceStream))
  );

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
