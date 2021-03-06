#-------------------------------------------------
#
# Project created by QtCreator 2020-05-30T18:03:18
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = labeldoc
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS


# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

CONFIG += c++11

INCLUDEPATH += /usr/local/include/opencv4 \
     /usr/local/include \
     /usr/local/include/leptonica


LIBS += -L/usr/local/opt/opencv/lib
LIBS += -lopencv_core -lopencv_highgui -lopencv_imgproc -lopencv_imgcodecs -lopencv_tracking -lopencv_video -lopencv_videoio -lopencv_dnn -lopencv_ml

LIBS += -L/usr/local/lib -llept -ltesseract

INCLUDEPATH +=/usr/include/leptonica

LIBS += -llept





SOURCES += \
        autolabeldialog.cpp \
        canvas.cpp \
        filelistwidget.cpp \
        imagebinary/bernsen.cpp \
        imagebinary/isauvola.cpp \
        imagebinary/localthreshold.cpp \
        imagebinary/niblack.cpp \
        imagebinary/nick.cpp \
        imagebinary/otsu.cpp \
        imagebinary/padding.cpp \
        imagebinary/sauvola.cpp \
        imagebinary/trsingh.cpp \
        imagebinary/wan.cpp \
        imagebinary/wolf.cpp \
        imageutil.cpp \
        imageviewer.cpp \
        labelwidget.cpp \
        main.cpp \
        mainwindow.cpp \
        models/commutil.cpp \
        models/erfilter.cpp \
        models/msertextdetector.cpp \
        models/pageitem.cpp \
        models/tesseract_model.cpp \
        models/mser.cpp \
        shape.cpp \
        statestorage.cpp


HEADERS += \
        autolabeldialog.h \
        canvas.h \
        filelistwidget.h \
        imagebinary/binary.h \
        imageutil.h \
        imageviewer.h \
        labelwidget.h \
        mainwindow.h \
        models/commutil.h \
        models/erfilter.hpp \
        models/msertextdetector.h \
        models/pageitem.h \
        models/mser.h \
        shape.h \
        statestorage.h \
        models/tesseract_model.h




# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    image.qrc
