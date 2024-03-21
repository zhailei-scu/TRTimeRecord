#ifndef FORM_TRTIMEOPERATOR_QUERYFORNEXTPATIENT_H
#define FORM_TRTIMEOPERATOR_QUERYFORNEXTPATIENT_H

#include "Form_TRTimeOperator_Interface.h"
#include <QDialog>

class QueryNextPatientDialog: public QDialog{
    Q_OBJECT

public:
    QueryNextPatientDialog() = delete;
    QueryNextPatientDialog(TRTimeOperator_Interface * parent = NULL);
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


#endif // FORM_TRTIMEOPERATOR_QUERYFORNEXTPATIENT_H
