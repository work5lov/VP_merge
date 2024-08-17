#ifndef PAINTERPRINTER_H
#define PAINTERPRINTER_H

#include <QObject>

class painterPrinter : public QObject
{
    Q_OBJECT
public:
    explicit painterPrinter(QObject *parent = nullptr);

signals:

};

#endif // PAINTERPRINTER_H
