#include <iostream>
#include <QApplication>

int main(int argc,char** argv){
    QApplication *app = new QApplication(argc,argv);



    app->exec();
    delete app;
    return 0;
}
