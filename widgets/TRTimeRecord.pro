QT       += widgets sql

CONFIG += c++17

SOURCES += \
    source/ConfigLoader.cpp \
    source/DAO.cpp \
    source/PatientInput.cpp \
    source/TRTimeOperator.cpp \
    source/main.cpp

HEADERS += \
    include/ConfigLoader.h \
    include/DAO.h \
    include/PatientInput.h \
    include/TRTimeOperator.h

FORMS += \
    ui/Form/TRTimeOperator.ui

RESOURCES += \
    ui/qrc/StartUi.qrc \
    ui/qrc/style.qrc
