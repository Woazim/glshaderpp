#ifdef WIN32
#include <Windows.h>
#endif
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <GLShaderPP/ShaderProgram.h>
using namespace std;

const float pi = 3.1415926535897932384626433832795f;

#ifdef WIN32
//This magic line is to force notebook computer that share NVidia and Intel graphics to use high performance GPU (NVidia).
//Intel graphics doesn't support OpenGL > 1.2
extern "C" _declspec(dllexport) unsigned long NvOptimusEnablement = 0x00000001;
#endif

const char* GetGLErrorString();
void terminate(const string& strErr);
void error_callback(int error, const char* description)
{
  cerr << "GLFW3 Error #" << error << " : " << description << endl;
}

int main(void)
{
#ifdef WIN32
  SetConsoleOutputCP(65001);
#endif
  const char* pText;

  if (!glfwInit())
    exit(EXIT_FAILURE);

  glfwSetErrorCallback(error_callback);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
  GLFWwindow* pWnd = glfwCreateWindow(800, 600, "Hello World", nullptr, nullptr);
  if (!pWnd)
    terminate("Impossible de créer la fenêtre !");

  glfwMakeContextCurrent(pWnd);

  glewExperimental = GL_TRUE;
  GLenum err;
  if((err = glewInit()) != GLEW_OK)    /* Problem: glewInit failed, something is seriously wrong. */
    terminate(string("Error: ") + (char*)glewGetErrorString(err));
  
  pText = (char*)glGetString(GL_VENDOR);
  if (!pText) pText = GetGLErrorString();
  cout << "Company : " << pText << endl;

  pText = (char*)glGetString(GL_RENDERER);
  if (!pText) pText = GetGLErrorString();
  cout << "Renderer : " << pText << endl;

  pText = (char*)glGetString(GL_VERSION);
  if (!pText) pText = GetGLErrorString();
  cout << "Version : " << pText << endl;

  CShaderProgram program{ 
    CShader{ GL_VERTEX_SHADER, ifstream{ "vertex.vert" } },
    CShader{ GL_FRAGMENT_SHADER, ifstream{ "fragment.frag" } }
  };

  //// Equivalent à :
  //CShader vertexShader(GL_VERTEX_SHADER);
  //vertexShader.SetSource(ifstream{ "vertex.vert" });
  //vertexShader.Compile();
  //CShader fragmentShader(GL_FRAGMENT_SHADER);
  //fragmentShader.SetSource(ifstream{ "fragment.frag" });
  //fragmentShader.Compile();
  //CShaderProgram program;
  //program.AttachShader(vertexShader);
  //program.AttachShader(fragmentShader);
  //program.Link();


  if(program.GetLinkingStatus() != CShaderProgram::linkingOk)
    terminate("");
  program.Use();

  GLfloat vertices[] = {
    //Positions        //Couleurs       //Phase
    -0.5f, -0.5f,   1.0f, 0.0f, 0.0f,     0.0f,         //Sommet 1
     0.5f, -0.5f,   0.0f, 1.0f, 0.0f,     2.0f*pi/3.0f, //Sommet 2
     0.0f,  0.5f,   0.0f, 0.0f, 1.0f,     4.0f*pi/3.0f  //Sommet 3
  };
  //Spécification des vertices pour ce programme shader
  // Le VAO
  GLuint VAO;
  glGenVertexArrays(1, &VAO);
  glBindVertexArray(VAO);

  // Le VBO
  GLuint VBO;
  glGenBuffers(1, &VBO);
  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

  //spécifie l'organisation de l'entrée du vertex shader
  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), reinterpret_cast<GLvoid*>(0));
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), reinterpret_cast<GLvoid*>(2 * sizeof(GLfloat)));
  glEnableVertexAttribArray(1);
  glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), reinterpret_cast<GLvoid*>(5 * sizeof(GLfloat)));
  glEnableVertexAttribArray(2);


  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  
  //Retrouve la position de l'uniform fTime dans le programme en cours
  GLint nUniform_fTime = glGetUniformLocation(program.GetProgramId(), "fTime");
  
  glfwSetTime(0);
  while(!glfwWindowShouldClose(pWnd))
  {
    glfwPollEvents();

    glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    glUniform1f(nUniform_fTime, (GLfloat)glfwGetTime());
    glDrawArrays(GL_TRIANGLES, 0, 3);

    glfwSwapBuffers(pWnd);
  }

  glfwTerminate();

  return 0;
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
    //case GL_INVALID_FRAMEBUFFER_OPERATION?:
    //	return "GL_INVALID_FRAMEBUFFER_OPERATION???";
    //case GL_TABLE_TOO_LARGE?:
    //	return "GL_TABLE_TOO_LARGE???";
  }
  return "Erreur inconnue";
}

void terminate(const string& strErr)
{
  cerr << strErr << endl;
  glfwTerminate();
  exit(EXIT_FAILURE);
}