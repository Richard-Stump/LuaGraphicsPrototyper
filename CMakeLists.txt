cmake_minimum_required(VERSION 3.20)

project(GraphicsPrototype VERSION 0.1.1)

#Require C++17 for the project
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

find_package(OpenGL REQUIRED)

#Disable building extraneous parts of GLFW
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)

set(glm_DIR /lib/glm)

#Add the libraries for the project
add_subdirectory(lib/glfw/)
add_subdirectory(lib/glm/)
add_subdirectory(lib/glad/)

set(SOURCE_FILES
	src/Main.cpp
	src/Application.cpp
	src/OpenGL.hpp
)

#Add the executable and all the libraries it needs
add_executable(GraphicsPrototype ${SOURCE_FILES})
target_link_libraries(GraphicsPrototype PRIVATE glfw glm::glm glad)