#ifndef TRTIMEOPERATOR_H
#define TRTIMEOPERATOR_H

#include <QWidget>
#include <QButtonGroup>

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
    QButtonGroup *buttonGroup = NULL;

private:
    std::map<unsigned int,QAbstractButton*> *buttonsMap = NULL;
    std::map<unsigned int,QString> patientInfoRecord;
    std::map<unsigned int,QString> buttonTimeRecord;

private:
    void buttonConstruct();
    void clear();

private:
    void timeRecord(unsigned int);
    void changeButtonStatus(unsigned int);

/*
signals:
    void buttonGroup_Click();
*/
private slots:
    void HandleSignal(int ID);
};

#endif
