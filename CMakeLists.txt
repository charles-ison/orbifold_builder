cmake_minimum_required(VERSION 3.16)
project(orbifold_builder LANGUAGES CXX)

if(NOT DEFINED INSTALL_EXAMPLESDIR)
    set(INSTALL_EXAMPLESDIR "examples")
endif()

set(INSTALL_EXAMPLEDIR "${INSTALL_EXAMPLESDIR}/opengl/orbifold_builder")

find_package(Qt6 REQUIRED COMPONENTS Core Gui OpenGL OpenGLWidgets Widgets)

qt_standard_project_setup()

qt_add_executable(orbifold_builder
    GeometryEngine.cpp GeometryEngine.h
    Arrow.cpp Arrow.h
    diagrams/DiagramItem.cpp diagrams/DiagramItem.h
    diagrams/DiagramScene.cpp diagrams/DiagramScene.h
    diagrams/DiagramTextItem.cpp diagrams/DiagramTextItem.h
    Main.cpp
    MainWindow.cpp MainWindow.h
    ResultsWidget.cpp ResultsWidget.h
    surfaces/Cube.cpp surfaces/Cube.h
    surfaces/Sphere.cpp surfaces/Sphere.h
    surfaces/Torus.cpp surfaces/Torus.h
    surfaces/MobiusStrip.cpp surfaces/MobiusStrip.h
    surfaces/KleinBottle.cpp surfaces/KleinBottle.h
    surfaces/VertexData.h
)

set_target_properties(orbifold_builder PROPERTIES
    WIN32_EXECUTABLE TRUE
    MACOSX_BUNDLE TRUE
)

target_link_libraries(orbifold_builder PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::OpenGL
    Qt6::OpenGLWidgets
    Qt6::Widgets
)

# Resources:
set(shaders_resource_files
    "shaders/FShader.glsl"
    "shaders/VShader.glsl"
)

qt6_add_resources(orbifold_builder "shaders"
    PREFIX
        "/"
    FILES
        ${shaders_resource_files}
)

set(images
    "images/delete.png"
    "images/floodfill.png"
    "images/linecolor.png"
    "images/linepointer.png"
    "images/pointer.png"
    "images/textpointer.png"
)

qt6_add_resources(orbifold_builder "images"
    PREFIX
        "/"
    FILES
        ${images}
)

install(TARGETS orbifold_builder
    RUNTIME DESTINATION "${INSTALL_EXAMPLEDIR}"
    BUNDLE DESTINATION "${INSTALL_EXAMPLEDIR}"
    LIBRARY DESTINATION "${INSTALL_EXAMPLEDIR}"
)
