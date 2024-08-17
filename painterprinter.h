#ifndef PAINTERPRINTER_H
#define PAINTERPRINTER_H

#include <QObject>
#include <QPainter>
#include <QPen>
#include <QSlider>
#include <QFile>
#include <QXmlStreamReader>
#include <QDebug>
#include <QtPrintSupport/QPrinter>
#include <QtPrintSupport/QPrintDialog>
#include <QPageSize>
#include "Structures.h"
#include "perechenDrwingConstants.h"
#include "specificationDrwingContants.h"
#include "lriDrwingConstants.h"
#include "vedomostDrwingConstants.h"

class painterPrinter : public QObject
{
    Q_OBJECT

public:
    explicit painterPrinter(QObject *parent = nullptr);
    void printDocPDF(QString docType, PageContainer pageContainer);

signals:

protected:
//    void paintEvent(QPaintEvent *event);

private:

    // Вспомогательные методы для печати и отрисовки
    void drawText(QPainter &painter, const TextElement &textElements, bool xMirror, bool yMirror, double marginX, double marginY);
    void drawTextPrint(QPainter &painter, const TextElement &textElements, bool xMirror, bool yMirror, double marginX, double marginY);
    void drawTextFromMatrix(QPainter &painter, const QVector<TextElement> &textMatrix, bool xMirror, bool yMirror, double marginX, double marginY);
    void drawTextFromMatrixPrint(QPainter &painter, const QVector<TextElement> &textMatrix, bool xMirror, bool yMirror, double marginX, double marginY);
    void drawTextInCenter(QPainter &painter, const QRect &rect, const TextElement &textElements);
    void drawTextInCenter(QPainter &painter, const QRect &rect, const QString &text);
    double mm_to_points(double mm);
    double mm_to_points_print(double mm);
    void drawTextTitle(QPainter &painter, const QRect &rect, const QString &text);
    void drawTextInCenter(QPainter &painter, const QRect &rect, const QString &text, double textSize);

    // Методы для отрисовки
//    void updateCanvas();
    void drawSquare(QPainter &painter, const QVector<double> &squareData, bool xMirror, bool yMirror);
    void drawSquarePrint(QPainter &painter, const QVector<double> &squareData, bool xMirror, bool yMirror);
    void drawSquaresFromMatrix(QPainter &painter, const QVector<QVector<double>> &squaresMatrix, bool xMirror, bool yMirror);
    void drawSquaresFromMatrixPrint(QPainter &painter, const QVector<QVector<double>> &squaresMatrix, bool xMirror, bool yMirror);
    void drawFirstPagePE(QPainter &painter);
    void drawOtherPagePE(QPainter &painter);
    void drawFirstPageSP(QPainter &painter);
    void drawOtherPageSP(QPainter &painter);
    void drawFirstPageVP(QPainter &painter);
    void drawOtherPageVP(QPainter &painter);
    void drawLRIPage(QPainter &painter);

    // Методы для печати
    void drawPage(QPainter &painter, QString docType, int pageNumber);
    void drawFirstPagePEPrint(QPainter &painter);
    void drawOtherPagePEPrint(QPainter &painter);
    void drawFirstPageSPPrint(QPainter &painter);
    void drawOtherPageSPPrint(QPainter &painter);
    void drawFirstPageVPPrint(QPainter &painter);
    void drawOtherPageVPPrint(QPainter &painter);
    void drawLRIPagePrint(QPainter &painter);

    void drawContentPE(QPainter &painter, PageContainer pageContainer, int pageNumber);

    int _xOffsetPE, _yOffsetPE, _xOffsetSP, _yOffsetSP, _xOffsetVP, _yOffsetVP;
    float scale_factorPE;
};

#endif // PAINTERPRINTER_H
