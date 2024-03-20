#ifndef PATIENTINFOSETTING_H
#define PATIENTINFOSETTING_H

#include <QDialog>
#include <QMenu>

namespace Ui{
class PatientInfoSetting;
};

class PatientInfoSetting: public QDialog{
    Q_OBJECT;

public:
    PatientInfoSetting(QWidget* parent = nullptr);
    virtual ~PatientInfoSetting();
    PatientInfoSetting(const PatientInfoSetting & ) = delete;
    const PatientInfoSetting & operator = (const PatientInfoSetting &) = delete;

private:
    QMenu* rightClickMenu = NULL;

private:
    virtual void closeEvent(QCloseEvent *event) override;

private:
    Ui::PatientInfoSetting *uiForm;

private slots:
    void insertUpAction();
    void insertDownAction();
    void deleteAction();
    void resetAction();
    void execRightMenu(const QPoint &);
};


#endif // PATIENTINFOSETTING_H
