#include "../../../include/Form/StorageSetting/Form_StorageSetting.h"
#include "ui_StorageSetting.h"
#include "../../../include/Global/Config/Global_Config_ConfigLoader.h"
#include "../../../include/Storage/CSV/Storage_CSV_Writer.h"
#include <QDialogButtonBox>
#include <QPushButton>
#include <QFileDialog>
#include <QFile>
#include <QFileDevice>

StorageSetting::StorageSetting(QWidget* parent):QDialog(parent),uiForm(new Ui::StorageSetting){
    uiForm->setupUi(this);

    /*Load configuration*/

    this->uiForm->lineEdit->setText(ConfigLoader::getInstance()->getSystemCSVPath());
    this->theSettingPath = ConfigLoader::getInstance()->getSystemCSVPath();

    QObject::connect(this->uiForm->pushButton_FilePath,
                     SIGNAL(pressed()),
                     this,
                     SLOT(openFileDialog()));

    QObject::connect(this->uiForm->pushButton_Save,
                     SIGNAL(pressed()),
                     this,
                     SLOT(SaveConfigHandle()));

    QObject::connect(this->uiForm->pushButton_Cancel,
                     SIGNAL(pressed()),
                     this,
                     SLOT(CancelHandle()));
}

StorageSetting::~StorageSetting(){

}

void StorageSetting::closeEvent(QCloseEvent *event){
    QDialog::closeEvent(event);
    this->setResult(3);
}

void StorageSetting::SaveConfigHandle(){
    QFile file(ConfigLoader::getInstance()->getSystemCSVPath());
    ConfigLoader::getInstance()->setSystemCSVPath(this->theSettingPath);
    file.setPermissions(QFileDevice::ReadOwner | QFileDevice::WriteOwner);
    file.copy(ConfigLoader::getInstance()->getSystemCSVPath());
    file.close();
    CSVWriter::getInstance()->reOpen();
    accept();
}

void StorageSetting::CancelHandle(){
    reject();
}

void StorageSetting::openFileDialog(){
    QString theDir = QFileDialog::getExistingDirectory();
    this->theSettingPath = theDir.append("/TR.csv");
    this->uiForm->lineEdit->setText(theDir);
}
