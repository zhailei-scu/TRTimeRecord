#ifndef FORM_TRTIMEOPERATOR_TOOLBAR_H
#define FORM_TRTIMEOPERATOR_TOOLBAR_H

#include "Form_TRTimeOperator_Interface.h"
#include <QToolBar>
#include <QToolButton>

class OperatorToolBar:public QToolBar{
public:
    OperatorToolBar(TRTimeOperator_Interface* parent = NULL);
    virtual ~OperatorToolBar();
public:
    void uiConstruct(const QRect & geometry);
    void uiDeConstruct();

    std::map<QString,QToolButton*>* toolsMap = NULL;
};

#endif // FORM_TRTIMEOPERATOR_TOOLBAR_H
