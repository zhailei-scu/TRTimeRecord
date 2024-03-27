#ifndef FORM_OPERATIONLOOP_H
#define FORM_OPERATIONLOOP_H

#include "../../../include/Global/Config/Global_Config_ConfigLoader.h"
#include <QDialog>
#include <map>
#include <QString>

namespace Ui{
class OperationLoop;
};

class OperationLoop: public QDialog{
    Q_OBJECT

public:
    OperationLoop() = delete;
    OperationLoop(QWidget * parent = NULL,unsigned int buttonID = 0);
    virtual ~OperationLoop();

private:
    unsigned int buttonID = 0;
    unsigned int doneLoopIndex = 0;
    OneOperationPattern onePattern;
    QString textBeforePause;
private:
    Ui::OperationLoop *uiForm;

private:
    void timeRecord();

private slots:
    void nextIndexHandle();
    void abortHandle();
    void pauseHandle();
};


#endif // FORM_OPERATIONLOOP_H