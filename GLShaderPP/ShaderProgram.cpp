#include "public/GLShaderPP/ShaderProgram.h"
#include "public/GLShaderPP/ShaderException.h"
#include <string>
#include <iostream>

namespace GLShaderPP {

  CShaderProgram::CShaderProgram()
    : m_eLinkingStatus{ notLinked }
  {
    m_nProgram = glCreateProgram();
  }

  void CShaderProgram::AttachShader(const CShader& s)
  {
    if (s.GetCompileState() != CShader::compileOk)
    {
      std::string what{ "Le " + s.GetType() + " shader n'est pas compilé avant d'être attaché au programme" };
      std::cerr << what << '\n';
#ifndef _DONT_USE_SHADER_EXCEPTION
      throw CShaderException("Le shader n'est pas compilé avant d'être attaché au programme", CShaderException::ExceptionType::PrepareLinkError);
#endif
    }
    else
    {
      glAttachShader(m_nProgram, s.GetShaderId());
      m_eLinkingStatus = notLinked;
    }
  }

  void CShaderProgram::Link()
  {
    if (m_eLinkingStatus != linkingOk)
    {
      glLinkProgram(m_nProgram);
      VerifLinking();
    }
  }

  CShaderProgram::CShaderProgram(const CShader& s1, const CShader& s2, const CShader& s3, const CShader& s4, const CShader& s5)
    : m_eLinkingStatus{ notLinked }
  {
    m_nProgram = glCreateProgram();
    AttachShader(s1);
    AttachShader(s2);
    AttachShader(s3);
    AttachShader(s4);
    AttachShader(s5);
    Link();
  }

  CShaderProgram::CShaderProgram(const CShader& s1, const CShader& s2, const CShader& s3, const CShader& s4)
    : m_eLinkingStatus{ notLinked }
  {
    m_nProgram = glCreateProgram();
    AttachShader(s1);
    AttachShader(s2);
    AttachShader(s3);
    AttachShader(s4);
    Link();
  }

  CShaderProgram::CShaderProgram(const CShader& s1, const CShader& s2, const CShader& s3)
    : m_eLinkingStatus{ notLinked }
  {
    m_nProgram = glCreateProgram();
    AttachShader(s1);
    AttachShader(s2);
    AttachShader(s3);
    Link();
  }

  CShaderProgram::CShaderProgram(const CShader& s1, const CShader& s2)
    : m_eLinkingStatus{ notLinked }
  {
    m_nProgram = glCreateProgram();
    AttachShader(s1);
    AttachShader(s2);
    Link();
  }

  CShaderProgram::CShaderProgram(const CShader& s1)
    : m_eLinkingStatus{ notLinked }
  {
    m_nProgram = glCreateProgram();
    AttachShader(s1);
    Link();
  }

  CShaderProgram::~CShaderProgram()
  {
    glDeleteProgram(m_nProgram);
  }

  void CShaderProgram::VerifLinking()
  {
    GLint value;
    glGetProgramiv(m_nProgram, GL_LINK_STATUS, &value);
    if (value == GL_TRUE)
      m_eLinkingStatus = linkingOk;
    else
    {
      m_eLinkingStatus = linkingError;
      GLint length = 0;
      glGetProgramiv(m_nProgram, GL_INFO_LOG_LENGTH, &length);
      std::string infologbuffer;
      infologbuffer.resize(length);
      glGetProgramInfoLog(m_nProgram, length, nullptr, &infologbuffer.front());
      std::string what{ "Erreur lors de l'édition des liens du programme shader\n" + infologbuffer };
      std::cerr << what << '\n';
#ifndef _DONT_USE_SHADER_EXCEPTION
      throw CShaderException(what, CShaderException::ExceptionType::LinkError);
#endif
    }
  }

}