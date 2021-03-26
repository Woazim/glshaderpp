# GLShaderPP Readme

[![Build Status](https://travis-ci.org/Woazim/GLShaderPP.svg?branch=master)](https://travis-ci.org/Woazim/GLShaderPP)

## About

GLShaderPP is a lightweight C++ library that simplifies GLSL shader management (compiling and linking) in OpenGL application.

The following example shows how to use it in your client application:

First, you have to include GLShaderPP header:

``` cpp
#include <GLShaderPP/ShaderProgram.h>
```

Then, wherever you want to create a new shader program, you just have to:

``` cpp
  GLShaderPP::CShaderProgram program{ 
    GLShaderPP::CShader{ GL_VERTEX_SHADER, std::ifstream{ "vertex.vert" } },
    GLShaderPP::CShader{ GL_FRAGMENT_SHADER, std::ifstream{ "fragment.frag" } }
  };
```

_Note that the second argument of `CShader` constructor is a text stream to GLSL source code. In last example, it is a `std::ifstream` but it can be anything else that inherits from `std::istream` as `std::stringstream` if you get your GLSL code from a `std::string`._

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

## Manually GLSL compilation and link

## Error management

Two error management systems are hardcoded in GLShaderPP. The first by using `std::exception` derived classes when GLShaderPP is defaultly compiled and the second with simple error codes when GLShaderPP is compiled with `_DONT_USE_SHADER_EXCEPTION` preprocessor constant defined.

### Using exceptions

### Using error codes

## Compilation / Installation / Testing

### Prerequisites

### Quick install

### Customizing compilation and installation

### Testing
