# The name of your app.
# NOTICE: name defined in TARGET has a corresponding QML filename.
#         If name defined in TARGET is changed, following needs to be
#         done to match new name:
#         - corresponding QML filename must be changed
#         - desktop icon filename must be changed
#         - desktop filename must be changed
#         - icon definition filename in desktop file must be changed
TARGET = harbour-dice

QT += sensors

# LIBS += --rpath,/

CONFIG += sailfishapp
CONFIG += c++11

# PKGCONFIG += bullet assimp
PKGCONFIG += assimp

include(src/bullet/bullet.pri)


# INCLUDEPATH += bullet/

SOURCES += src/harbour-dice.cpp \
    src/glitem.cpp \
    src/glprogram.cpp \
    src/cameramanager.cpp \
    src/bullet.cpp \
    src/dice.cpp \
    src/lightmanager.cpp \
    src/light.cpp \
    src/dicemodel.cpp \
    src/bimesh.cpp

DEPLOYMENT_PATH = /usr/share/$$TARGET

QMAKE_RPATHDIR += /opt/sdk/$$TARGET/$$DEPLOYMENT_PATH/lib/:$$DEPLOYMENT_PATH/lib/

glsl.files = \
    src/dice_vert.glsl.out src/dice_frag.glsl.out \
    src/debug_vert.glsl src/debug_frag.glsl \
    src/light_vert.glsl src/light_frag.glsl
glsl.path = $$DEPLOYMENT_PATH

image.files = mer-cube.png assets/dice.obj assets/dice.blend
image.path = $$DEPLOYMENT_PATH

INSTALLS += glsl image

OTHER_FILES += qml/harbour-dice.qml \
    qml/cover/CoverPage.qml \
    qml/pages/DicePage.qml \
    rpm/harbour-dice.spec \
    rpm/harbour-dice.yaml \
    harbour-dice.desktop \
    src/dice_frag.glsl \
    src/dice_vert.glsl \
    qml/pages/About.qml \
    src/debug_frag.glsl \
    src/debug_vert.glsl \
    src/light_frag.glsl \
    src/light_vert.glsl

HEADERS += \
    src/glitem.h \
    src/glprogram.h \
    src/cameramanager.h \
    src/bullet.h \
    src/dice.h \
    src/lightmanager.h \
    src/light.h \
    src/dicemodel.h \
    src/bimesh.h

