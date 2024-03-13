QT       += widgets sql

CONFIG += c++17

SOURCES += \
    source/ConfigLoader.cpp \
    source/DAO.cpp \
    source/PatientInput.cpp \
    source/TRTimeOperator.cpp \
    source/main.cpp \
    ui/Form/dialog.cpp

HEADERS += \
    include/ConfigLoader.h \
    include/DAO.h \
    include/DAO_Interface.h \
    include/PatientInput.h \
    include/TRTimeOperator.h \
    include/Util.h \
    ui/Form/dialog.h

FORMS += \
    ui/Form/TRTimeOperator.ui \
    ui/Form/dialog.ui

RESOURCES += \
    ui/qrc/StartUi.qrc \
    ui/qrc/icon.qrc \
    ui/qrc/style.qrc
