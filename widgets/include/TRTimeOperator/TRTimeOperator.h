#ifndef TRTIMEOPERATOR_H
#define TRTIMEOPERATOR_H

#include "TRTimeOperator_Interface.h"
#include "OperatorMenuBar.h"
#include "OperatorToolBar.h"
#include <QWidget>
#include <QButtonGroup>
#include <QSqlTableModel>
#include <QSqlRecord>


namespace Ui{
    class TRTimeOperator;
};

class TRTimeOperator: public TRTimeOperator_Interface{
    Q_OBJECT;

public:
    TRTimeOperator(TRTimeOperator_Interface* parent = nullptr);
    virtual ~TRTimeOperator();

private:
    Ui::TRTimeOperator *uiForm;

private:
    OperatorMenuBar *menuBar = NULL;
    OperatorToolBar *toolBar = NULL;
    QButtonGroup *buttonGroup = NULL;

private:
    std::map<unsigned int,QAbstractButton*> *buttonsMap = NULL;
    std::map<unsigned int,QString> patientInfoRecord;
    std::map<unsigned int,QString> buttonTimeRecord;
    QString lastTableName;

private:
    virtual void uiConstruct();
    virtual void uiDeconstruct();
private:
    void buttonConstruct();
    void clear();

private:
    bool timeRecord(unsigned int);
    void changeButtonStatus(unsigned int);
    void queryForNextPatient();

/*
signals:
    void buttonGroup_Click();
*/
private slots:
    void HandleSignal(int ID);
    void removeTable(int index);
    void dataView();
    void appendARow(QObject * obj);
};


#endif
