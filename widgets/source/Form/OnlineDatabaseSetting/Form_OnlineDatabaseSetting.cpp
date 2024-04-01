#include "../../../include/Form/OnlineDatabaseSetting/Form_OnlineDatabaseSetting.h"
#include "ui_OnlineDatabaseSetting.h"
#include "../../../include/Global/Config/Global_Config_ConfigLoader.h"
#include <QDialogButtonBox>
#include <QPushButton>

OnlineDatabaseSetting::OnlineDatabaseSetting(QWidget* parent):QDialog(parent),uiForm(new Ui::OnlineDatabaseSetting){
    uiForm->setupUi(this);

    /*Load configuration*/
    this->uiForm->lineEdit_IP->setText(ConfigLoader::getInstance()->getOnlineDBIP());
    this->uiForm->lineEdit_Port->setText(QString("%1").arg(ConfigLoader::getInstance()->getOnlineDBPort()));
    this->uiForm->lineEdit_DatabaseName->setText(ConfigLoader::getInstance()->getOnlineDBName());
    this->uiForm->lineEdit_username->setText(ConfigLoader::getInstance()->getOnlineDBUserName());
    this->uiForm->lineEdit_password->setText(ConfigLoader::getInstance()->getOnlineDBPassword());

    QObject::connect(this->uiForm->pushButton_Save,
                     SIGNAL(pressed()),
                     this,
                     SLOT(SaveConfigHandle()));

    QObject::connect(this->uiForm->pushButton_Cancel,
                     SIGNAL(pressed()),
                     this,
                     SLOT(CancelHandle()));
}

OnlineDatabaseSetting::~OnlineDatabaseSetting(){

}

void OnlineDatabaseSetting::closeEvent(QCloseEvent *event){
    QDialog::closeEvent(event);
    this->setResult(3);
}

void OnlineDatabaseSetting::SaveConfigHandle(){
    ConfigLoader::getInstance()->setOnlineDBIP(this->uiForm->lineEdit_IP->text());
    ConfigLoader::getInstance()->setOnlineDBPort(this->uiForm->lineEdit_Port->text().toInt());
    ConfigLoader::getInstance()->setOnlineDBName(this->uiForm->lineEdit_DatabaseName->text());
    ConfigLoader::getInstance()->setOnlineDBUserName(this->uiForm->lineEdit_username->text());
    ConfigLoader::getInstance()->setOnlineDBPassword(this->uiForm->lineEdit_password->text());
    accept();
}

void OnlineDatabaseSetting::CancelHandle(){
    reject();
}
