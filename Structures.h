#ifndef STRUCTURES_H
#define STRUCTURES_H

#include <QMainWindow>

struct peline {
    QString num;
    QString oboz;
    QString name;
    QString comment;
    bool underline;
    QString type;
};

struct vpline {
    QString num;
    QString oboz;
    QString kod;
    QString name;
    QString comment;
    bool underline;
    QString type;
    bool merge;
    QString vhodit;
    QString post;
};

struct specline {
    QString num;
    QString oboz;
    QString name;
    QString comment;
    bool underline;
    QString type;
    bool merge;
    bool needtu;
    QString GroupLine1;
    QString GroupLine2;
    QString format;
    QString pos;
    QString posinelement;
};

struct Page {
    QList<peline> pipelines;
    QList<vpline> pivplines;
    QList<specline> pispeclines;
};

struct PageContainer {
    QList<Page> pages;
};

#endif // STRUCTURES_H
