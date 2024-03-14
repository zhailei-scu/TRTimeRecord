#ifndef TRTIMEOPERATOR_H
#define TRTIMEOPERATOR_H

#include <QWidget>
#include <QMenuBar>
#include <QButtonGroup>
#include <QDialogButtonBox>
#include <QDialog>

namespace Ui{
    class TRTimeOperator;
};

class TRTimeOperator: public QWidget{
    Q_OBJECT;

public:
    TRTimeOperator(QWidget* parent = nullptr);
    virtual ~TRTimeOperator();

private:
    Ui::TRTimeOperator *uiForm;


private:
    QMenuBar *menuBar = NULL;
    QButtonGroup *buttonGroup = NULL;

private:
    std::map<unsigned int,QAbstractButton*> *buttonsMap = NULL;
    std::map<unsigned int,QString> patientInfoRecord;
    std::map<unsigned int,QString> buttonTimeRecord;
    QString lastTableName;

private:
    void uiConstruct();
    void uiDeconstruct();
    void menuBarConstruct();
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
