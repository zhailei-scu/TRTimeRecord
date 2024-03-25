#ifndef FORM_OPERATIONPIPELINESETTING_H
#define FORM_OPERATIONPIPELINESETTING_H

#include <QDialog>
#include <QMenu>

namespace Ui{
class OperationPipelineSetting;
};

class OperationPipelineSetting: public QDialog{
    Q_OBJECT;

public:
    OperationPipelineSetting(QWidget* parent = nullptr);
    virtual ~OperationPipelineSetting();
    OperationPipelineSetting(const OperationPipelineSetting & ) = delete;
    const OperationPipelineSetting & operator = (const OperationPipelineSetting &) = delete;

private:
    QMenu* rightClickMenu = NULL;

private:
    virtual void closeEvent(QCloseEvent *event) override;
    bool setOperationPipelineSetting();
private:
    Ui::OperationPipelineSetting *uiForm;

private slots:
    void insertUpAction();
    void insertDownAction();
    void deleteAction();
    void resetAction();
    void execRightMenu(const QPoint &);
};


#endif // FORM_OPERATIONPIPELINESETTING_H
