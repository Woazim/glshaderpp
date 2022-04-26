cmake_minimum_required (VERSION 3.12)

project("testProg")

add_executable (${PROJECT_NAME})
file(GLOB testProg_SRC "*.h" "*.cpp")
target_sources(${PROJECT_NAME} PRIVATE ${testProg_SRC})
target_sources(${PROJECT_NAME} PRIVATE conanfile.txt)
target_sources(${PROJECT_NAME} PRIVATE ../GLShaderPP/public/GLShaderPP/Shader.h)
target_sources(${PROJECT_NAME} PRIVATE ../GLShaderPP/public/GLShaderPP/ShaderException.h)
target_sources(${PROJECT_NAME} PRIVATE ../GLShaderPP/public/GLShaderPP/ShaderProgram.h)

#set CC and CXX environment variables to this cmake instance compiler (needed if conan needs to build some packages)
set(ENV{CC} ${CMAKE_C_COMPILER})
set(ENV{CXX} ${CMAKE_CXX_COMPILER})

#for some mystical reasons, clang sometimes use a very old stdlib (libstdc++)
if(CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -stdlib=libc++")
endif()

if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU" AND CMAKE_BUILD_TYPE STREQUAL "Debug")
    message("Code coverage enabled")
    set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
    include(CodeCoverage)
    append_coverage_compiler_flags()
    setup_target_for_coverage_gcovr_html(
        NAME ${PROJECT_NAME}_coverage 
        EXECUTABLE ctest -j ${PROCESSOR_COUNT}
        DEPENDENCIES ${PROJECT_NAME}
        BASE_DIRECTORY ${PROJECT_SOURCE_DIR}/..
        EXCLUDE "test/*"
    )
endif()

#ignore unfound PDBs during linking with MSVC compiler
if(MSVC)
    set_target_properties(${PROJECT_NAME} PROPERTIES LINK_FLAGS "/ignore:4099")
endif()

# Execute conan to install dependencies in Release and / or Debug mode
get_property(is_multi_config GLOBAL PROPERTY GENERATOR_IS_MULTI_CONFIG)
if(is_multi_config)
	message("Multi config generator")
	execute_process(COMMAND conan install "${CMAKE_CURRENT_SOURCE_DIR}" --profile=${CONAN_PROFILE} --build missing -if "${CMAKE_BINARY_DIR}" -s build_type=Debug -g cmake_multi)
	execute_process(COMMAND conan install "${CMAKE_CURRENT_SOURCE_DIR}" --profile=${CONAN_PROFILE} --build missing -if "${CMAKE_BINARY_DIR}" -s build_type=Release -g cmake_multi)
else()
	message("Build type : ${CMAKE_BUILD_TYPE}")
	execute_process(COMMAND conan install "${CMAKE_CURRENT_SOURCE_DIR}" --profile=${CONAN_PROFILE} --build missing -if "${CMAKE_BINARY_DIR}" -s build_type=${CMAKE_BUILD_TYPE})
endif()

if(EXISTS ${CMAKE_BINARY_DIR}/conanbuildinfo_multi.cmake)
    include(${CMAKE_BINARY_DIR}/conanbuildinfo_multi.cmake)
else()
    include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
endif()
conan_basic_setup(TARGETS)

set(CMAKE_DEBUG_POSTFIX d)

set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 20)
set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD_REQUIRED ON)

if(TARGET CONAN_PKG::glfw)
    target_link_libraries(${PROJECT_NAME} CONAN_PKG::glfw)
endif()
if(TARGET CONAN_PKG::glew)
    target_link_libraries(${PROJECT_NAME} CONAN_PKG::glew)
endif()
if(TARGET CONAN_PKG::catch2)
    target_link_libraries(${PROJECT_NAME} CONAN_PKG::catch2)
endif()

target_link_libraries(${PROJECT_NAME} libGLShaderPP)

# copy shader files
file(GLOB shaderFiles "shaders/*.*")
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
                   COMMAND ${CMAKE_COMMAND} -E copy_if_different 
                       ${shaderFiles}
                           $<TARGET_FILE_DIR:testProg>)
# copy dataset files
file(GLOB datasetFiles "dataset/*.*")
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
                   COMMAND ${CMAKE_COMMAND} -E copy_if_different 
                       ${datasetFiles}
                           $<TARGET_FILE_DIR:testProg>)

set_property(TARGET ${PROJECT_NAME} PROPERTY VS_DEBUGGER_WORKING_DIRECTORY $<TARGET_FILE_DIR:${PROJECT_NAME}>)

add_test(NAME direct-shader-from-files      COMMAND ${PROJECT_NAME} [direct-shader-from-files]     WORKING_DIRECTORY $<TARGET_FILE_DIR:${PROJECT_NAME}>)
add_test(NAME shader-from-files             COMMAND ${PROJECT_NAME} [shader-from-files]            WORKING_DIRECTORY $<TARGET_FILE_DIR:${PROJECT_NAME}>)
add_test(NAME direct-shader-from-strings    COMMAND ${PROJECT_NAME} [direct-shader-from-strings]   WORKING_DIRECTORY $<TARGET_FILE_DIR:${PROJECT_NAME}>)
add_test(NAME direct-vertex-from-files      COMMAND ${PROJECT_NAME} [direct-vertex-from-files]     WORKING_DIRECTORY $<TARGET_FILE_DIR:${PROJECT_NAME}>)
add_test(NAME direct-fragment-from-files    COMMAND ${PROJECT_NAME} [direct-fragment-from-files]   WORKING_DIRECTORY $<TARGET_FILE_DIR:${PROJECT_NAME}>)
add_test(NAME vertex-from-files             COMMAND ${PROJECT_NAME} [vertex-from-files]            WORKING_DIRECTORY $<TARGET_FILE_DIR:${PROJECT_NAME}>)
add_test(NAME fragment-from-files           COMMAND ${PROJECT_NAME} [fragment-from-files]          WORKING_DIRECTORY $<TARGET_FILE_DIR:${PROJECT_NAME}>)
add_test(NAME direct-faulty-vertex          COMMAND ${PROJECT_NAME} [direct-faulty-vertex]         WORKING_DIRECTORY $<TARGET_FILE_DIR:${PROJECT_NAME}>)
add_test(NAME faulty-vertex                 COMMAND ${PROJECT_NAME} [faulty-vertex]                WORKING_DIRECTORY $<TARGET_FILE_DIR:${PROJECT_NAME}>)
add_test(NAME direct-notready-program       COMMAND ${PROJECT_NAME} [direct-notready-program]      WORKING_DIRECTORY $<TARGET_FILE_DIR:${PROJECT_NAME}>)
add_test(NAME notready-program              COMMAND ${PROJECT_NAME} [notready-program]             WORKING_DIRECTORY $<TARGET_FILE_DIR:${PROJECT_NAME}>)
add_test(NAME direct-faulty-program         COMMAND ${PROJECT_NAME} [direct-faulty-program]        WORKING_DIRECTORY $<TARGET_FILE_DIR:${PROJECT_NAME}>)
add_test(NAME faulty-program                COMMAND ${PROJECT_NAME} [faulty-program]               WORKING_DIRECTORY $<TARGET_FILE_DIR:${PROJECT_NAME}>)
add_test(NAME bad-source-stream             COMMAND ${PROJECT_NAME} [bad-source-stream]            WORKING_DIRECTORY $<TARGET_FILE_DIR:${PROJECT_NAME}>)
add_test(NAME no-glewinit                   COMMAND ${PROJECT_NAME} [no-glewinit]                  WORKING_DIRECTORY $<TARGET_FILE_DIR:${PROJECT_NAME}>)
