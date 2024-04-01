#include "../../../include/Form/OnlineDatabaseSetting/Form_OnlineDatabaseSetting.h"
#include "ui_OnlineDatabaseSetting.h"
#include "../../../include/Global/Config/Global_Config_ConfigLoader.h"
#include <QDialogButtonBox>
#include <QPushButton>

OnlineDatabaseSetting::OnlineDatabaseSetting(QWidget* parent):QDialog(parent),uiForm(new Ui::OnlineDatabaseSetting){
    uiForm->setupUi(this);

    /*Load configuration*/
    this->uiForm->lineEdit_IP->setText(ConfigLoader::getInstance()->getOnlineDatabaseInfo().at(OnlineInfoPattern::getInstance()->getDefalutPattern().at(0)));
    this->uiForm->lineEdit_Port->setText(ConfigLoader::getInstance()->getOnlineDatabaseInfo().at(OnlineInfoPattern::getInstance()->getDefalutPattern().at(1)));
    this->uiForm->lineEdit_DatabaseName->setText(ConfigLoader::getInstance()->getOnlineDatabaseInfo().at(OnlineInfoPattern::getInstance()->getDefalutPattern().at(2)));
    this->uiForm->lineEdit_username->setText(ConfigLoader::getInstance()->getOnlineDatabaseInfo().at(OnlineInfoPattern::getInstance()->getDefalutPattern().at(3)));
    this->uiForm->lineEdit_password->setText(ConfigLoader::getInstance()->getOnlineDatabaseInfo().at(OnlineInfoPattern::getInstance()->getDefalutPattern().at(4)));

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
    std::map<QString,QString> config;
    config.insert(std::pair<QString,QString>(OnlineInfoPattern::getInstance()->getDefalutPattern().at(0),this->uiForm->lineEdit_IP->text()));
    config.insert(std::pair<QString,QString>(OnlineInfoPattern::getInstance()->getDefalutPattern().at(1),this->uiForm->lineEdit_Port->text()));
    config.insert(std::pair<QString,QString>(OnlineInfoPattern::getInstance()->getDefalutPattern().at(2),this->uiForm->lineEdit_DatabaseName->text()));
    config.insert(std::pair<QString,QString>(OnlineInfoPattern::getInstance()->getDefalutPattern().at(3),this->uiForm->lineEdit_username->text()));
    config.insert(std::pair<QString,QString>(OnlineInfoPattern::getInstance()->getDefalutPattern().at(4),this->uiForm->lineEdit_password->text()));

    ConfigLoader::getInstance()->setOnlineDatabaseInfo(config);

    accept();
}

void OnlineDatabaseSetting::CancelHandle(){
    reject();
}
