QT       += widgets

CONFIG += c++17

SOURCES += \
    source/DAO.cpp \
    source/PatientInput.cpp \
    source/TRTimeOperator.cpp \
    source/main.cpp

HEADERS += \
    include/DAO.h \
    include/PatientInput.h \
    include/TRTimeOperator.h

FORMS += \
    ui/Form/TRTimeOperator.ui

RESOURCES += \
    ui/qrc/style.qrc
