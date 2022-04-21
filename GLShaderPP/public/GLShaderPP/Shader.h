/*!
 * \file Shader.h
 * Ce fichier contient la déclaration de la classe CShader
 * 
 * Copyright (c) 2015 by Benjamin ALBOUY-KISSI
 */
#pragma once
#include <string>
#include <istream>
#include <sstream>
#include "ShaderException.h"

namespace GLShaderPP {

#ifdef GLEW_VERSION
  inline void GlewInit() {
    glewExperimental = GL_TRUE;
    GLenum err;
    if ((err = ::glewInit()) != GLEW_OK)    /* Problem: glewInit failed, something is seriously wrong. */
      throw CShaderException(std::string("Error: ") + (char*)glewGetErrorString(err), CShaderException::ExceptionType::GlewInit);
  };
#endif

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
    enum class ShaderCompileState
    {
      notCompiled,
      compileError,
      compileOk
    };
  private:
    ShaderCompileState m_eCompileState = ShaderCompileState::notCompiled;
    GLuint m_nShaderId;

    //rend cette classe non copiable
    CShader(const CShader&) = delete;
    CShader& operator=(const CShader&) = delete;
    void createShader(GLenum eShaderType) {
      if (!glCreateShader)
#ifdef GLEW_VERSION
        GlewInit();
      if (!glCreateShader)
#endif
        throw CShaderException("Error: OpenGL context seems not to be properly initialised.", CShaderException::ExceptionType::GlewInit);
      m_nShaderId = glCreateShader(eShaderType);
    }

  public:
    CShader(GLenum eShaderType) { createShader(eShaderType); }
    CShader(GLenum eShaderType, const std::string& strSource) { 
      createShader(eShaderType);
      SetSource(strSource);
      Compile();
    }
    CShader(GLenum eShaderType, const std::istream& streamSource) { 
      createShader(eShaderType);
      SetSource(streamSource);
      Compile();
    }
    ~CShader() { glDeleteShader(m_nShaderId); }

    void SetSource(const std::string& strSource)
    {
      const GLchar* vertexShaderSource = strSource.c_str();
      glShaderSource(m_nShaderId, 1, &vertexShaderSource, nullptr);
      m_eCompileState = ShaderCompileState::notCompiled;
    }

    void SetSource(const std::istream& streamSource)
    {
      if (streamSource.good())
      {
        std::stringstream vertexShaderStream;
        vertexShaderStream << streamSource.rdbuf();

        SetSource(vertexShaderStream.str());
      }
      else
      {
        std::string what{ "Can not open " + GetType() + " shader sources" };
#ifndef _DONT_USE_SHADER_EXCEPTION
        throw CShaderException(what, CShaderException::ExceptionType::BadSourceStream);
#else
        std::cerr << what << '\n';
#endif
      }
    }

    void Compile()
    {
      if (m_eCompileState != ShaderCompileState::notCompiled)
        return;
      glCompileShader(m_nShaderId);

      GLint value;
      glGetShaderiv(m_nShaderId, GL_COMPILE_STATUS, &value);

      if (value == GL_TRUE)
        m_eCompileState = ShaderCompileState::compileOk;
      else
      {
        m_eCompileState = ShaderCompileState::compileError;
        GLint length = 0;
        glGetShaderiv(m_nShaderId, GL_INFO_LOG_LENGTH, &length);
        std::string infologbuffer;
        infologbuffer.resize(length);
        glGetShaderInfoLog(m_nShaderId, length, nullptr, &infologbuffer.front());
        std::string what{ "An error occured during " + GetType() + " shader compilation\n" + infologbuffer };
#ifndef _DONT_USE_SHADER_EXCEPTION
        throw CShaderException(what, CShaderException::ExceptionType::CompilationError);
#else
        std::cerr << what << '\n';
#endif
      }
    }

    std::string GetType() const
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
        return "unknown";
      }
    }

  public:
    ShaderCompileState GetCompileState() const { return m_eCompileState; }
    GLuint GetShaderId() const { return m_nShaderId; }
  };

}