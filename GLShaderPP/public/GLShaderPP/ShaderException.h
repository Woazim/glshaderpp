/*****************************************************************//**
 * \file      ShaderException.h
 * \brief     Declaration of CShaderException class
 * 
 * \author    Benjamin ALBOUY-KISSI
 * \date      2022
 * \copyright GNU Lesser Public License v3
 *********************************************************************/
#pragma once
#include <stdexcept>
#include <string>

/**
 * \def _DONT_USE_SHADER_EXCEPTION
 * If you define it before including Shader.h or ShaderProgram.h, no exception will be thrown 
 * during shader compilation or program linking. Instead of that, errors will be discreetly 
 * reported in stderr and by changing CShader and CShaderProgram corresponding members.
 */
//For documentation purpose
#ifndef _DONT_USE_SHADER_EXCEPTION
#define _DONT_USE_SHADER_EXCEPTION
#undef _DONT_USE_SHADER_EXCEPTION
#else
#undef _DONT_USE_SHADER_EXCEPTION
#define _DONT_USE_SHADER_EXCEPTION
#endif // !_DONT_USE_SHADER_EXCEPTION


namespace GLShaderPP {

  /**
   * \brief Exception handling.
   * 
   * This class inherits from std::exception. You can use what() to get human
   * readable problem explanation or use type() to programmaticaly manage it.
   * 
   */
  class CShaderException :
    public std::exception
  {
  public:
    /**
     * \brief Enumeration of exception types
     */
    enum class ExceptionType
    {
      BadSourceStream,  //!< The shader source istream is "bad" (ie. file cannot be opened)
      CompilationError, //!< An error occured during shader compilation
      PrepareLinkError, //!< A shader stage was not compiled before linking the shader program
      LinkError,        //!< An error occured during shader program linking
      GlewInit          //!< You use GLEW to get OpenGL functions but there is no compatible active context
    };
  private:
    ExceptionType m_eType;  //!< The type of this exception
    std::string m_strWhat;  //!< The human readable message for this exception

  public:
    /**
     * \brief Constructor
     * 
     * \param strWhat A human readable message to explain what the problem is
     * \param eType   The type of this exception
     */
    CShaderException(const std::string& strWhat, ExceptionType eType) : m_strWhat(strWhat), m_eType(eType) {};

    /**
     * \brief Human readable message to explain what the problem is
     */
    const char* what() const noexcept { return m_strWhat.c_str(); }

    /**
     * \brief The type of this exception
     */
    ExceptionType type() const { return m_eType; }
  };

}