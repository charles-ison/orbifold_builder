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
    DiagramItem.cpp DiagramItem.h
    DiagramScene.cpp DiagramScene.h
    DiagramTextItem.cpp DiagramTextItem.h
    Main.cpp
    MainWindow.cpp MainWindow.h
    ResultsWidget.cpp ResultsWidget.h
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
    "FShader.glsl"
    "VShader.glsl"
)

qt6_add_resources(orbifold_builder "shaders"
    PREFIX
        "/"
    FILES
        ${shaders_resource_files}
)

set(images
    "images/cube.png"
    "images/bold.png"
    "images/delete.png"
    "images/floodfill.png"
    "images/italic.png"
    "images/linecolor.png"
    "images/linepointer.png"
    "images/pointer.png"
    "images/textpointer.png"
    "images/underline.png"
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
