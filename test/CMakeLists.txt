cmake_minimum_required (VERSION 3.8)

project("testProg")

add_executable (testProg)
file(GLOB testProg_SRC "*.h" "*.cpp")
target_sources(testProg PRIVATE ${testProg_SRC})

if(STATIC_LIB)
    add_compile_definitions(GLSHADERPP_STATIC)
endif()

set_property(TARGET testProg PROPERTY CXX_STANDARD 20)
set_property(TARGET testProg PROPERTY CXX_STANDARD_REQUIRED ON)

if(TARGET CONAN_PKG::glm)
    target_link_libraries(testProg CONAN_PKG::glm)
endif()
if(TARGET CONAN_PKG::glfw)
    target_link_libraries(testProg CONAN_PKG::glfw)
endif()
if(TARGET CONAN_PKG::glew)
    target_link_libraries(testProg CONAN_PKG::glew)
endif()
if(TARGET CONAN_PKG::catch2)
    target_link_libraries(testProg CONAN_PKG::catch2)
endif()

target_link_libraries(testProg libGLShaderPP)

# copy shader files
file(GLOB shaderFiles "shaders/*.*")
add_custom_command(TARGET testProg POST_BUILD
                   COMMAND ${CMAKE_COMMAND} -E copy_if_different 
                       ${shaderFiles}
                           $<TARGET_FILE_DIR:testProg>)
# copy dataset files
file(GLOB datasetFiles "dataset/*.*")
add_custom_command(TARGET testProg POST_BUILD
                   COMMAND ${CMAKE_COMMAND} -E copy_if_different 
                       ${datasetFiles}
                           $<TARGET_FILE_DIR:testProg>)
# copy dynamic linked library
if(NOT STATIC_LIB)
    add_custom_command(TARGET testProg POST_BUILD
                       COMMAND ${CMAKE_COMMAND} -E copy_if_different 
                           $<TARGET_FILE:libGLShaderPP>
                               $<TARGET_FILE_DIR:testProg>)
endif()

set_property(TARGET testProg PROPERTY VS_DEBUGGER_WORKING_DIRECTORY $<TARGET_FILE_DIR:testProg>)

add_test(NAME direct-shader-from-files      COMMAND testProg [direct-shader-from-files]     WORKING_DIRECTORY $<TARGET_FILE_DIR:testProg>)
add_test(NAME shader-from-files             COMMAND testProg [shader-from-files]            WORKING_DIRECTORY $<TARGET_FILE_DIR:testProg>)
add_test(NAME direct-shader-from-strings    COMMAND testProg [direct-shader-from-strings]   WORKING_DIRECTORY $<TARGET_FILE_DIR:testProg>)
add_test(NAME direct-vertex-from-files      COMMAND testProg [direct-vertex-from-files]     WORKING_DIRECTORY $<TARGET_FILE_DIR:testProg>)
add_test(NAME direct-fragment-from-files    COMMAND testProg [direct-fragment-from-files]   WORKING_DIRECTORY $<TARGET_FILE_DIR:testProg>)
add_test(NAME vertex-from-files             COMMAND testProg [vertex-from-files]            WORKING_DIRECTORY $<TARGET_FILE_DIR:testProg>)
add_test(NAME fragment-from-files           COMMAND testProg [fragment-from-files]          WORKING_DIRECTORY $<TARGET_FILE_DIR:testProg>)
add_test(NAME direct-faulty-vertex          COMMAND testProg [direct-faulty-vertex]         WORKING_DIRECTORY $<TARGET_FILE_DIR:testProg>)
add_test(NAME faulty-vertex                 COMMAND testProg [faulty-vertex]                WORKING_DIRECTORY $<TARGET_FILE_DIR:testProg>)
add_test(NAME direct-notready-program       COMMAND testProg [direct-notready-program]      WORKING_DIRECTORY $<TARGET_FILE_DIR:testProg>)
add_test(NAME notready-program              COMMAND testProg [notready-program]             WORKING_DIRECTORY $<TARGET_FILE_DIR:testProg>)
add_test(NAME direct-faulty-program         COMMAND testProg [direct-faulty-program]        WORKING_DIRECTORY $<TARGET_FILE_DIR:testProg>)
add_test(NAME faulty-program                COMMAND testProg [faulty-program]               WORKING_DIRECTORY $<TARGET_FILE_DIR:testProg>)
add_test(NAME bad-source-stream             COMMAND testProg [bad-source-stream]            WORKING_DIRECTORY $<TARGET_FILE_DIR:testProg>)
add_test(NAME no-glewinit                   COMMAND testProg [no-glewinit]                  WORKING_DIRECTORY $<TARGET_FILE_DIR:testProg>)
