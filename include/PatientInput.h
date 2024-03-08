#ifndef PATIENTINPUT_H
#define PATIENTINPUT_H

#include <QWidget>

class PatientInput: public QWidget{
public:
    PatientInput(QWidget* parent = nullptr);
    virtual ~PatientInput();
private:
    void setupUI();
};

#endif
