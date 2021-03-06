/*!
 * \file Shader.h
 * Ce fichier contient la déclaration de la classe CShader
 * 
 * Copyright (c) 2015 by Benjamin ALBOUY-KISSI
 */
#pragma once
#include <GL/glew.h>
#include <string>
#include <istream>

/*!
 * \brief Classe de chargement et compilation d'un shader
 *
 * Cette classe permet d'encapsuler le chargement et la compilation des shader OpenGL. Les erreurs 
 * de chargement et de compilation sont reportées sur le flux de sortie d'erreur, et des 
 * exceptions de type CShaderException sont levées. Pour éviter les envoies d'exception, vous 
 * devez définir la macro _DONT_USE_SHADER_EXCEPTION préalablement à l'inclusion du fichier 
 * CShader.h
 */
class CShader
{
public:
  enum ShaderCompileState
  {
    notCompiled,
    compileError,
    compileOk
  };
private:
  ShaderCompileState m_eCompileState;
  GLuint m_nShaderId;

  //rend cette classe non copiable
  CShader(const CShader&) = delete;
  CShader& operator=(const CShader&) = delete;

public:
  CShader(GLenum eShaderType);
  CShader(GLenum eShaderType, const std::string& strSource);
  CShader(GLenum eShaderType, const std::istream& streamSource);
  ~CShader();
  void SetSource(const std::string& strSource);
  void SetSource(const std::istream& streamSource);
  void Compile();
  std::string GetType() const;
public:
  ShaderCompileState GetCompileState() const { return m_eCompileState; }
  GLuint GetShaderId() const { return m_nShaderId; }
};

