QT       += widgets sql quickwidgets

CONFIG += c++17
CONFIG -= qml_debug

SOURCES += \
    source/Applications/TRTimeRecord.cpp \
    source/Common/Ui/Common_Ui_SelfPushButton.cpp \
    source/Common/Util/Common_Util_Base.cpp \
    source/Common/Util/Common_Util_JsonExt.cpp \
    source/Form/About/Form_About.cpp \
    source/Global/Communication/Global_Communication_Record.cpp \
    source/Global/Config/Global_Config_ConfigLoader.cpp \
    source/Form/OnlineDatabaseSetting/Form_OnlineDatabaseSetting.cpp \
    source/Form/OperationLoop/Form_OperationLoop.cpp \
    source/Form/OperationPipelineSetting/Form_OperationPipelineSetting.cpp \
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
    source/Storage/DAO/Storage_DAO_Mysql.cpp \
    source/Storage/DAO/Storage_DAO_Sqlite.cpp \
    source/Storage/DAO/Storage_DAO_Viewer.cpp

HEADERS += \
    include/Common/Ui/Common_Ui_SelfPushButton.h \
    include/Common/Util/Common_Util_Base.h \
    include/Common/Util/Common_Util_JsonExt.h \
    include/Form/About/Form_About.h \
    include/Global/Communication/Global_Communication_Record.h \
    include/Global/Config/Global_Config_ConfigLoader.h \
    include/Form/OnlineDatabaseSetting/Form_OnlineDatabaseSetting.h \
    include/Form/OperationLoop/Form_OperationLoop.h \
    include/Form/OperationPipelineSetting/Form_OperationPipelineSetting.h \
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
    include/Storage/DAO/Storage_DAO_Mysql.h \
    include/Storage/DAO/Storage_DAO_Sqlite.h \
    include/Storage/DAO/Storage_DAO_Viewer.h

FORMS += \
    ui/Form/About.ui \
    ui/Form/OnlineDatabaseSetting.ui \
    ui/Form/OperationLoop.ui \
    ui/Form/OperationPipelineSetting.ui \
    ui/Form/PatientInfoSetting.ui \
    ui/Form/TRTimeOperator.ui

RESOURCES += \
    ui/qrc/StartUi.qrc \
    ui/qrc/icon.qrc \
    ui/qrc/qml.qrc \
    ui/qrc/style.qrc
	
RC_ICONS +=  \
	ui/qrc/img/logo.ico
