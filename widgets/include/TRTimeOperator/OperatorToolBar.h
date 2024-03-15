#ifndef OPERATORTOOLBAR_H
#define OPERATORTOOLBAR_H
#include "../../include/TRTimeOperator/TRTimeOperator_Interface.h"
#include <QToolBar>

class OperatorToolBar:public QToolBar{
public:
    OperatorToolBar(TRTimeOperator_Interface* parent = NULL);
    virtual ~OperatorToolBar();
public:
    void uiConstruct(const QRect & geometry);
    void uiDeConstruct();
};

#endif // OPERATORTOOLBAR_H
