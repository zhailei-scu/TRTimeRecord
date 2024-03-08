#include <iostream>
#include <QApplication>

#include "../include/PatientInput.h"

int main(int argc,char** argv){
    QApplication *app = new QApplication(argc,argv);

    PatientInput *patientInputForm = new PatientInput(nullptr);

    patientInputForm->show();

    app->exec();
    delete patientInputForm;
    delete app;
    return 0;
}
