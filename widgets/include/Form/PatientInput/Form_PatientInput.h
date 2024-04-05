#ifndef FORM_PATIENTINPUT_H
#define FORM_PATIENTINPUT_H

#include <QDialog>
#include <QLabel>
#include <QLineEdit>
#include <map>


enum PatientInputMode{
    Modify = 0,
    ViewAndModify = 1
};

using patientInfoQtCompentsPair = std::pair<QLabel*,QLineEdit*>;

class PatientInput: public QDialog{
    Q_OBJECT;

public:
    PatientInput(QWidget* parent = nullptr,std::map<unsigned int,std::pair<QString,QString>> * info = NULL,PatientInputMode model = PatientInputMode(Modify));
    virtual ~PatientInput();
    PatientInput(const PatientInput & ) = delete;
    const PatientInput & operator = (const PatientInput &) = delete;

private:
    std::map<unsigned int,patientInfoQtCompentsPair>* patternCompents = NULL;
    std::map<unsigned int,QString>* infos = NULL;
    QPushButton *buttonOK = NULL;
    QPushButton *buttonCancle = NULL;
    PatientInputMode currentMode = PatientInputMode(Modify);

private:
    void closeEvent(QCloseEvent *) override;

    void clearPatternCompents();
    void clearInfos();
    void clear();
public slots:
    void acceptHandle();
    void rejectHandle();
public:
    const std::map<unsigned int,QString>* getInfos() const;
    PatientInputMode getCurrentMode() const;
};

#endif
