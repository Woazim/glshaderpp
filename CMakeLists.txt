cmake_minimum_required (VERSION 3.9)


project ("GLShaderPP" VERSION 0.1 DESCRIPTION "A lightweight object oriented lib to compile GLSL shaders.")

option(BUILD_TESTING "Build test program" OFF)

set(CONAN_PROFILE default CACHE STRING "The conan profile you need to use to compile. See conan documentation on https://conan.io")

# Add subdirectories

add_subdirectory ("GLShaderPP")

if(BUILD_TESTING)
	enable_testing()
	add_subdirectory ("test")
endif()
