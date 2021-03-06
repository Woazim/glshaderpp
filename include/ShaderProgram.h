#pragma once
#include "Shader.h"

class CShaderProgram
{
public:
  enum LinkingStatus
  {
    notLinked,
    linkingError,
    linkingOk
  };
private:
  LinkingStatus m_eLinkingStatus;
  GLuint m_nProgram;

  //rend cette classe non copiable
  CShaderProgram(const CShaderProgram&) = delete;
  CShaderProgram& operator=(const CShaderProgram&) = delete;

public:
  CShaderProgram(const CShader& s1, const CShader& s2, const CShader& s3, const CShader& s4, const CShader& s5);
  CShaderProgram(const CShader& s1, const CShader& s2, const CShader& s3, const CShader& s4);
  CShaderProgram(const CShader& s1, const CShader& s2, const CShader& s3);
  CShaderProgram(const CShader& s1, const CShader& s2);
  CShaderProgram(const CShader& s1);
  CShaderProgram();
  ~CShaderProgram();

  LinkingStatus GetLinkingStatus() const { return m_eLinkingStatus; }
  GLuint GetProgramId() const { return m_nProgram; }
  void Use() { glUseProgram(m_nProgram); }
  void AttachShader(const CShader& s);
  void Link();

private:
  void VerifLinking();

};

