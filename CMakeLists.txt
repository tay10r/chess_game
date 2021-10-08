cmake_minimum_required(VERSION 3.14.7)

project(chess_game CXX)

include(FetchContent)

FetchContent_Declare(bvh URL "https://github.com/madmann91/bvh/archive/master.zip")

FetchContent_MakeAvailable(bvh)

find_package(Qt5 REQUIRED COMPONENTS Widgets)

add_executable(main
  main.cpp
  ChessBvhModel.h
  ChessBvhModel.cpp
  OpenGLWidget.h
  OpenGLWidget.cpp
  RenderEngine.h
  CxxRenderEngine.h
  CxxRenderEngine.cpp
  assets/assets.qrc
  deps/tiny_obj_loader.h
  deps/tiny_obj_loader.cpp)

set_target_properties(main
  PROPERTIES
    AUTOMOC ON
    AUTORCC ON)

target_compile_features(main PRIVATE cxx_std_17)

target_link_libraries(main PRIVATE Qt5::Widgets bvh)
