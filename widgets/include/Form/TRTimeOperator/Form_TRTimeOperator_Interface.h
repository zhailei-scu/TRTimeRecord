#ifndef FORM_TRTIMEOPERATOR_INTERFACE_H
#define FORM_TRTIMEOPERATOR_INTERFACE_H

#include <QWidget>

class TRTimeOperator_Interface:public QWidget{
public:
    TRTimeOperator_Interface(QWidget* parent = NULL);
    virtual ~TRTimeOperator_Interface();
public:
    virtual void uiConstruct() = 0;
    virtual void uiDeconstruct() = 0;
};

#endif // FORM_TRTIMEOPERATOR_INTERFACE_H
