cmake_minimum_required (VERSION 3.8)

project("libGLShaderPP")

if(STATIC_LIB)
    add_library (libGLShaderPP STATIC)
    add_compile_definitions(GLSHADERPP_STATIC)
else()
    add_library (libGLShaderPP SHARED)
endif()
set_property(TARGET libGLShaderPP PROPERTY OUTPUT_NAME GLShaderPP)
set_property(TARGET libGLShaderPP PROPERTY PUBLIC_HEADER 
    "${CMAKE_CURRENT_SOURCE_DIR}/public/GLShaderPP/ShaderProgram.h" 
    "${CMAKE_CURRENT_SOURCE_DIR}/public/GLShaderPP/Shader.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/public/GLShaderPP/ShaderException.h")
target_include_directories(libGLShaderPP INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}/public")

if(TARGET CONAN_PKG::glew)
    target_link_libraries (libGLShaderPP CONAN_PKG::glew)
endif()

target_sources(libGLShaderPP PRIVATE Shader.cpp ShaderProgram.cpp)

add_compile_definitions(GLShaderPP_EXPORTS)

set_property(TARGET libGLShaderPP PROPERTY CXX_STANDARD 20)
set_property(TARGET libGLShaderPP PROPERTY CXX_STANDARD_REQUIRED ON)

include(GNUInstallDirs)
install(TARGETS libGLShaderPP PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/GLShaderPP)
