#include "public/GLShaderPP/Shader.h"
#include "public/GLShaderPP/ShaderException.h"
#include <sstream>
#include <iostream>

using namespace std;

CShader::CShader(GLenum eShaderType)
  : m_eCompileState(notCompiled)
{
  m_nShaderId = glCreateShader(eShaderType);
}

CShader::CShader(GLenum eShaderType, const string& strSource)
  : m_eCompileState(notCompiled)
{
  m_nShaderId = glCreateShader(eShaderType);
  SetSource(strSource);
  Compile();
}

CShader::CShader(GLenum eShaderType, const istream& streamSource)
  : m_eCompileState(notCompiled)
{
  m_nShaderId = glCreateShader(eShaderType);
  SetSource(streamSource);
  Compile();
}

CShader::~CShader()
{
  glDeleteShader(m_nShaderId);
}

void CShader::SetSource(const string& strSource)
{
  const GLchar* vertexShaderSource = strSource.c_str(); //vertexShaderSource est correct tant que vertexShaderString n'est pas modifié

  glShaderSource(m_nShaderId, 1, &vertexShaderSource, nullptr);

  m_eCompileState = notCompiled;
}

void CShader::SetSource(const istream& streamSource)
{
  if(streamSource.good())
  { 
    stringstream vertexShaderStream;
    vertexShaderStream << streamSource.rdbuf();

    SetSource(vertexShaderStream.str());
  }
  else
  {
    string what{ "Impossible d'ouvrir les sources du " + GetType() + " shader"};
    cerr << what << endl;
#ifndef _DONT_USE_SHADER_EXCEPTION
    throw CShaderException(what, CShaderException::TypeBadSourceStream);
#endif
  }
}

void CShader::Compile()
{
  if(m_eCompileState != notCompiled)
    return;
  glCompileShader(m_nShaderId);

  GLint value;
  glGetShaderiv(m_nShaderId, GL_COMPILE_STATUS, &value);

  if(value == GL_TRUE)
    m_eCompileState = compileOk;
  else
  {
    m_eCompileState = compileError;
    GLint length = 0;
    glGetShaderiv(m_nShaderId, GL_INFO_LOG_LENGTH, &length);
    string infologbuffer;
    infologbuffer.resize(length);
    glGetShaderInfoLog(m_nShaderId, length, nullptr, &infologbuffer.front());
    string what{ "Erreur de compilation du " + GetType() + " shader\n" + infologbuffer };
    cerr << what << endl;
#ifndef _DONT_USE_SHADER_EXCEPTION
    throw CShaderException(what, CShaderException::TypeCompilationError);
#endif
  }
}

string CShader::GetType() const
{
  GLint type;
  glGetShaderiv(m_nShaderId, GL_SHADER_TYPE, &type);
  switch(type)
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