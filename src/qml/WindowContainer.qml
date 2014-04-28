/****************************************************************************
**
** Copyright (C) 2012 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the examples of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:BSD$
** You may use this file under the terms of the BSD license as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of Digia Plc and its Subsidiary(-ies) nor the names
**     of its contributors may be used to endorse or promote products derived
**     from this software without specific prior written permission.
**
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
**
** $QT_END_LICENSE$
**
****************************************************************************/

import QtQuick 2.0
import QtQuick.Window 2.0

Item {
    id: container

    x: targetX
    y: targetY
    width: targetWidth
    height: targetHeight
    scale: targetScale

    visible: isFullscreen || !root.hasFullscreenWindow
    onVisibleChanged: {
        window.clientRenderingEnabled = visible
        console.log("visibility changed: " + visible);
    }

    opacity: 0

    property real targetX
    property real targetY
    property real targetWidth
    property real targetHeight
    property real targetScale

    property variant window: null
    onWindowChanged: {
        window.parent = container
        targetWidth = window.width
        targetHeight = window.height
    }

    property bool animationsEnabled: false
    property bool isFullscreen: state === "fullscreen"
    property int index

    state: "normal"

    Behavior on x {
        enabled: container.animationsEnabled;
        NumberAnimation { easing.type: Easing.InCubic; duration: 200; }
    }

    Behavior on y {
        enabled: container.animationsEnabled;
        NumberAnimation { easing.type: Easing.InQuad; duration: 200; }
    }

    Behavior on width {
        enabled: container.animationsEnabled;
        NumberAnimation { easing.type: Easing.InCubic; duration: 200; }
    }

    Behavior on height {
        enabled: container.animationsEnabled;
        NumberAnimation { easing.type: Easing.InCubic; duration: 200; }
    }

    Behavior on scale {
        enabled: container.animationsEnabled;
        NumberAnimation { easing.type: Easing.InQuad; duration: 200; }
    }

    Behavior on opacity {
        enabled: true;
        NumberAnimation { easing.type: Easing.Linear; duration: 250; }
    }

    SurfaceRenderer {
        id: effect
        source: window
        anchors.fill: window
        opacity: 1.0
        z: 1
    }

    transform: [
        Scale { id: scaleTransform; origin.x: container.width / 2; origin.y: container.height / 2; xScale: 1; yScale: 1 }
    ]

    property real fullscreenScale: Math.min(root.width / width, root.height / height)

    transitions: [
        Transition {
            from: "*"; to: "normal"
            SequentialAnimation {
                ScriptAction {
                    script: {
                        compositor.fullscreenSurface = null
                        background.opacity = 1
                    }
                }
                ParallelAnimation {
                    NumberAnimation { target: container; property: "x"; easing.type: Easing.Linear; to: targetX; duration: 400; }
                    NumberAnimation { target: container; property: "y"; easing.type: Easing.Linear; to: targetY; duration: 400; }
                    NumberAnimation { target: container; property: "scale"; easing.type: Easing.Linear; to: targetScale; duration: 400; }
                }
                ScriptAction {
                    script: container.z = 0
                }
            }
        },
        Transition {
            from: "*"; to: "fullscreen"
            SequentialAnimation {
                ScriptAction {
                    script: {
                        container.z = 1
                        background.opacity = 0
                    }
                }
                ParallelAnimation {
                    NumberAnimation { target: container; property: "x"; easing.type: Easing.Linear; to: (root.width - container.width) / 2; duration: 400; }
                    NumberAnimation { target: container; property: "y"; easing.type: Easing.Linear; to: (root.height - container.height) / 2; duration: 400; }
                    NumberAnimation { target: container; property: "scale"; easing.type: Easing.Linear; to: fullscreenScale; duration: 400; }
                }
                ScriptAction {
                    script: compositor.fullscreenSurface = window.surface
                }
            }
        }
    ]

    SequentialAnimation {
        id: destroyAnimation
        NumberAnimation { target: scaleTransform; property: "yScale"; easing.type: Easing.Linear; to: 0.01; duration: 200; }
        NumberAnimation { target: scaleTransform; property: "xScale"; easing.type: Easing.Linear; to: 0.01; duration: 150; }
        NumberAnimation { target: container; property: "opacity"; easing.type: Easing.Linear; to: 0.0; duration: 150; }
        ScriptAction { script: container.parent.removeWindow(window); }
    }

    function runDestroyAnimation() {
        destroyAnimation.start();
    }

//    Component.onCompleted: {
//        window.takeFocus()
//    }
}
