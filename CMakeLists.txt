cmake_minimum_required(VERSION 3.16)
project(orbifold_builder LANGUAGES CXX)

if(NOT DEFINED INSTALL_EXAMPLESDIR)
    set(INSTALL_EXAMPLESDIR "examples")
endif()

set(INSTALL_EXAMPLEDIR "${INSTALL_EXAMPLESDIR}/opengl/orbifold_builder")

find_package(Qt6 REQUIRED COMPONENTS Core Gui OpenGL OpenGLWidgets Widgets)

qt_standard_project_setup()

qt_add_executable(orbifold_builder
    geometryengine.cpp geometryengine.h
    arrow.cpp arrow.h
    diagramitem.cpp diagramitem.h
    diagramscene.cpp diagramscene.h
    diagramtextitem.cpp diagramtextitem.h
    main.cpp
    mainwindow.cpp mainwindow.h
    mainwidget.cpp mainwidget.h
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
    "fshader.glsl"
    "vshader.glsl"
)

qt6_add_resources(orbifold_builder "shaders"
    PREFIX
        "/"
    FILES
        ${shaders_resource_files}
)

set(images
    "images/cube.png"
    "images/background1.png"
    "images/background2.png"
    "images/background3.png"
    "images/background4.png"
    "images/bold.png"
    "images/bringtofront.png"
    "images/delete.png"
    "images/floodfill.png"
    "images/italic.png"
    "images/linecolor.png"
    "images/linepointer.png"
    "images/pointer.png"
    "images/sendtoback.png"
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
