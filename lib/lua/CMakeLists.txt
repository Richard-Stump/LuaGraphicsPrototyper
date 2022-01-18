cmake_minimum_required(VERSION 3.20)

project(liblua VERSION 5.4 LANGUAGES C)

file(GLOB SOURCE_FILES "src/*")

add_library(lua ${SOURCE_FILES})

target_include_directories(lua PUBLIC "src/")

set_target_properties(lua PROPERTIES LINKER_LANGUAGE C)