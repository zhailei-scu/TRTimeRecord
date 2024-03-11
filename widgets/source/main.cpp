#include <iostream>
#include <QApplication>
#include <QFile>
#include <QString>
#include <QSplashScreen>
#include <windows.h>

#include "../include/PatientInput.h"
#include "../include/TRTimeOperator.h"
#include "../include/ConfigLoader.h"
#include "../include/DAO.h"

int main(int argc,char** argv){
    /*Start QT compents*/
    QApplication *app = new QApplication(argc,argv);
    /*Loadding GUI*/
    QPixmap pix(":/img/StartGUI.bmp");
    QSplashScreen splan(pix);
    splan.show();
    Sleep(2000);
    splan.close();

    QFile qss(":/style/QSS/MacOS.qss");
    qss.open(QFile::ReadOnly);
    app->setStyleSheet(qss.readAll());
    qss.close();

    QStringList drivers = QSqlDatabase::drivers();

    for(QStringList::iterator it = drivers.begin();it != drivers.end(); it++){
        qDebug()<<*it;
    }
    //PatientInput *patientInputForm = new PatientInput(nullptr);
    //patientInputForm->show();

    /*StartDataBase*/
    app->addLibraryPath(app->applicationDirPath() + "/plugins/sqldrivers");
    DAO::Start();

    TRTimeOperator *trTimeOperatorForm = new TRTimeOperator(nullptr);
    trTimeOperatorForm->show();

    app->exec();
    //delete patientInputForm;
    delete trTimeOperatorForm;
    delete app;
    return 0;
}
