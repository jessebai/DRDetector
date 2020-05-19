QT += quick serialport
QT += charts
CONFIG += c++11

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS QT_MESSAGELOGCONTEXT

DEFINES -= UNICODE
DEFINES -= _UNICODE
# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

HEADERS += \
    $$files(src/facade/*.h) \
    $$files(src/device/*.h) \
    $$files(src/data/*.h)

SOURCES += \
    main.cpp \
    $$files(src/facade/*.cpp) \
    $$files(src/device/*.cpp) \
    $$files(src/data/*.cpp)


RESOURCES += qml.qrc \
    $$files(modules/*.qml) \
    $$files(custom/*.qml) \
    $$files(images/*)

RC_FILE = myapp.rc

OPENCV_PATH = "E:\Programs Files\opencv\opencv342\install"
DCMTK_PATH = "E:\ProgramFiles\dcmtk86\install"
SDK_PATH = "E:\Programs Files\SDK"
MVS_PATH = "E:\Programs\MVS\MVS\Development"

INCLUDEPATH += "$${DCMTK_PATH}\include"\
    "$${OPENCV_PATH}\include"\
    "$${SDK_PATH}\include"\
    "$${MVS_PATH}\Includes"\


LIBS += -L"$${DCMTK_PATH}\lib"\
    -L"$${OPENCV_PATH}\x86\vc15\lib"\
    -L"$${SDK_PATH}\lib\x86"\
    -L"$${MVS_PATH}\Libraries\win32"\

LIBS += -lopencv_core342 \
    -lopencv_highgui342 \
    -lopencv_imgproc342 \
    -lopencv_imgcodecs342 \

LIBS += -lCRInterface \

LIBS += -ladvapi32 \
    -ldcmdata \
    -loflog \
    -lofstd \
    -lws2_32 \
    -lnetapi32 \
    -lwsock32 \
    -ldcmimage \
    -ldcmimgle \

LIBS += -lGdi32 \

LIBS += -lMvCameraControl \

# Additional import path used to resolve QML modules in Qt Creator's code model
QML_IMPORT_PATH =

# Additional import path used to resolve QML modules just for Qt Quick Designer
QML_DESIGNER_IMPORT_PATH =

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

DISTFILES += \
    myapp.rc
