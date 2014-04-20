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


INCLUDEPATH += "."

SOURCES += src/harbour-roll.cpp \
    src/glitem.cpp \
    src/glprogram.cpp \
    src/cameramanager.cpp \
    src/roll.cpp \
    src/lightmanager.cpp \
    src/light.cpp \
    src/renderable.cpp \
    src/physics.cpp \
    src/assetstore.cpp \
    src/vao.cpp \
    src/utils.cpp \
    src/worlditem.cpp \
    src/world.cpp \
    src/rollworld.cpp \
    src/worlddebugdrawer.cpp \
    src/worldbuilder.cpp \
    src/worldrunner.cpp \
    src/shader.cpp \
    src/texture.cpp \
    src/rollbuilder.cpp \
    src/transform.cpp \
    src/rendermeshtextured.cpp \
    src/rendermeshshaded.cpp \
    src/cameraflyermotion.cpp \
    src/basemotion.cpp \
    src/followmotion.cpp \
    src/orbitmotion.cpp

DEPLOYMENT_PATH = /usr/share/$$TARGET

QMAKE_RPATHDIR += /opt/sdk/$$TARGET/$$DEPLOYMENT_PATH/lib/:$$DEPLOYMENT_PATH/lib/

glsl.files = \
    src/roll_vert.glsl.out src/roll_frag.glsl.out \
    src/track_vert.glsl.out src/track_frag.glsl.out \
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
    src/roll.h \
    src/lightmanager.h \
    src/light.h \
    src/renderable.h \
    src/physics.h \
    src/assetstore.h \
    src/vao.h \
    src/utils.h \
    src/worlditem.h \
    src/world.h \
    src/rollworld.h \
    src/worlddebugdrawer.h \
    src/worldbuilder.h \
    src/worldrunner.h \
    src/shader.h \
    src/texture.h \
    src/rollbuilder.h \
    src/transform.h \
    src/rendermeshtextured.h \
    src/rendermeshshaded.h \
    src/cameraflyermotion.h \
    src/basemotion.h \
    src/followmotion.h \
    src/orbitmotion.h

# ~/src/glsl-optimizer/glslopt -f track_frag.glsl
# ~/src/glsl-optimizer/glslopt -v track_vert.glsl
