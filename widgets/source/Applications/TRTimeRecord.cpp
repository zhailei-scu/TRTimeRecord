#include <QApplication>
#include <QFile>
#include <QString>
#include <QSplashScreen>
#include <QQuickWidget>
#include <windows.h>
#include <QQuickItem>
#include <QQmlApplicationEngine>
#include "../../include/Form/TRTimeOperator/Form_TRTimeOperator.h"
#include "../../include/Storage/DAO/Storage_DAO_Sqlite.h"
#include "../../include/Storage/CSV/Storage_CSV_Writer.h"
#include "../../include/Global/Config/Global_Config_ConfigLoader.h"

int main(int argc,char** argv){
    /*Start QT compents*/
    QApplication *app = new QApplication(argc,argv);
    /*Loadding GUI*/
    QQmlApplicationEngine engine;
    engine.load(QUrl(QStringLiteral("qrc:/SplanGUI.ui.qml")));
    engine.clearComponentCache();
    //Sleep(2000);
    /*
    QQuickWidget * splanQml = new QQuickWidget;
    splanQml->setSource(QUrl::fromLocalFile(":/WelcomeGUI.qml"));
    splanQml->setResizeMode(QQuickWidget::SizeRootObjectToView );
    splanQml->setClearColor(QColor(Qt::transparent));
    splanQml->show();
    */
    //Sleep(2000);
    //splan->close();
    //delete splan;
    //splan = NULL;
    /*
    QPixmap pix(":/img/StartGUI.bmp");
    QSplashScreen splan(pix);
    splan.show();
    splan.
    Sleep(2000);
    splan.close();
    */

    /*Style sheet*/
    QFile qss(":/style/QSS/MacOS.qss");
    qss.open(QFile::ReadOnly);
    app->setStyleSheet(qss.readAll());
    qss.close();
    //PatientInput *patientInputForm = new PatientInput(nullptr);
    //patientInputForm->show();

    /*Start configuration*/
    ConfigLoader::Start();
    /*StartDataBase*/
    DAO::Start();
    CSVWriter::Start();

    /*main frame*/
    TRTimeOperator *trTimeOperatorForm = new TRTimeOperator(nullptr);
    trTimeOperatorForm->show();

    app->exec();
    //delete patientInputForm;
    /*
    delete trTimeOperatorForm;
    */
    delete app;
    return 0;
}
