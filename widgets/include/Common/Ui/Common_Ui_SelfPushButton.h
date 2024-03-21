#ifndef COMMON_UI_SELFPUSHBUTTON_H
#define COMMON_UI_SELFPUSHBUTTON_H
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



#endif // COMMON_UI_SELFPUSHBUTTON_H
