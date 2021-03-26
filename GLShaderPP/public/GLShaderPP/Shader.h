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

#ifndef EXPORT

#if defined(_MSC_VER)
 //  Microsoft 
#ifdef GLSHADERPP_STATIC
#define EXPORT
#define IMPORT
#else
#define EXPORT __declspec(dllexport)
#define IMPORT __declspec(dllimport)
#endif // _LIB
#elif defined(__GNUC__)
 //  GCC
#define EXPORT __attribute__((visibility("default")))
#define IMPORT
#else
 //  do nothing and hope for the best?
#define EXPORT
#define IMPORT
#pragma warning Unknown dynamic link import/export semantics.
#endif

#endif

#ifndef GLSHADERPP_API

#ifdef GLShaderPP_EXPORTS
#define GLSHADERPP_API EXPORT
#else
#define GLSHADERPP_API IMPORT
#endif

#endif // !GLSHADERPP_API

namespace GLShaderPP {

  /*!
   * \brief Classe de chargement et compilation d'un shader
   *
   * Cette classe permet d'encapsuler le chargement et la compilation des shader OpenGL. Les erreurs
   * de chargement et de compilation sont reportées sur le flux de sortie d'erreur, et des
   * exceptions de type CShaderException sont levées. Pour éviter les envoies d'exception, vous
   * devez définir la macro _DONT_USE_SHADER_EXCEPTION préalablement à l'inclusion du fichier
   * CShader.h
   */
  class GLSHADERPP_API CShader
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

}