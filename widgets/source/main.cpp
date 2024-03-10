#include <iostream>
#include <QApplication>
#include <QFile>

#include "../include/PatientInput.h"
#include "../include/TRTimeOperator.h"
#include "../include/DAO.h"

int main(int argc,char** argv){
    QApplication *app = new QApplication(argc,argv);

    QFile qss(":/style/QSS/MacOS.qss");
    qss.open(QFile::ReadOnly);
    app->setStyleSheet(qss.readAll());
    qss.close();

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
