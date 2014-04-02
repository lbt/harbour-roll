# The name of your app.
# NOTICE: name defined in TARGET has a corresponding QML filename.
#         If name defined in TARGET is changed, following needs to be
#         done to match new name:
#         - corresponding QML filename must be changed
#         - desktop icon filename must be changed
#         - desktop filename must be changed
#         - icon definition filename in desktop file must be changed
TARGET = harbour-roll

QT += sensors

# LIBS += --rpath,/

CONFIG += sailfishapp
CONFIG += c++11

# PKGCONFIG += bullet assimp
PKGCONFIG += assimp

include(src/bullet/bullet.pri)


# INCLUDEPATH += bullet/

SOURCES += src/harbour-roll.cpp \
    src/glitem.cpp \
    src/glprogram.cpp \
    src/cameramanager.cpp \
    src/bullet.cpp \
    src/roll.cpp \
    src/lightmanager.cpp \
    src/light.cpp \
    src/bimesh.cpp \
    src/vaocontainer.cpp \
    src/worldobject.cpp

DEPLOYMENT_PATH = /usr/share/$$TARGET

QMAKE_RPATHDIR += /opt/sdk/$$TARGET/$$DEPLOYMENT_PATH/lib/:$$DEPLOYMENT_PATH/lib/

glsl.files = \
    src/roll_vert.glsl.out src/roll_frag.glsl.out \
    src/debug_vert.glsl src/debug_frag.glsl \
    src/light_vert.glsl src/light_frag.glsl
glsl.path = $$DEPLOYMENT_PATH

model.files = model/*.png model/*.obj model/*.mtl
model.path = $$DEPLOYMENT_PATH

INSTALLS += glsl model

OTHER_FILES += \
    qml/cover/CoverPage.qml \
    rpm/harbour-roll.spec \
    rpm/harbour-roll.yaml \
    harbour-roll.desktop \
    src/roll_frag.glsl \
    src/roll_vert.glsl \
    qml/pages/About.qml \
    src/debug_frag.glsl \
    src/debug_vert.glsl \
    src/light_frag.glsl \
    src/light_vert.glsl \
    qml/pages/RollPage.qml \
    qml/harbour-roll.qml

HEADERS += \
    src/glitem.h \
    src/glprogram.h \
    src/cameramanager.h \
    src/bullet.h \
    src/roll.h \
    src/lightmanager.h \
    src/light.h \
    src/bimesh.h \
    src/vaocontainer.h \
    src/worldobject.h

# MerSDK david:~/src/glsl-optimizer > ./glslopt -f ../harbour-dice/src/dice_frag.glsl 
# MerSDK david:~/src/glsl-optimizer > ./glslopt -v ../harbour-dice/src/dice_vert.glsl 