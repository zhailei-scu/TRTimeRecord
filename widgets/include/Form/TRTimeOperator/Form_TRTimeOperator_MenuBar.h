#ifndef FORM_TRTIMEOPERATOR_MENUBAR_H
#define FORM_TRTIMEOPERATOR_MENUBAR_H

#include "Form_TRTimeOperator_Interface.h"
#include <QMenuBar>
#include <map>

class OperatorMenuBar:public QMenuBar{
public:
    OperatorMenuBar(TRTimeOperator_Interface* parent = NULL);
    virtual ~OperatorMenuBar();
public:
    void uiConstruct(const QRect & geometry);
    void uiDeConstruct();

private:
    std::map<QString,const QMenu &>* menuMap = NULL;

public:
    const QMenu * getMenu(const QString & name) const;

private:
    void clear();

private slots:
    void dataView();
};


#endif // FORM_TRTIMEOPERATOR_MENUBAR_H
