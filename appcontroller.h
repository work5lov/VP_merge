#ifndef APPCONTROLLER_H
#define APPCONTROLLER_H

#include <QObject>
#include <QString>

class AppController : public QObject
{
    Q_OBJECT
public:
    explicit AppController(QObject *parent = nullptr);

    Q_INVOKABLE void createVP(const QString &filename);
    Q_INVOKABLE void exportToPdf();
    // Добавьте дополнительные методы при необходимости
};

#endif // APPCONTROLLER_H