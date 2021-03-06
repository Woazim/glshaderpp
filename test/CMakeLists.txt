cmake_minimum_required (VERSION 3.8)

project("testProg")

add_executable (testProg)
file(GLOB testProg_SRC "*.h" "*.cpp")
target_sources(testProg PRIVATE ${testProg_SRC})

if(STATIC_LIB)
    add_compile_definitions(GLSHADERPP_STATIC)
endif()

# set_property(TARGET testProg PROPERTY CXX_STANDARD 20)

if(TARGET CONAN_PKG::catch2)
    target_link_libraries(testProg CONAN_PKG::catch2)
endif()
if(TARGET CONAN_PKG::glm)
    target_link_libraries(testProg CONAN_PKG::glm)
endif()
if(TARGET CONAN_PKG::glfw)
    target_link_libraries(testProg CONAN_PKG::glfw)
endif()
if(TARGET CONAN_PKG::glew)
    target_link_libraries(testProg CONAN_PKG::glew)
endif()

target_link_libraries(testProg libGLShaderPP)

# copy dataset files
file(GLOB shaderFiles "shaders/*.*")
add_custom_command(TARGET testProg POST_BUILD
                   COMMAND ${CMAKE_COMMAND} -E copy_if_different 
                       ${shaderFiles}
                           $<TARGET_FILE_DIR:testProg>)
# copy dynamic linked library
if(NOT STATIC_LIB)
    add_custom_command(TARGET testProg POST_BUILD
                       COMMAND ${CMAKE_COMMAND} -E copy_if_different 
                           $<TARGET_FILE:libGLShaderPP>
                               $<TARGET_FILE_DIR:testProg>)
endif()

set_property(TARGET testProg PROPERTY VS_DEBUGGER_WORKING_DIRECTORY $<TARGET_FILE_DIR:testProg>)

# add_test(NAME Reading_basi0g01.png COMMAND testProg [basi0g01] WORKING_DIRECTORY $<TARGET_FILE_DIR:testProg>)
