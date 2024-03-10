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
    std::map<int,QAbstractButton*> *buttonsMap = NULL;

private:
    void setupUi(TRTimeOperator*);
    void clear();

private:
    void timeRecord(int);
    void changeButtonStatus(int);

/*
signals:
    void buttonGroup_Click();
*/
private slots:
    void HandleSignal(int ID);
};

#endif
