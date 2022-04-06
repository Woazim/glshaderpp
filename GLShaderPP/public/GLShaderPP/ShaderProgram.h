#pragma once
#include "Shader.h"
#ifdef __cpp_lib_concepts
#include <concepts>
#endif

namespace GLShaderPP {

#ifdef __cpp_lib_concepts
  template<typename T>
  concept Shader = std::derived_from<T, CShader>;
#else
#define Shader class
#endif

  class GLSHADERPP_API CShaderProgram
  {
  public:
    enum class LinkingStatus
    {
      notLinked,
      linkingError,
      linkingOk
    };
  private:
    LinkingStatus m_eLinkingStatus = LinkingStatus::notLinked;
    GLuint m_nProgram;

    //rend cette classe non copiable
    CShaderProgram(const CShaderProgram&) = delete;
    CShaderProgram& operator=(const CShaderProgram&) = delete;

  public:
    template<Shader... S>
    CShaderProgram(const S&... shaders);

    CShaderProgram();
    ~CShaderProgram();

    LinkingStatus GetLinkingStatus() const { return m_eLinkingStatus; }
    GLuint GetProgramId() const { return m_nProgram; }
    void Use() { glUseProgram(m_nProgram); }
    void AttachShader(const CShader& s);
    CShaderProgram& operator<<(const CShader& s) { AttachShader(s); return *this; }
    void Link();

  private:
    void VerifLinking();

  };

  template<Shader... S>
  CShaderProgram::CShaderProgram(const S&... shaders)
  {
    m_nProgram = glCreateProgram();
    ((*this) << ... << shaders);
    Link();
  }
}