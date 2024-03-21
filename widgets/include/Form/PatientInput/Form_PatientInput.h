#ifndef FORM_PATIENTINPUT_H
#define FORM_PATIENTINPUT_H

#include <QDialog>

namespace Ui{
    class PatientInput;
};

class PatientInput: public QDialog{
    Q_OBJECT;

public:
    PatientInput(QWidget* parent = nullptr);
    virtual ~PatientInput();
    PatientInput(const PatientInput & ) = delete;
    const PatientInput & operator = (const PatientInput &) = delete;

private:
    Ui::PatientInput *uiForm;

private:
    void closeEvent(QCloseEvent *) override;
public slots:
    void acceptHandle();
    void rejectHandle();
};

#endif
