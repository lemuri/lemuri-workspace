file(GLOB QML_SRC qml/*.qml qml/*.js)

set(lemuri_workspace_SRCS
    ${QML_SRC}
    qmlcompositor.cpp
    main.cpp
)

qt5_add_resources(lemuri_workspace_SRCS
    application.qrc
    OPTIONS -compress 3 -threshold 3
)

add_executable(lemuri-workspace
    ${lemuri_workspace_SRCS}
)

qt5_use_modules(lemuri-workspace Quick DBus Widgets Compositor)
