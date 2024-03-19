#ifndef CSVVIEWER_H
#define CSVVIEWER_H

#include <QTableWidget>

struct OneCSVViewerCompents{
public:
    OneCSVViewerCompents();
    virtual ~OneCSVViewerCompents();
    OneCSVViewerCompents(const OneCSVViewerCompents &);
    const OneCSVViewerCompents & operator = (const OneCSVViewerCompents &);

    QTableWidget* tableView = NULL;
private:
    void clear();

};

//using OneDataTableViewer = std::map<QWidget*,OneDataTableViewerCompents*>;

struct OneCSVViewer{
public:
    OneCSVViewer();
    virtual ~OneCSVViewer();
    OneCSVViewer(const OneCSVViewer &);
    const OneCSVViewer & operator =(const OneCSVViewer &);

public:
    std::map<QWidget*,OneCSVViewerCompents*> oneViewer;

public:
    void insertOneLeaf(QWidget*,OneCSVViewerCompents*);
private:
    void clear();
};

class CSVViewer:public QObject{
    Q_OBJECT
public:
    explicit CSVViewer(QObject* parent = NULL);
    virtual ~CSVViewer();
    Q_INVOKABLE explicit CSVViewer(const CSVViewer &);
    const CSVViewer & operator = (const CSVViewer &);
public:
    /*Viewer tree*/
    std::map<QTabWidget*,OneCSVViewer*> * viewTree = NULL;
public:
    void insertOneLeaf(QTabWidget*,OneCSVViewer*);
    void deleteOneLeaf(QTabWidget*);
    void clear();
};


#endif // CSVVIEWER_H
