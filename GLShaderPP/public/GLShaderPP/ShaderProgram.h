/*****************************************************************//**
 * \file      ShaderProgram.h
 * \brief     Declaration of CShaderProgram class
 * 
 * \author    Benjamin ALBOUY-KISSI
 * \date      2022
 * \copyright GNU Lesser Public License v3
 *********************************************************************/

#pragma once
#include "Shader.h"
#ifdef __cpp_lib_concepts
#include <concepts>
#endif

namespace GLShaderPP {

  /**
   * \brief Shader concept.
   * 
   * If C++20 is supported, Shader is a concept which ensures that CShaderProgram::CShaderProgram
   * parameters are subclasses of CShader. Otherwise, it's simply a synonym of \c class
   */
#ifdef __cpp_lib_concepts
  template<typename T>
  concept Shader = std::derived_from<T, CShader>;
#else
#define Shader class
#endif

  /**
   * \brief An OpenGL shader program.
   * 
   * This class represents an OpenGL shader program. The principle is to attach 
   * compiled CShader objects to it (by AttachShader() or operator<<()), then to
   * Link() the program. If everything is OK, you can then Use() your shader program 
   * in your rendering.
   * 
   * Attachements and linking can be done directly at construction by using templated 
   * constructor CShaderProgram::CShaderProgram(Shader... S)
   * 
   * If something is going wrong during attachement or linking, a CShaderException will 
   * be thrown except if you defined #_DONT_USE_SHADER_EXCEPTION before including this file. 
   * In this case, you have to GetLinkingStatus() to know if everything is all right.
   * 
   * \see CShader, CShaderException
   */
  class CShaderProgram
  {
  public:
    //!\brief The status of the linking process
    enum class LinkingStatus
    {
      notLinked,        //!< The shader program has not been linked yet
      linkingError,     //!< An error occured during the linking attempt
      prepareLinkError, //!< A non compiled shader has been attached
      linkingOk         //!< The link has been correctly done
    };

  private:
    LinkingStatus m_eLinkingStatus = LinkingStatus::notLinked; //!< The status of the linking process of this shader program
    GLuint m_nProgram; //!< The OpenGL object identifier of this shader program 

    CShaderProgram(const CShaderProgram&) = delete;
    CShaderProgram& operator=(const CShaderProgram&) = delete;

  public:
    /**
     * \brief Automatically attaches and links shaders
     * 
     * This constructor take a list of CShader objects as parameters. Each CShader is attached to this
     * constructed program shader, then the shader program is linked. CShader objects must be compiled before
     * being passed to this constructor
     * 
     * \tparam S must be CShader class or one of its derivative. Must respect the GLShaderPP::Shader concept.
     * 
     * \param shaders must be CShader objects to be attached and linked into this shader program.
     */
    template<Shader... S>
    CShaderProgram(const S&... shaders);

    /**
     * \brief Simply creates an empty shader program.
     * 
     * After creating a CShaderProgram this way, you have to AttachShader()s to it then Link() it.
     * 
     * \throw If \c glCreateProgram is \c nullptr, a CShaderException::ExceptionType::GlewInit typed CShaderException 
     * is thrown. It may append when your OpenGL function loader is not initialised. This exception is thrown whenever
     * #_DONT_USE_SHADER_EXCEPTION is defined or not.
     */
    CShaderProgram()
    {
      if (!glCreateProgram)
#ifdef GLEW_VERSION
        GlewInit();
      if (!glCreateProgram)
#endif
        throw CShaderException("Error: OpenGL context seems not to be properly initialised.", CShaderException::ExceptionType::GlewInit);
      m_nProgram = glCreateProgram();
    }

    /**
     * \brief Delete underlying OpenGL shader program object.
     */
    ~CShaderProgram() {
      glDeleteProgram(m_nProgram);
    }

    /**
     * \brief Returns the status of the linking process.
     */
    LinkingStatus GetLinkingStatus() const { return m_eLinkingStatus; }

    /**
     * \brief Returns the OpenGL object identifier of this shader program.
     */
    GLuint GetProgramId() const { return m_nProgram; }

    /**
     * Enable this shader program by calling \c glUseProgram().
     */
    void Use() { glUseProgram(m_nProgram); }

    /**
     * \brief Attaches a shader stage to this shader program.
     * 
     * \param s The CShader object to attach. Note that \c s must be previously compiled.
     * 
     * \note A possibly more convenient way to do the same task is to use operator<<().
     * 
     * \throw A CShaderException::ExceptionType::PrepareLinkError typed CShaderException if
     * \c s is not compiled and #_DONT_USE_SHADER_EXCEPTION is not defined. If #_DONT_USE_SHADER_EXCEPTION
     * is defined, a message is displayed in stderr.
     */
    void AttachShader(const CShader& s) {
      if (m_eLinkingStatus != LinkingStatus::notLinked)
        return;
      if (s.GetCompileState() != CShader::ShaderCompileState::compileOk)
      {
        m_eLinkingStatus = LinkingStatus::prepareLinkError;
        std::string what{ s.GetType() + " shader has not been compiled before being attached to program" };
#ifndef _DONT_USE_SHADER_EXCEPTION
        throw CShaderException(what, CShaderException::ExceptionType::PrepareLinkError);
#else
        std::cerr << what << '\n';
#endif
      }
      else
      {
        glAttachShader(m_nProgram, s.GetShaderId());
      }
    }

    /**
     * \brief Attaches a shader stage to this shader program.
     * 
     * This operator is a convenient shortcut to AttachShader()
     * 
     * \param s The CShader object to attach. Note that \c s must be previously compiled.
     * \return A reference to this shader program object.
     */
    CShaderProgram& operator<<(const CShader& s) { AttachShader(s); return *this; }

    /**
     * \brief Links this shader program.
     *
     * \throw A CShaderException::ExceptionType::LinkError typed CShaderException if link is not possible
     * and #_DONT_USE_SHADER_EXCEPTION is not defined. If #_DONT_USE_SHADER_EXCEPTION is defined, 
     * a message is displayed in stderr.
     */
    void Link() {
      if (m_eLinkingStatus == LinkingStatus::notLinked)
      {
        glLinkProgram(m_nProgram);
        VerifLinking();
      }
    }


  private:
    /**
     * \brief Checks the state of linking.
     * 
     * If an error occured during linking, according to #_DONT_USE_SHADER_EXCEPTION definition, 
     * a CShaderException containing the GLSL linker error message is thrown or this message 
     * is displayed on \c stderr.
     */
    void VerifLinking()
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
        std::string what{ "An error occured during program linking\n" + infologbuffer };
#ifndef _DONT_USE_SHADER_EXCEPTION
        throw CShaderException(what, CShaderException::ExceptionType::LinkError);
#else
        std::cerr << what << '\n';
#endif
      }
    }


  };

  template<Shader... S>
  CShaderProgram::CShaderProgram(const S&... shaders)
  {
    if (!glCreateProgram)
#ifdef GLEW_VERSION
      GlewInit();
    if (!glCreateProgram)
#endif
      throw CShaderException("Error: OpenGL context seems not to be properly initialised.", CShaderException::ExceptionType::GlewInit);
    m_nProgram = glCreateProgram();
    ((*this) << ... << shaders);
    Link();
  }
}