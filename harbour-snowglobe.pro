# The name of your app.
# NOTICE: name defined in TARGET has a corresponding QML filename.
#         If name defined in TARGET is changed, following needs to be
#         done to match new name:
#         - corresponding QML filename must be changed
#         - desktop icon filename must be changed
#         - desktop filename must be changed
#         - icon definition filename in desktop file must be changed
TARGET = harbour-snowglobe

QT += sensors

CONFIG += sailfishapp

SOURCES += src/harbour-snowglobe.cpp \
    src/globegeometry.cpp \
    src/flakes.cpp \
    src/cube.cpp \
    src/swarm.cpp \
    src/glitem.cpp \
    src/gparticle.cpp

DEPLOYMENT_PATH = /usr/share/$$TARGET

glsl.files = \
    src/cube_vert.glsl src/cube_frag.glsl \
    src/swarm_vert.glsl src/swarm_frag.glsl

glsl.path = $$DEPLOYMENT_PATH

image.files = src/cube.png
image.path = $$DEPLOYMENT_PATH

INSTALLS += glsl image

OTHER_FILES += qml/harbour-snowglobe.qml \
    qml/cover/CoverPage.qml \
    qml/pages/FirstPage.qml \
    qml/pages/CubePage.qml \
    qml/pages/FlakesPage.qml \
    qml/pages/SwarmPage.qml \
    rpm/harbour-snowglobe.spec \
    rpm/harbour-snowglobe.yaml \
    harbour-snowglobe.desktop \
    src/cube_frag.glsl \
    src/cube_vert.glsl \
    src/flakes_frag.glsl \
    src/flakes_vert.glsl \
    src/swarm_frag.glsl \
    src/swarm_vert.glsl

HEADERS += \
    src/globegeometry.h \
    src/flakes.h \
    src/cube.h \
    src/swarm.h \
    src/glitem.h \
    src/gparticle.h

