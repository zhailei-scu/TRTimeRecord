#ifndef TRTIMEOPERATOR_H
#define TRTIMEOPERATOR_H

#include <QWidget>

namespace Ui{
    class TRTimeOperator;
};

class TRTimeOperator: public QWidget{
    Q_OBJECT;

public:
    TRTimeOperator(QWidget* parent = nullptr);
    virtual ~TRTimeOperator();
private slots:
    void on_pushButton_clicked();

    void on_pushButton_2_clicked();

private:
    Ui::TRTimeOperator *uiForm;

    void setupUi(TRTimeOperator*);
};

#endif
