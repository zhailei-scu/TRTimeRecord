#ifndef STORAGE_DAO_VIEWER_H
#define STORAGE_DAO_VIEWER_H

#include "../../Common/Ui/Common_Ui_SelfPushButton.h"
#include <map>
#include <QWidget>
#include <QTableView>
#include <QSqlTableModel>

struct OneDataTableViewerCompents{
public:
    OneDataTableViewerCompents();
    virtual ~OneDataTableViewerCompents();
    OneDataTableViewerCompents(const OneDataTableViewerCompents &);
    const OneDataTableViewerCompents & operator = (const OneDataTableViewerCompents &);

    QTableView* tableView = NULL;
    QSqlTableModel* model = NULL;
    SelfPushButton* insertButton = NULL;
    SelfPushButton* deleteButton = NULL;
private:
    void clear();

};

//using OneDataTableViewer = std::map<QWidget*,OneDataTableViewerCompents*>;

struct OneDataTableViewer{
public:
    OneDataTableViewer();
    virtual ~OneDataTableViewer();
    OneDataTableViewer(const OneDataTableViewer &);
    const OneDataTableViewer & operator =(const OneDataTableViewer &);

public:
    std::map<QWidget*,OneDataTableViewerCompents*> oneViewer;

public:
    void insertOneLeaf(QWidget*,OneDataTableViewerCompents*);
private:
    void clear();
};

class DAOViewer:public QObject{
    Q_OBJECT
public:
    explicit DAOViewer(QObject* parent = NULL);
    virtual ~DAOViewer();
    Q_INVOKABLE explicit DAOViewer(const DAOViewer &);
    const DAOViewer & operator = (const DAOViewer &);
public:
    /*Viewer tree*/
    std::map<QTabWidget*,OneDataTableViewer*> * viewTree = NULL;

    std::map<QObject*,QSqlTableModel*> *map_Button_SqlTableModel = NULL;
    std::map<QObject*,QTableView*> *map_Button_TableView = NULL;
public:
    void insertOneLeaf(QTabWidget*,OneDataTableViewer*);
    void deleteOneLeaf(QTabWidget*);
    void bindOneButtonToSqlModel(QObject*,QSqlTableModel*);
    void bindOneButtonToTableView(QObject*,QTableView*);
    void clear();

public slots:
    void deleteRow(QObject *obj);
    void appendARow(QObject * obj);
};

#endif // STORAGE_DAO_VIEWER_H
