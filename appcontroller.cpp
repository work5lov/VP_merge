#include "appcontroller.h"
#include "docpainter.h"
#include <QMessageBox>

AppController::AppController(QObject *parent) : QObject(parent) {}

void AppController::createVP(const QString &filename) {
    DocPainter painter;
    painter.createVP(filename);
    QMessageBox::information(nullptr, "VP Merge", "Файл VP создан");
}

void AppController::exportToPdf() {
    PainterPrinter printer;
    printer.printToPdf();
    QMessageBox::information(nullptr, "VP Merge", "Экспорт в PDF выполнен");
}