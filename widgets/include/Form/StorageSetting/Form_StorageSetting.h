#ifndef FORM_STORAGESETTING_H
#define FORM_STORAGESETTING_H

#include <QDialog>

namespace Ui{
class StorageSetting;
};

class StorageSetting:public QDialog{
    Q_OBJECT

public:
    StorageSetting() = delete;
    StorageSetting(QWidget* parent = NULL);
    virtual ~StorageSetting();

private:
    Ui::StorageSetting *uiForm;

private:
    QString theSettingPath;

private:
    virtual void closeEvent(QCloseEvent *event) override;
private slots:
    void SaveConfigHandle();
    void CancelHandle();
    void openFileDialog();
};

#endif // FORM_STORAGESETTING_H
