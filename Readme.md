# GLShaderPP Readme

[![Build Status](https://gitlab-lepuy.iut.uca.fr/opengl/glshaderpp/badges/master/pipeline.svg?ignore_skipped=true)](https://gitlab-lepuy.iut.uca.fr/opengl/glshaderpp/-/pipelines)

GLShaderPP is a lightweight C++ library that simplifies GLSL shader management (compiling and linking) in OpenGL application.

[TOC]

## Quick Start

The following example shows how to use it in your client application:

First, you have to include GLShaderPP header. But to be functionnal, you have to provide OpenGL functions to compiler. So, you need to previously include your OpenGL loading library. Supposing you use [GLEW](https://github.com/nigels-com/glew) to do it, your inclusion will be something like:

``` cpp
#include <GL/glew.h>
#include <GLShaderPP/ShaderProgram.h>
```

Then, wherever you want to create a new shader program, you just have to:

``` cpp
  GLShaderPP::CShaderProgram program{ 
    GLShaderPP::CShader{ GL_VERTEX_SHADER, std::ifstream{ "vertex.vert" } },
    GLShaderPP::CShader{ GL_FRAGMENT_SHADER, std::ifstream{ "fragment.frag" } }
  };
```

_Note that the second argument of `CShader` constructor is a text stream to GLSL source code. In this example, it is a `std::ifstream` but it can be anything else that inherits from `std::istream`._

Some error mangement can help you to get human understandable information if your GLSL code is not compiling or linking. See [error management](#error-management) section for more explanation.

`GLShaderPP::CShaderProgram` class has two simple member functions to manage shader program:

- `GLuint GetProgramId() const`: Returns the OpenGL id of this shader program. With it, you can manage your shader program with OpenGL functions such as [`glGetUniformLocation`](https://www.khronos.org/registry/OpenGL-Refpages/gl4/html/glGetUniformLocation.xhtml)
- `void Use()`: Simply calls OpenGL's [`glUseProgram`](https://www.khronos.org/registry/OpenGL-Refpages/gl4/html/glUseProgram.xhtml) to activate your shader program for next OpenGL rendering.

This is the simplest way to use GLShaderPP. Finally, a typical OpenGL application can be built by following these steps:

1. Create a window with an OpenGL context (you may use [GLFW](https://www.glfw.org/) for this)
2. You may want to retreive "modern" OpenGL functions using [GLEW](https://github.com/nigels-com/glew) (for example)
3. Create your shader program with a `GLShaderPP::CShaderProgram program` object (see the last example)
4. Get your uniform locations from `glGetUniformLocation(program.GetProgramId(), "uniform_name")`
5. Prepare your VAO/VBO/EBO/Textures...
6. Activate your shader program by calling `program.Use()`
7. Set your uniforms values with OpenGL's `glUniform*` functions
8. Do rendering

## Automatic GLSL compilation and link

By using constructors with parameters, you can directly create in one line a fully functional shader program (ie. with GLSL sources, compiled and linked).

The `GLShaderPP::CShaderProgram` class has a constructor which takes a list of `GLShaderPP::CShader` objects. Each `GLShaderPP::CShader` should correspond to an OpenGL pipeline stage (vertex, geometry, fragment shader...). By using this constructor, the `GLShaderPP::CShaderProgram` is created with specified shaders attached and linked to it.

The GLShderPP::CShader class has also a convenient constructor which takes two parameters. The first one is the shader type (OpenGL shader type constant such as GL_VERTEX_SHADER and so on...) and the second is a constant `std::string` or constant `std::istream` reference to the shader GLSL source. By using this constructor, the `GLShaderPP::CShader` is automatically compiled.

So, using these constructors, you can create a totally functional shader program with the following type of syntax:

``` cpp
  GLShaderPP::CShaderProgram program{ 
    { GL_VERTEX_SHADER, std::ifstream{ "vertex.vert" } },
    { GL_FRAGMENT_SHADER, std::ifstream{ "fragment.frag" } }
  };
```

If something goes wrong during all these underlying steps, you will be warned. See [Error management](#error-management) section.

## Manually GLSL compilation and link

If you want to keep control of what and when it's done, you may create empty objects and load, compile and link later. So you have to create your `GLShaderPP::CShader` objects and call their `GLShaderPP::CShader::LoadSource()` and `GLShaderPP::CShader::Compile()` member functions. When your shader are ready, you have to create a `GLShaderPP::CShaderProgram` object and call its `GLShaderPP::CShaderProgram::AttachShader()` for each shader then call its `GLShaderPP::CShaderProgram::Link()` member function.

The following example shows all these steps:
``` cpp
  GLShaderPP::CShader vertexShader{ GL_VERTEX_SHADER };
  vertexShader.LoadSource(std::ifstream{ "vertex.vert" });
  vertexShader.Compile();
  GLShaderPP::CShader fragmentShader{ GL_FRAGMENT_SHADER };
  fragmentShader.LoadSource(std::ifstream{ "fragment.frag" });
  fragmentShader.Compile();
  GLShaderPP::CShaderProgram program;
  program.AttachShader(vertexShader);
  program.AttachShader(fragmentShader);
  program.Link();
```

If something goes wrong during all these steps, you will be warned. See [Error management](#error-management) section.

## Error management                         {#error-management}

Two error management systems are hardcoded in GLShaderPP. The first by using `std::exception` derived classes when GLShaderPP header file is defaultly included and the second with simple error codes when GLShaderPP header file is included with `_DONT_USE_SHADER_EXCEPTION` preprocessor constant defined.

### Using exceptions

All thrown exception are `GLShaderPP::CShaderException` objects. This class inherits from `std::exception`. The standard `GLShaderPP::CShaderException::what()` member explains in an human understandable way what has been wrong. Moreover, the `GLShaderPP::CShaderException::type()` gives an enumaration switchable type of the exception.

See `GLShaderPP::CShaderException::ExceptionType` in the documentation or in `ShaderException.h` file for the possible values.

### Using error codes

If GLShaderPP header file is included with `_DONT_USE_SHADER_EXCEPTION` preprocessor constant defined, errors are ignored but discreetly reported in stderr. You should check every time the status of your last action by using `GLShaderPP::CShader::GetCompileState()` or `GLShaderPP::CShaderProgramm::GetLinkingStatus()` member functions. They return an enumaration value (`GLShaderPP::CShader::ShaderCompileState` or `GLShaderPP::CShaderProgramm::LinkingStatus`). Consult the documentation or header files for the possible values.

## Compilation / Installation / Testing

GLShaderPP is a header only library. There is no need to build it to use it. However, several solution are possible to install it to your project.

### Installation

#### Using conan

#### Download release from gitlab-lepuy.iut.uca.fr

#### Automatic installation from sources

#### Manual installation from sources

You just have to take the GLShaderPP/public directory and add it to your project. 

### Building and running tests

#### Prerequisites for compiling tests

#### Compiling and running tests

### Building documentation

#### Prerequisites for making documentation

#### Making documentation

#### Reading documentation
