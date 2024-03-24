#ifndef FORM_PATIENTINPUT_H
#define FORM_PATIENTINPUT_H

#include <QDialog>
#include <QLabel>
#include <QLineEdit>
#include <map>

using patientInfoQtCompentsPair = std::pair<QLabel*,QLineEdit*>;

class PatientInput: public QDialog{
    Q_OBJECT;

public:
    PatientInput(QWidget* parent = nullptr);
    virtual ~PatientInput();
    PatientInput(const PatientInput & ) = delete;
    const PatientInput & operator = (const PatientInput &) = delete;

private:
    std::map<unsigned int,patientInfoQtCompentsPair>* patternCompents = NULL;
    std::map<unsigned int,QString>* infos = NULL;

private:
    double scrolbarVerticalMax = 0.0;
    double scrolbarHorizontalMax = 0.0;

private:
    void closeEvent(QCloseEvent *) override;

    void clearPatternCompents();
    void clearInfos();
    void clear();
public slots:
    void acceptHandle();
    void rejectHandle();
    void scrollVertical(int value);
    void scrollHorizontal(int value);
};

#endif
