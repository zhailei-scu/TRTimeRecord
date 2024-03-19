QT       += widgets sql quickwidgets

CONFIG += c++17

SOURCES += \
    source/Config/ConfigLoader.cpp \
    source/DAO/CSVViewer.cpp \
    source/DAO/CSVWriter.cpp \
    source/DAO/DAO.cpp \
    source/DAO/DAOViewer.cpp \
    source/PatientInput/PatientInput.cpp \
    source/TRTimeOperator/OperatorMenuBar.cpp \
    source/TRTimeOperator/OperatorToolBar.cpp \
    source/TRTimeOperator/QueryNextPatient.cpp \
    source/TRTimeOperator/TRTimeOperator.cpp \
    source/TRTimeOperator/TRTimeOperator_Interface.cpp \
    source/Util/SelfPushButton.cpp \
    source/main.cpp

HEADERS += \
    include/Config/ConfigLoader.h \
    include/DAO/CSVViewer.h \
    include/DAO/CSVWriter.h \
    include/DAO/DAO.h \
    include/DAO/DAOViewer.h \
    include/DAO/DAO_Interface.h \
    include/PatientInput/PatientInput.h \
    include/TRTimeOperator/OperatorMenuBar.h \
    include/TRTimeOperator/OperatorToolBar.h \
    include/TRTimeOperator/QueryForNextPatient.h \
    include/TRTimeOperator/TRTimeOperator.h \
    include/TRTimeOperator/TRTimeOperator_Interface.h \
    include/Util/SelfPushButton.h \
    include/Util/Util.h

FORMS += \
    ui/Form/PatientInput.ui \
    ui/Form/TRTimeOperator.ui

RESOURCES += \
    ui/qrc/StartUi.qrc \
    ui/qrc/icon.qrc \
    ui/qrc/qml.qrc \
    ui/qrc/style.qrc
