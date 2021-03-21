#pragma once
#include <stdexcept>
#include <string>

class CShaderException :
  public std::exception
{
public:
  enum class ExceptionType
  {
    BadSourceStream,
    CompilationError,
    PrepareLinkError,
    LinkError,
    GlewInit
  };
private:
  ExceptionType m_eType;
  std::string m_strWhat;

public:
  CShaderException(const std::string& strWhat, ExceptionType eType) : m_strWhat(strWhat), m_eType(eType) {};
  const char* what() const noexcept { return m_strWhat.c_str(); }
  ExceptionType type() const { return m_eType; }
};

