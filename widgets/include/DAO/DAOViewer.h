#ifndef DAOVIEWER_H
#define DAOVIEWER_H

#include "../../include/Util/SelfPushButton.h"
#include <map>
#include <QWidget>
#include <QTableView>
#include <QSqlTableModel>

struct OneDataTableViewer{
public:
    OneDataTableViewer();
    virtual ~OneDataTableViewer();
    OneDataTableViewer(const OneDataTableViewer &);
    const OneDataTableViewer & operator = (const OneDataTableViewer &);

    QWidget* mainWidget = NULL;
    QTableView* tableView = NULL;
    QSqlTableModel* model = NULL;
    SelfPushButton* insertButton = NULL;
    SelfPushButton* deleteButton = NULL;
private:
    void clear();

};

class DAOViewer{
public:
    DAOViewer();
    virtual ~DAOViewer();
    DAOViewer(const DAOViewer &);
    const DAOViewer & operator = (const DAOViewer &);
public:
    /*Viewer tree*/
    std::map<QTabWidget*,OneDataTableViewer*> * viewTree = NULL;

    std::map<QObject*,QSqlTableModel*> *map_InsertButton_SqlTableModel = NULL;
    std::map<QObject*,QSqlTableModel*> *map_DeletetButton_SqlTableModel = NULL;
private:
    void clear();
};

#endif // DAOVIEWER_H
