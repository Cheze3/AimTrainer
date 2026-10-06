cmake_minimum_required(VERSION 3.20)
project(aimtrainer CXX C)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)

set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)

FetchContent_Declare(glfw
    GIT_REPOSITORY https://github.com/glfw/glfw.git
    GIT_TAG 3.4
    GIT_SHALLOW TRUE)

FetchContent_Declare(glm
    GIT_REPOSITORY https://github.com/g-truc/glm.git
    GIT_TAG 1.0.1
    GIT_SHALLOW TRUE)

FetchContent_MakeAvailable(glfw glm)

add_library(glad STATIC extern/glad/src/gl.c)
target_include_directories(glad PUBLIC extern/glad/include)
target_link_libraries(glad PUBLIC ${CMAKE_DL_LIBS})

add_executable(aimtrainer src/main.cpp)
target_compile_definitions(aimtrainer PRIVATE GLFW_INCLUDE_NONE)
target_link_libraries(aimtrainer PRIVATE glfw glm::glm glad)

if(MSVC)
    target_compile_options(aimtrainer PRIVATE /W4)
else()
    target_compile_options(aimtrainer PRIVATE -Wall -Wextra)
endif()