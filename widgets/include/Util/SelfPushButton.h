#ifndef SELFPUSHBUTTON_H
#define SELFPUSHBUTTON_H
#include <QPushButton>

class SelfPushButton:public QPushButton{
    Q_OBJECT

public:
    SelfPushButton(QWidget *parent = nullptr);
    virtual ~SelfPushButton();

    void mousePressEvent(QMouseEvent *e) override;

Q_SIGNALS:
    void sign_handlePressEvent(QObject* obj);
};



#endif // SELFPUSHBUTTON_H
