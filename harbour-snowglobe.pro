# The name of your app.
# NOTICE: name defined in TARGET has a corresponding QML filename.
#         If name defined in TARGET is changed, following needs to be
#         done to match new name:
#         - corresponding QML filename must be changed
#         - desktop icon filename must be changed
#         - desktop filename must be changed
#         - icon definition filename in desktop file must be changed
TARGET = harbour-snowglobe

CONFIG += sailfishapp

SOURCES += src/harbour-snowglobe.cpp \
    src/globegeometry.cpp \
    src/flakes.cpp \
    src/glitem.cpp

DEPLOYMENT_PATH = /usr/share/$$TARGET

glsl.files = src/sq_vert.glsl src/sq_shad.glsl src/sq_geom.glsl
glsl.path = $$DEPLOYMENT_PATH

image.files = src/cube.png
image.path = $$DEPLOYMENT_PATH

INSTALLS += glsl image

OTHER_FILES += qml/harbour-snowglobe.qml \
    qml/cover/CoverPage.qml \
    qml/pages/FirstPage.qml \
    qml/pages/SecondPage.qml \
    rpm/harbour-snowglobe.spec \
    rpm/harbour-snowglobe.yaml \
    harbour-snowglobe.desktop \
    src/vshad.glsl \
    src/fshad.glsl \
    src/sq_shad.glsl \
    src/sq_vert.glsl \
    src/sq_geom.glsl

HEADERS += \
    src/globegeometry.h \
    src/flakes.h \
    src/glitem.h

