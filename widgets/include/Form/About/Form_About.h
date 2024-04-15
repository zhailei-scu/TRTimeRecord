#ifndef FORM_ABOUT_H
#define FORM_ABOUT_H

#include <QDialog>

namespace Ui{
class About;
}

class About:public QDialog{
    Q_OBJECT

public:
    About() = delete;
    About(QWidget* parent = NULL);
    virtual ~About();

private:
    Ui::About* uiForm;
};

#endif // FORM_ABOUT_H
