QT       += widgets sql quickwidgets

CONFIG += c++17
CONFIG -= qml_debug

SOURCES += \
    source/Applications/TRTimeRecord.cpp \
    source/Common/Ui/Common_Ui_SelfPushButton.cpp \
    source/Common/Util/Common_Util_Base.cpp \
    source/Common/Util/Common_Util_JsonExt.cpp \
    source/Config/Config_ConfigLoader.cpp \
    source/Form/PatientInfoSetting/Form_PatientInfoSetting.cpp \
    source/Form/PatientInput/Form_PatientInput.cpp \
    source/Form/TRTimeOperator/Form_TRTimeOperator.cpp \
    source/Form/TRTimeOperator/Form_TRTimeOperator_Interface.cpp \
    source/Form/TRTimeOperator/Form_TRTimeOperator_MenuBar.cpp \
    source/Form/TRTimeOperator/Form_TRTimeOperator_QueryNextPatient.cpp \
    source/Form/TRTimeOperator/Form_TRTimeOperator_ToolBar.cpp \
    source/Storage/CSV/Storage_CSV_Viewer.cpp \
    source/Storage/CSV/Storage_CSV_Writer.cpp \
    source/Storage/DAO/Storage_DAO.cpp \
    source/Storage/DAO/Storage_DAO_Viewer.cpp

HEADERS += \
    include/Common/Ui/Common_Ui_SelfPushButton.h \
    include/Common/Util/Common_Util_Base.h \
    include/Common/Util/Common_Util_JsonExt.h \
    include/Config/Config_ConfigLoader.h \
    include/Form/PatientInfoSetting/Form_PatientInfoSetting.h \
    include/Form/PatientInput/Form_PatientInput.h \
    include/Form/TRTimeOperator/Form_TRTimeOperator.h \
    include/Form/TRTimeOperator/Form_TRTimeOperator_Interface.h \
    include/Form/TRTimeOperator/Form_TRTimeOperator_MenuBar.h \
    include/Form/TRTimeOperator/Form_TRTimeOperator_QueryForNextPatient.h \
    include/Form/TRTimeOperator/Form_TRTimeOperator_ToolBar.h \
    include/PatientInfoSetting/PatientInfoSetting.h \
    include/PatientInput/PatientInput.h \
    include/Storage/CSV/Storage_CSV_Viewer.h \
    include/Storage/CSV/Storage_CSV_Writer.h \
    include/Storage/DAO/Storage_DAO.h \
    include/Storage/DAO/Storage_DAO_Interface.h \
    include/Storage/DAO/Storage_DAO_Viewer.h

FORMS += \
    ui/Form/PatientInfoSetting.ui \
    ui/Form/TRTimeOperator.ui

RESOURCES += \
    ui/qrc/StartUi.qrc \
    ui/qrc/icon.qrc \
    ui/qrc/qml.qrc \
    ui/qrc/style.qrc
