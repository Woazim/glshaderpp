#include "public/GLShaderPP/ShaderProgram.h"
#include "public/GLShaderPP/ShaderException.h"
#include <string>
#include <iostream>

namespace GLShaderPP {

  CShaderProgram::CShaderProgram()
  {
    m_nProgram = glCreateProgram();
  }

  void CShaderProgram::AttachShader(const CShader& s)
  {
    if (s.GetCompileState() != CShader::ShaderCompileState::compileOk)
    {
      std::string what{ "Le " + s.GetType() + " shader n'est pas compilé avant d'être attaché au programme" };
#ifndef _DONT_USE_SHADER_EXCEPTION
      throw CShaderException("Le shader n'est pas compilé avant d'être attaché au programme", CShaderException::ExceptionType::PrepareLinkError);
#else
      std::cerr << what << '\n';
#endif
    }
    else
    {
      glAttachShader(m_nProgram, s.GetShaderId());
    }
  }

  void CShaderProgram::Link()
  {
    if (m_eLinkingStatus != LinkingStatus::linkingOk)
    {
      glLinkProgram(m_nProgram);
      VerifLinking();
    }
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
      m_eLinkingStatus = LinkingStatus::linkingOk;
    else
    {
      m_eLinkingStatus = LinkingStatus::linkingError;
      GLint length = 0;
      glGetProgramiv(m_nProgram, GL_INFO_LOG_LENGTH, &length);
      std::string infologbuffer;
      infologbuffer.resize(length);
      glGetProgramInfoLog(m_nProgram, length, nullptr, &infologbuffer.front());
      std::string what{ "Erreur lors de l'édition des liens du programme shader\n" + infologbuffer };
#ifndef _DONT_USE_SHADER_EXCEPTION
      throw CShaderException(what, CShaderException::ExceptionType::LinkError);
#else
      std::cerr << what << '\n';
#endif
    }
  }

}