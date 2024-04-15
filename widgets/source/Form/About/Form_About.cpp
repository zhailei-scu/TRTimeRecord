#include "../../../include/Form/About/Form_About.h"
#include "ui_About.h"

About::About(QWidget* parent):QDialog(parent),uiForm(new Ui::About){
    uiForm->setupUi(this);
}

About::~About(){

}
