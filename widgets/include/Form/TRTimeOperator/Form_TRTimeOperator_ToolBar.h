#ifndef FORM_TRTIMEOPERATOR_TOOLBAR_H
#define FORM_TRTIMEOPERATOR_TOOLBAR_H

#include "Form_TRTimeOperator_Interface.h"
#include <QToolBar>

class OperatorToolBar:public QToolBar{
public:
    OperatorToolBar(TRTimeOperator_Interface* parent = NULL);
    virtual ~OperatorToolBar();
public:
    void uiConstruct(const QRect & geometry);
    void uiDeConstruct();
};

#endif // FORM_TRTIMEOPERATOR_TOOLBAR_H
