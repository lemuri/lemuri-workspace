file(GLOB QML_SRC qml/*.qml qml/*.js)

include_directories(
    ${Qt5Gui_PRIVATE_INCLUDE_DIRS}
)

set(lemuri_workspace_SRCS
    ${QML_SRC}
    surfaceitem.cpp
    viewoutput.cpp
    lemuricompositor.cpp
    main.cpp
)

qt5_add_resources(lemuri_workspace_SRCS
    application.qrc
    OPTIONS -compress 3 -threshold 3
)

message(STATUS "----EGL " ${EGL_LIBRARIES})
message(STATUS "----GL " ${GL_LIBRARIES})

add_executable(lemuri-workspace
    ${lemuri_workspace_SRCS}
)
target_link_libraries(lemuri-workspace
    ${EGL_LIBRARIES}
    ${GL_LIBRARIES}
)

qt5_use_modules(lemuri-workspace Quick DBus Widgets Compositor)
