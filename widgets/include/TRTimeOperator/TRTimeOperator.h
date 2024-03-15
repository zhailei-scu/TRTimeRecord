#ifndef TRTIMEOPERATOR_H
#define TRTIMEOPERATOR_H

#include "TRTimeOperator_Interface.h"
#include <QMenuBar>
#include <QToolBar>
#include <QButtonGroup>
#include <QDialogButtonBox>
#include <QDialog>

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
    QMenuBar *menuBar = NULL;
    QToolBar *toolBar = NULL;
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
    void menuBarConstruct();
    void toolBarConstruct();
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
};

class QueryNextPatientDialog: public QDialog{
    Q_OBJECT

public:
    QueryNextPatientDialog() = delete;
    QueryNextPatientDialog(TRTimeOperator * parent = NULL);
    virtual ~QueryNextPatientDialog();

private:
    QPushButton *nextPatient = NULL;
    QPushButton *deleteRecord = NULL;

private:
    void closeEvent(QCloseEvent *) override;

private slots:
    void nextPatientHandle();
    void deleteRecordHandle();
};

#endif
