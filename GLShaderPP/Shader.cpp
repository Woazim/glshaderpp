#include "public/GLShaderPP/Shader.h"
#include "public/GLShaderPP/ShaderException.h"
#include <sstream>
#include <iostream>

namespace GLShaderPP {

  class CGlewInit {
    static bool m_bGlewInit;
  public:
    CGlewInit() {
      if (!m_bGlewInit)
      {
        glewExperimental = GL_TRUE;
        GLenum err;
        if ((err = ::glewInit()) != GLEW_OK)    /* Problem: glewInit failed, something is seriously wrong. */
          throw CShaderException(std::string("Error: ") + (char*)glewGetErrorString(err), CShaderException::ExceptionType::GlewInit);
        m_bGlewInit = true;
      }
    }
  };

  bool CGlewInit::m_bGlewInit = false;

  CShader::CShader(GLenum eShaderType)
    : m_eCompileState(notCompiled)
  {
    CGlewInit();
    m_nShaderId = glCreateShader(eShaderType);
  }

  CShader::CShader(GLenum eShaderType, const std::string& strSource)
    : m_eCompileState(notCompiled)
  {
    CGlewInit();
    m_nShaderId = glCreateShader(eShaderType);
    SetSource(strSource);
    Compile();
  }

  CShader::CShader(GLenum eShaderType, const std::istream& streamSource)
    : m_eCompileState(notCompiled)
  {
    CGlewInit();
    m_nShaderId = glCreateShader(eShaderType);
    SetSource(streamSource);
    Compile();
  }

  CShader::~CShader()
  {
    glDeleteShader(m_nShaderId);
  }

  void CShader::SetSource(const std::string& strSource)
  {
    const GLchar* vertexShaderSource = strSource.c_str(); //vertexShaderSource est correct tant que vertexShaderString n'est pas modifié

    glShaderSource(m_nShaderId, 1, &vertexShaderSource, nullptr);

    m_eCompileState = notCompiled;
  }

  void CShader::SetSource(const std::istream& streamSource)
  {
    if (streamSource.good())
    {
      std::stringstream vertexShaderStream;
      vertexShaderStream << streamSource.rdbuf();

      SetSource(vertexShaderStream.str());
    }
    else
    {
      std::string what{ "Impossible d'ouvrir les sources du " + GetType() + " shader" };
      std::cerr << what << '\n';
#ifndef _DONT_USE_SHADER_EXCEPTION
      throw CShaderException(what, CShaderException::ExceptionType::BadSourceStream);
#endif
    }
  }

  void CShader::Compile()
  {
    if (m_eCompileState != notCompiled)
      return;
    glCompileShader(m_nShaderId);

    GLint value;
    glGetShaderiv(m_nShaderId, GL_COMPILE_STATUS, &value);

    if (value == GL_TRUE)
      m_eCompileState = compileOk;
    else
    {
      m_eCompileState = compileError;
      GLint length = 0;
      glGetShaderiv(m_nShaderId, GL_INFO_LOG_LENGTH, &length);
      std::string infologbuffer;
      infologbuffer.resize(length);
      glGetShaderInfoLog(m_nShaderId, length, nullptr, &infologbuffer.front());
      std::string what{ "Erreur de compilation du " + GetType() + " shader\n" + infologbuffer };
      std::cerr << what << '\n';
#ifndef _DONT_USE_SHADER_EXCEPTION
      throw CShaderException(what, CShaderException::ExceptionType::CompilationError);
#endif
    }
  }

  std::string CShader::GetType() const
  {
    GLint type;
    glGetShaderiv(m_nShaderId, GL_SHADER_TYPE, &type);
    switch (type)
    {
    case GL_COMPUTE_SHADER:
      return "compute";
    case GL_VERTEX_SHADER:
      return "vertex";
    case GL_TESS_CONTROL_SHADER:
      return "tesselation control";
    case GL_TESS_EVALUATION_SHADER:
      return "tesselation evaluation";
    case GL_GEOMETRY_SHADER:
      return "geometry";
    case GL_FRAGMENT_SHADER:
      return "fragment";
    default:
      return "inconnu";
    }
  }

}