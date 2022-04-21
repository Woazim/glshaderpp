cmake_minimum_required (VERSION 3.16)

project("libGLShaderPP")

option(BUILD_DOCUMENTATION "Build doxygen documentation" OFF)

add_library (${PROJECT_NAME} INTERFACE)
set_property(TARGET ${PROJECT_NAME} PROPERTY PUBLIC_HEADER 
    "${CMAKE_CURRENT_SOURCE_DIR}/public/GLShaderPP/ShaderProgram.h" 
    "${CMAKE_CURRENT_SOURCE_DIR}/public/GLShaderPP/Shader.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/public/GLShaderPP/ShaderException.h")
target_include_directories(${PROJECT_NAME} INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}/public")

include(GNUInstallDirs)
install(TARGETS ${PROJECT_NAME} PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/GLShaderPP)

if(BUILD_DOCUMENTATION)
    find_package(Doxygen REQUIRED dot)
    if(NOT DOXYGEN_FOUND)
        message(FATAL_ERROR "Doxygen and/or dot is not found and is required to build documentation.")
    endif()

    set(DOXYGEN_USE_MDFILE_AS_MAINPAGE ../../Readme.md)
    set(DOXYGEN_PREDEFINED __cpp_lib_concepts)

    doxygen_add_docs(${PROJECT_NAME}doc 
        ../../Readme.md GLShaderPP/ShaderProgram.h GLShaderPP/Shader.h GLShaderPP/ShaderException.h
        ALL
        USE_STAMP_FILE
        WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/public"
    )

    install(DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/html" TYPE DOC)
endif()