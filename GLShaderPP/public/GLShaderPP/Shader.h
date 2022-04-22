/*****************************************************************//**
 * \file      Shader.h
 * \brief     Declaration of CShader class
 * 
 * \author    Benjamin ALBOUY-KISSI
 * \date      2022
 * \copyright GNU Lesser Public License v3
 *********************************************************************/
#pragma once
#include <string>
#include <istream>
#include <sstream>
#include "ShaderException.h"

namespace GLShaderPP {

#ifdef GLEW_VERSION
  /**
   * \brief Initialise GLEW
   * 
   * This function is called if you included \c glew.h and if it seems that GLEW has not been initialised
   * (OpenGL functions are null pointers). It tries to init GLEW by calling \c glewInit() and if it fails it 
   * will throw a CShaderException::ExceptionType::GlewInit typed CShaderException.
   * 
   * \throw CShaderException A CShaderException::ExceptionType::GlewInit typed CShaderException if \c glewInit() fails.
   */
  inline void GlewInit() {
    glewExperimental = GL_TRUE;
    GLenum err;
    if ((err = ::glewInit()) != GLEW_OK)    /* Problem: glewInit failed, something is seriously wrong. */
      throw CShaderException(std::string("Error: ") + (char*)glewGetErrorString(err), CShaderException::ExceptionType::GlewInit);
  };
#endif

  /*!
   * \brief Loads and compiles an OpenGL shader
   *
   * This class encapsulates the loading and compilation of OpenGL shaders. If #_DONT_USE_SHADER_EXCEPTION
   * is defined before including this file, loading and compilation errors are reported to the error output
   * stream. Otherwise, CShaderException objects are thrown. 
   * 
   * To use this class, you can automatically load and compile a shader by constructing a CShader with
   * CShader(GLenum eShaderType, const std::string& strSource) or CShader(GLenum eShaderType, const std::istream& streamSource)
   * 
   * Alternatively, you can create an empty CShader object with CShader(GLenum eShaderType). Then, call one of the SetSource() 
   * functions followed by a call to Compile()
   */
  class CShader
  {
  public:
    /**
     * Enumaration of possible states of shader compilation.
     */
    enum class ShaderCompileState
    {
      notCompiled,      //!< Compilation has not been tried.
      badSourceStream,  //!< The source stream is not readable
      compileError,     //!< An error occured during compilation.
      compileOk         //!< Compilation is Ok.
    };
  private:

    ShaderCompileState m_eCompileState = ShaderCompileState::notCompiled; //!< State of the shader compilation
    GLuint m_nShaderId; //!< Identifier of the underlying OpenGL shader object.

    CShader(const CShader&) = delete;
    CShader& operator=(const CShader&) = delete;

    /**
     * \brief Create the underlying OpenGL shader object.
     * 
     * \param eShaderType OpenGL type of this shader.
     */
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
    /**
     * \brief Creates an empty shader object.
     * 
     * \param eShaderType OpenGL type of this shader.
     * 
     * \see <a href="https://www.khronos.org/registry/OpenGL-Refpages/gl4/html/glCreateShader.xhtml">OpenGL's glCreateShader()</a> for \c eShaderType possible values
     */
    CShader(GLenum eShaderType) { createShader(eShaderType); }

    /**
     * \brief Creates an shader object from string source.
     *
     * This constructor creates the shader, sets its source code from a string then compiles it.
     * 
     * \param eShaderType OpenGL type of this shader.
     * \param strSource The string of the GLSL source code of the shader.
     *
     * \see <a href="https://www.khronos.org/registry/OpenGL-Refpages/gl4/html/glCreateShader.xhtml">OpenGL's glCreateShader()</a> for \c eShaderType possible values
     */
    CShader(GLenum eShaderType, const std::string& strSource) {
      createShader(eShaderType);
      SetSource(strSource);
      Compile();
    }

    /**
     * \brief Creates an shader object from string source.
     *
     * This constructor creates the shader, sets its source code from an istream then compiles it.
     *
     * \param eShaderType OpenGL type of this shader.
     * \param streamSource The stream containing the GLSL source code of the shader.
     *
     * \see <a href="https://www.khronos.org/registry/OpenGL-Refpages/gl4/html/glCreateShader.xhtml">OpenGL's glCreateShader()</a> for \c eShaderType possible values
     */
    CShader(GLenum eShaderType, const std::istream& streamSource) {
      createShader(eShaderType);
      SetSource(streamSource);
      Compile();
    }

    /**
     * \brief Deletes the underlying OpenGL shader object.
     */
    ~CShader() { glDeleteShader(m_nShaderId); }

    /**
     * \brief Sets the GLSL source code of the shader from a string.
     * 
     * \param strSource The string of the GLSL source code of the shader.
     */
    void SetSource(const std::string& strSource)
    {
      const GLchar* vertexShaderSource = strSource.c_str();
      glShaderSource(m_nShaderId, 1, &vertexShaderSource, nullptr);
      m_eCompileState = ShaderCompileState::notCompiled;
    }

    /**
     * \brief Sets the GLSL source code of the shader from an istream.
     *
     * \param streamSource The istream containing the GLSL source code of the shader.
     * 
     * \throw CShaderException If #_DONT_USE_SHADER_EXCEPTION is defined, it may throw a CShaderException::ExceptionType::BadSourceStream 
     * typed CShaderException if the source stream is not readable for any reason.
     */
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
        m_eCompileState = ShaderCompileState::badSourceStream;
        std::string what{ "Can not open " + GetType() + " shader sources" };
#ifndef _DONT_USE_SHADER_EXCEPTION
        throw CShaderException(what, CShaderException::ExceptionType::BadSourceStream);
#else
        std::cerr << what << '\n';
#endif
      }
    }

    /**
     * \brief Compiles the GLSL source code of this shader.
     * 
     * \throw CShaderException If #_DONT_USE_SHADER_EXCEPTION is defined, it may throw a CShaderException::ExceptionType::CompilationError
     * typed CShaderException if the compilation fails.
     */
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

    /**
     * \brief Returns a string representation of this shader type.
     * 
     * \return Possible values are:
     * - "compute"
     * - "vertex"
     * - "tesselation control"
     * - "tesselation evaluation"
     * - "geometry"
     * - "fragment"
     * - "unknown"
     */
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

    /**
     * \brief Gets the state of this shader compilation.
     */
    ShaderCompileState GetCompileState() const { return m_eCompileState; }

    /**
     * \brief Gets the identifier of the underlying OpenGL shader object.
     */
    GLuint GetShaderId() const { return m_nShaderId; }
  };

}