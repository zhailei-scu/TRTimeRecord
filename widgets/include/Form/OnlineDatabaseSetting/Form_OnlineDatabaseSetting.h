#ifndef FORM_ONLINEDATABASESETTING_H
#define FORM_ONLINEDATABASESETTING_H

#include <QDialog>

namespace Ui{
class OnlineDatabaseSetting;
};

class OnlineDatabaseSetting:public QDialog{
    Q_OBJECT

public:
    OnlineDatabaseSetting() = delete;
    OnlineDatabaseSetting(QWidget* parent = NULL);
    virtual ~OnlineDatabaseSetting();

private:
    Ui::OnlineDatabaseSetting *uiForm;

private:
    virtual void closeEvent(QCloseEvent *event) override;
private slots:
    void SaveConfigHandle();
    void CancelHandle();
};

#endif // FORM_ONLINEDATABASESETTING_H
