#ifndef FORM_TRTIMEOPERATOR_H
#define FORM_TRTIMEOPERATOR_H

#include "Form_TRTimeOperator_Interface.h"
#include "../PatientInput/Form_PatientInput.h"
#include "../../Storage/DAO/Storage_DAO_Viewer.h"
#include "../../Storage/CSV/Storage_CSV_Viewer.h"
#include "Form_TRTimeOperator_MenuBar.h"
#include "Form_TRTimeOperator_ToolBar.h"
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
    TRTimeOperator(const TRTimeOperator &) = delete;
    const TRTimeOperator & operator = (const TRTimeOperator &) = delete;

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
    DAOViewer *daoViewer = NULL;
    CSVViewer *csvViewer = NULL;

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
    void inputPatientInfo();

/*
signals:
    void buttonGroup_Click();
*/
private slots:
    void HandleSignal(int ID);
    void removeTable(int index);
    void dataBaseView();
    void csvView();
    void storageSetting();
    void patientInfoSetting();
    void pipleLineSetting();
};


#endif