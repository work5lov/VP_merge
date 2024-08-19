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
//#include "perechenDrwingConstants.h"
//#include "specificationDrwingContants.h"
//#include "lriDrwingConstants.h"
//#include "vedomostDrwingConstants.h"

class painterPrinter : public QObject
{
    Q_OBJECT

public:
    explicit painterPrinter(QObject *parent = nullptr);
    void printDocPDF(QString docType, PageContainer pageContainer);

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

signals:

protected:
//    void paintEvent(QPaintEvent *event);

private:

    double Basicline1 = 0.6;
    double Thinline1 = 0.2;

    struct TextElement1 {
        double x1;      // Координата x верхнего левого угла
        double y1;      // Координата y верхнего левого угла
        double size;    // Размер шрифта
        double angle;   // Угол поворота текста
        QString position;   // Позиция текста (HVCenter, и т.д.)
        QString var;    // Переменная (для vtext)
        QString content;    // Текстовое содержимое элемента
        double width;   // Ширина элемента (для текстовых элементов, где указана)
    };

    //List1 PE
    QVector<QVector<double>> ramka_A4_PE1 = {
        {20, 5, 20, 292, Basicline1},
        {20, 292, 205, 292, Basicline1},
        {205, 292, 205, 5, Basicline1},
        {205, 5, 20, 5, Basicline1}
    };

    QVector<QVector<double>> Title_block_List1vp_TextDocuments1 = {
        { 125, 0, 125, 22, Basicline1 },          // Первая линия
        { 111, 8, 111, 22, Basicline1 },          // Вторая линия
        { 58, 8, 58, 22, Basicline1 },            // Третья линия
        { 125, 8, 5, 8, Basicline1 },     // Четвёртая линия
        { 125, 22, 5, 22, Basicline1 }    // Пятая линия
    };

    QVector<TextElement1> Title_block_List1_TextDocuments1_textData_A41_PE1 = {
        { -50.5, -26.4, 3.5, 0, "HVCenter", "", "Лит.", 0 },
        { -114.467, -0.771, 3.5, 0, "", "", "Копировал", 0 },
        { -36.5, -0.771, 3.5, 0, "", "", "Формат А4", 0 },
        { -109, -9, 3.5, 0, "", "", "Перечень элементов", 17 },
        { -134.5, -31.5, 3.5, 0, "HVCenter", "", "Дата", 10 },
        { -147, -31.5, 3.5, 0, "HVCenter", "", "Подп.", 0 },
        { -170, -31.5, 3.5, 0, "HVCenter", "", "№ докум.", 0 },
        { -182.5, -31.5, 3.5, 0, "HVCenter", "", "Лист", 10 },
        { -190, -31.5, 3.5, 0, "HVCenter", "", "Изм.", 8 },
        { -189.467, -25.771, 3.5, 0, "", "", "Разраб.", 17 },
        { -189.467, -20.771, 3.5, 0, "", "", "Пров.", 17 },
        { -189.467, -15.771, 3.5, 0, "", "Должность", "", 17 },
        { -189.467, -10.771, 3.5, 0, "", "", "Н.контр.", 17 },
        { -189.467, -5.771, 3.5, 0, "", "", "Утв.", 17 },
        { -36.5, -26.4, 3.5, 0, "HVCenter", "", "Лист", 0 },
        { -21, -26.4, 3.5, 0, "HVCenter", "", "Листов", 0 }
    };

    QVector<TextElement1> Title_block_List1_TextDocuments1_textData_A41 = {
        { -50.5, -26.4, 3.5, 0, "HVCenter", "", "Лит.", 0 },
        { -114.467, -0.771, 3.5, 0, "", "", "Копировал", 0 },
        { -36.5, -0.771, 3.5, 0, "", "", "Формат А4", 0 },
        { -134.5, -31.5, 3.5, 0, "HVCenter", "", "Дата", 10 },
        { -147, -31.5, 3.5, 0, "HVCenter", "", "Подп.", 0 },
        { -170, -31.5, 3.5, 0, "HVCenter", "", "№ докум.", 0 },
        { -182.5, -31.5, 3.5, 0, "HVCenter", "", "Лист", 10 },
        { -190, -31.5, 3.5, 0, "HVCenter", "", "Изм.", 8 },
        { -189.467, -25.771, 3.5, 0, "", "", "Разраб.", 17 },
        { -189.467, -20.771, 3.5, 0, "", "", "Пров.", 17 },
        { -189.467, -15.771, 3.5, 0, "", "Должность", "", 17 },
        { -189.467, -10.771, 3.5, 0, "", "", "Н.контр.", 17 },
        { -189.467, -5.771, 3.5, 0, "", "", "Утв.", 17 },
        { -36.5, -26.4, 3.5, 0, "HVCenter", "", "Лист", 0 },
        { -21, -26.4, 3.5, 0, "HVCenter", "", "Листов", 0 }
    };

    QVector<QVector<double>> Title_block_List1_TextDocuments1 = {
        { 190, 5.5, 190, 45, Basicline1 },   // Первая линия
        { 190, 10, 125, 10, Thinline1 },   // Вторая линия
        { 190, 15, 125, 15, Thinline1 },   // Третья линия
        { 190, 20, 125, 20, Thinline1 },   // Четвёртая линия
        { 190, 25, 125, 25, Thinline1 },   // Пятая линия
        { 190, 30, 5, 30, Basicline1 },    // Шестая линия
        { 190, 35, 125, 35, Basicline1 },  // Седьмая линия
        { 190, 40, 125, 40, Thinline1 },   // Восьмая линия
        { 190, 45, 5, 45, Basicline1 },    // Девятая линия
        { 55, 20, 5, 20, Basicline1 },     // Десятая линия
        { 55, 25, 5, 25, Basicline1 },     // Одиннадцатая линия
        { 183, 30, 183, 45, Basicline1 },  // Двенадцатая линия
        { 173, 5.5, 173, 45, Basicline1 },   // Тринадцатая линия
        { 150, 5.5, 150, 45, Basicline1 },   // Четырнадцатая линия
        { 135, 5.5, 135, 45, Basicline1 },   // Пятнадцатая линия
        { 125, 5.5, 125, 45, Basicline1 },   // Шестнадцатая линия
        { 55, 5.5, 55, 30, Basicline1 },     // Семнадцатая линия
        { 50, 20, 50, 25, Thinline1 },     // Восемнадцатая линия
        { 45, 20, 45, 25, Thinline1 },     // Девятнадцатая линия
        { 40, 20, 40, 30, Basicline1 },    // Двадцатая линия
        { 25, 20, 25, 30, Basicline1 }     // Двадцать первая линия
    };

    QVector<QVector<double>> Left_block_List11 = {
        { 13, 172.4, 13, 292.4, Basicline1 },  // Первая линия
        { 13, 5.4, 13, 150.4, Basicline1 },    // Вторая линия
        { 8, 292.4, 20, 292.4, Basicline1 },   // Третья линия
        { 8, 232.4, 20, 232.4, Basicline1 },   // Четвёртая линия
        { 8, 172.4, 8, 292.4, Basicline1 },    // Пятая линия
        { 8, 172.4, 20, 172.4, Basicline1 },   // Шестая линия
        { 8, 150.4, 20, 150.4, Basicline1 },   // Седьмая линия
        { 8, 115.4, 20, 115.4, Basicline1 },   // Восьмая линия
        { 8, 90.4, 20, 90.4, Basicline1 },     // Девятая линия
        { 8, 65.4, 20, 65.4, Basicline1 },     // Десятая линия
        { 8, 30.4, 20, 30.4, Basicline1 },     // Одиннадцатая линия
        { 8, 5.4, 8, 150.4, Basicline1 },      // Двенадцатая линия
        { 8, 5.4, 20, 5.4, Basicline1 }        // Тринадцатая линия 5
    };

    QVector<TextElement1> Left_block_List11_textElements = {
        {11.5, 247.4, 3.5, 90, "HVCenter90", "", "Перв. примен.", 59},
        {11.5, 195.4, 3.5, 90, "HVCenter90", "", "Справ. №", 59},
        {11.5, 120.4, 3.5, 90, "HVCenter90", "", "Подп. и дата", 33.5},
        {11.5, 90.9, 3.5, 90, "HVCenter90", "", "Инв. № дубл.", 23.5},
        {11.5, 66.4, 3.5, 90, "HVCenter90", "", "Взам. инв. №", 23.5},
        {11.5, 36.4, 3.5, 90, "HVCenter90", "", "Подп. и дата", 33.5},
        {11.5, 6.9, 3.5, 90, "HVCenter90", "", "Инв. № подл.", 23.5}
    };

    QVector<QVector<double>> Top_block_PE1 = {
        { 20, 20, 205, 20, Basicline1 },   // Первая линия
        { 40, 5, 40, 20, Basicline1 },     // Вторая линия
        { 150, 5, 150, 20, Basicline1 },   // Третья линия
        { 160, 5, 160, 20, Basicline1 }    // Четвёртая линия
    };

    QVector<TextElement1> Top_block_PE1_textElements = {
        {26, 9, 3.5, 0, "HVCenter", "", "Поз.", 0},
        {23, 14, 3.5, 0, "HVCenter", "", "обозна-", 0},
        {25, 19, 3.5, 0, "HVCenter", "", "чение", 0},
        {87, 14, 3.5, 0, "HVCenter", "", "Наименование", 0},
        {150.5, 14, 3.5, 0, "HVCenter", "", "Кол.", 0},
        {172, 14, 3.5, 0, "HVCenter", "", "Примечание", 0}
    };

    QVector<QVector<double>> Lines_A4_PE_3lines_List11 = {
        { 20, 69, 205, 69, Thinline1 },   // Первая линия
        { 20, 61, 205, 61, Thinline1 },   // Вторая линия
        { 20, 53, 205, 53, Thinline1 },   // Третья линия
        { 40, 53, 40, 77, Basicline1 },   // Четвёртая линия
        { 150, 53, 150, 77, Basicline1 }, // Пятая линия
        { 160, 53, 160, 77, Basicline1 }  // Шестая линия
    };

    QVector<QVector<double>> Lines_A4_PE_25lines_List11 = {
        { 20, 269, 205, 269, Thinline1 },   // Первая линия
        { 20, 261, 205, 261, Thinline1 },   // Вторая линия
        { 20, 253, 205, 253, Thinline1 },   // Третья линия
        { 20, 245, 205, 245, Thinline1 },   // Четвёртая линия
        { 20, 237, 205, 237, Thinline1 },   // Пятая линия
        { 20, 229, 205, 229, Thinline1 },   // Шестая линия
        { 20, 221, 205, 221, Thinline1 },   // Седьмая линия
        { 20, 213, 205, 213, Thinline1 },   // Восьмая линия
        { 20, 205, 205, 205, Thinline1 },   // Девятая линия
        { 20, 197, 205, 197, Thinline1 },   // Десятая линия
        { 20, 189, 205, 189, Thinline1 },   // Одиннадцатая линия
        { 20, 181, 205, 181, Thinline1 },   // Двенадцатая линия
        { 20, 173, 205, 173, Thinline1 },   // Тринадцатая линия
        { 20, 165, 205, 165, Thinline1 },   // Четырнадцатая линия
        { 20, 157, 205, 157, Thinline1 },   // Пятнадцатая линия
        { 20, 149, 205, 149, Thinline1 },   // Шестнадцатая линия
        { 20, 141, 205, 141, Thinline1 },   // Семнадцатая линия
        { 20, 133, 205, 133, Thinline1 },   // Восемнадцатая линия
        { 20, 125, 205, 125, Thinline1 },   // Девятнадцатая линия
        { 20, 117, 205, 117, Thinline1 },   // Двадцатая линия
        { 20, 109, 205, 109, Thinline1 },   // Двадцать первая линия
        { 20, 101, 205, 101, Thinline1 },   // Двадцать вторая линия
        { 20, 93, 205, 93, Thinline1 },     // Двадцать третья линия
        { 20, 85, 205, 85, Thinline1 },     // Двадцать четвёртая линия
        { 20, 77, 205, 77, Thinline1 },     // Двадцать пятая линия
        { 40, 77, 40, 277, Basicline1 },    // Двадцать шестая линия
        { 150, 77, 150, 277, Basicline1 },  // Двадцать седьмая линия
        { 160, 77, 160, 277, Basicline1 }   // Двадцать восьмая линия
    };

    //List2 PE
    QVector<QVector<double>> Title_block_List21 = {
        {-15, 13, -5, 13, Basicline1},
        {-15, 5.5, -15, 20, Basicline1},
        {-125, 5.5, -125, 20, Basicline1},
        {-135, 10, -125, 10, Basicline1},
        {-135, 5.5, -135, 20, Basicline1},
        {-150, 10, -135, 10, Basicline1},
        {-150, 5.5, -150, 20, Basicline1},
        {-173, 10, -150, 10, Basicline1},
        {-173, 5.5, -173, 20, Basicline1},
        {-183, 10, -173, 10, Basicline1},
        {-183, 5.5, -183, 20, Basicline1},
        {-190, 20, -5, 20, Basicline1},
        {-190, 15, -125, 15, Thinline1},
        {-190, 10, -183, 10, Basicline1},
        {-190, 5.5, -190, 20, Basicline1}
    };

    QVector<TextElement1> Title_block_List21_textElements_A4 = {
        {-134.5, 6, 3.5, 0, "HCenter", "", "Дата", 10},
        {-147, 6, 3.5, 0, "HCenter", "", "Подп.", 0},
        {-170, 6, 3.5, 0, "HCenter", "", "№ докум.", 0},
        {-190, 6, 3.5, 0, "HCenter", "", "Изм.", 8},
        {-182.5, 6, 3.5, 0, "HCenter", "", "Лист", 10},
        {-114.467, 0.771, 3.5, 0, "", "", "Копировал", 0},
        {-36.5, 0.771, 3.5, 0, "", "", "Формат А4", 0 },
        {-14.5, 14.5, 3.5, 0, "HCenter", "", "Лист", 10}
    };

    QVector<TextElement1> Title_block_List21_textElements_A3 = {
        {-134.5, 6, 3.5, 0, "HCenter", "", "Дата", 10},
        {-147, 6, 3.5, 0, "HCenter", "", "Подп.", 0},
        {-170, 6, 3.5, 0, "HCenter", "", "№ докум.", 0},
        {-190, 6, 3.5, 0, "HCenter", "", "Изм.", 8},
        {-182.5, 6, 3.5, 0, "HCenter", "", "Лист", 10},
        {-114.467, 0.771, 3.5, 0, "", "", "Копировал", 0},
        {-36.5, 0.771, 3.5, 0, "", "", "Формат А3", 0 },
        {-14.5, 14.5, 3.5, 0, "HCenter", "", "Лист", 10}
    };

    QVector<QVector<double>> Left_block_List21 = {
        {13, 5.4, 13, 150.4, Basicline1},
        {8, 150.4, 20, 150.4, Basicline1},
        {8, 115.4, 20, 115.4, Basicline1},
        {8, 90.4, 20, 90.4, Basicline1},
        {8, 65.4, 20, 65.4, Basicline1},
        {8, 30.4, 20, 30.4, Basicline1},
        {8, 5.4, 8, 150.4, Basicline1},
        {8, 5.4, 20, 5.4, Basicline1}
    };

    QVector<TextElement1> Left_block_List21_textElements = {
        {11.5, 120, 3.5, 90, "HVCenter90", "", "Подп. и дата", 33.5},
        {11.5, 90.5, 3.5, 90, "HVCenter90", "", "Инв. № дубл.", 23.5},
        {11.5, 66, 3.5, 90, "HVCenter90", "", "Взам. инв. №", 23.5},
        {11.5, 36, 3.5, 90, "HVCenter90", "", "Подп. и дата", 33.5},
        {11.5, 6.5, 3.5, 90, "HVCenter90", "", "Инв. № подл.", 23.5}
    };

    QVector<QVector<double>> Lines_A4_PE_List21 = {
        {20, 269, 205, 269, Thinline1},
        {20, 261, 205, 261, Thinline1},
        {20, 253, 205, 253, Thinline1},
        {20, 245, 205, 245, Thinline1},
        {20, 237, 205, 237, Thinline1},
        {20, 229, 205, 229, Thinline1},
        {20, 221, 205, 221, Thinline1},
        {20, 213, 205, 213, Thinline1},
        {20, 205, 205, 205, Thinline1},
        {20, 197, 205, 197, Thinline1},
        {20, 189, 205, 189, Thinline1},
        {20, 181, 205, 181, Thinline1},
        {20, 173, 205, 173, Thinline1},
        {20, 165, 205, 165, Thinline1},
        {20, 157, 205, 157, Thinline1},
        {20, 149, 205, 149, Thinline1},
        {20, 141, 205, 141, Thinline1},
        {20, 133, 205, 133, Thinline1},
        {20, 125, 205, 125, Thinline1},
        {20, 117, 205, 117, Thinline1},
        {20, 109, 205, 109, Thinline1},
        {20, 101, 205, 101, Thinline1},
        {20, 93, 205, 93, Thinline1},
        {20, 85, 205, 85, Thinline1},
        {20, 77, 205, 77, Thinline1},
        {20, 69, 205, 69, Thinline1},
        {20, 61, 205, 61, Thinline1},
        {20, 53, 205, 53, Thinline1},
        {20, 45, 205, 45, Thinline1},
        {20, 37, 205, 37, Thinline1},
        {20, 29, 205, 29, Thinline1},
        {40, 29, 40, 277, Basicline1},
        {150, 29, 150, 277, Basicline1},
        {160, 29, 160, 277, Basicline1}
    };

    double Basicline1SP = 0.6;
    double Thinline1SP = 0.2;

    //struct TextElement1 {
    //    double x1;      // Координата x верхнего левого угла
    //    double y1;      // Координата y верхнего левого угла
    //    double size;    // Размер шрифта
    //    double angle;   // Угол поворота текста
    //    QString position;   // Позиция текста (HVCenter, и т.д.)
    //    QString var;    // Переменная (для vtext)
    //    QString content;    // Текстовое содержимое элемента
    //    double width;   // Ширина элемента (для текстовых элементов, где указана)
    //};

    QVector<QVector<double>> Top_block_SP1 = {
        {26, -5, 26, -20, Basicline1SP},
        {32, -5, 32, -20, Basicline1SP},
        {40, -5, 40, -20, Basicline1SP},
        {110, -5, 110, -20, Basicline1SP},
        {173, -5, 173, -20, Basicline1SP},
        {183, -5, 183, -20, Basicline1SP},
        {20, -20, 205, -20, Basicline1SP}
    };

    QVector<TextElement1> Top_block_SP1textElements = {
        {24, -20, 3.5, 90, "HVCenter", "", "Формат", 16},
        {30, -17, 3.5, 90, "HVCenter", "", "Зона", 0},
        {37, -16, 3.5, 90, "HVCenter", "", "Поз.", 0},
        {63, -14, 3.5, 0, "HVCenter", "", "Обозначение", 0},
        {130, -14, 3.5, 0, "HVCenter", "", "Наименование", 0},
        {179, -16, 3.5, 90, "HVCenter", "", "Кол.", 0},
        {188.5, -11, 3.5, 0, "HVCenter", "", "Приме-", 0},
        {190, -16, 3.5, 0, "HVCenter", "", "чание", 0}
    };

    QVector<QVector<double>> Lines_A4_SP_25lines_List11 = {
        {20, 269, 205, 269, Thinline1SP},
        {20, 261, 205, 261, Thinline1SP},
        {20, 253, 205, 253, Thinline1SP},
        {20, 245, 205, 245, Thinline1SP},
        {20, 237, 205, 237, Thinline1SP},
        {20, 229, 205, 229, Thinline1SP},
        {20, 221, 205, 221, Thinline1SP},
        {20, 213, 205, 213, Thinline1SP},
        {20, 205, 205, 205, Thinline1SP},
        {20, 197, 205, 197, Thinline1SP},
        {20, 189, 205, 189, Thinline1SP},
        {20, 181, 205, 181, Thinline1SP},
        {20, 173, 205, 173, Thinline1SP},
        {20, 165, 205, 165, Thinline1SP},
        {20, 157, 205, 157, Thinline1SP},
        {20, 149, 205, 149, Thinline1SP},
        {20, 141, 205, 141, Thinline1SP},
        {20, 133, 205, 133, Thinline1SP},
        {20, 125, 205, 125, Thinline1SP},
        {20, 117, 205, 117, Thinline1SP},
        {20, 109, 205, 109, Thinline1SP},
        {20, 101, 205, 101, Thinline1SP},
        {20, 93, 205, 93, Thinline1SP},
        {20, 85, 205, 85, Thinline1SP},
        {20, 77, 205, 77, Thinline1SP},
        {26, 77, 26, 277, Basicline1SP},
        {32, 77, 32, 277, Basicline1SP},
        {40, 77, 40, 277, Basicline1SP},
        {110, 77, 110, 277, Basicline1SP},
        {173, 77, 173, 277, Basicline1SP},
        {183, 77, 183, 277, Basicline1SP}
    };

    QVector<QVector<double>> Lines_A4_SP_25lines_List21 = {
        {20, 269, 205, 269, Thinline1SP},
        {20, 261, 205, 261, Thinline1SP},
        {20, 253, 205, 253, Thinline1SP},
        {20, 245, 205, 245, Thinline1SP},
        {20, 237, 205, 237, Thinline1SP},
        {20, 229, 205, 229, Thinline1SP},
        {20, 221, 205, 221, Thinline1SP},
        {20, 213, 205, 213, Thinline1SP},
        {20, 205, 205, 205, Thinline1SP},
        {20, 197, 205, 197, Thinline1SP},
        {20, 189, 205, 189, Thinline1SP},
        {20, 181, 205, 181, Thinline1SP},
        {20, 173, 205, 173, Thinline1SP},
        {20, 165, 205, 165, Thinline1SP},
        {20, 157, 205, 157, Thinline1SP},
        {20, 149, 205, 149, Thinline1SP},
        {20, 141, 205, 141, Thinline1SP},
        {20, 133, 205, 133, Thinline1SP},
        {20, 125, 205, 125, Thinline1SP},
        {20, 117, 205, 117, Thinline1SP},
        {20, 109, 205, 109, Thinline1SP},
        {20, 101, 205, 101, Thinline1SP},
        {20, 93, 205, 93, Thinline1SP},
        {20, 85, 205, 85, Thinline1SP},
        {20, 77, 205, 77, Thinline1SP},
        {20, 69, 205, 69, Thinline1SP},
        {20, 61, 205, 61, Thinline1SP},
        {20, 53, 205, 53, Thinline1SP},
        {20, 45, 205, 45, Thinline1SP},
        {20, 37, 205, 37, Thinline1SP},
        {20, 29, 205, 29, Thinline1SP},
        {26, 29, 26, 277, Basicline1SP},
        {32, 29, 32, 277, Basicline1SP},
        {40, 29, 40, 277, Basicline1SP},
        {110, 29, 110, 277, Basicline1SP},
        {173, 29, 173, 277, Basicline1SP},
        {183, 29, 183, 277, Basicline1SP}
    };

    QVector<QVector<double>> ramka_A3_VP1 = {
        {20, 5, 20, 292, Basicline1},
        {20, 292, 415, 292, Basicline1},
        {415, 292, 415, 5, Basicline1},
        {415, 5, 20, 5, Basicline1}
    };

    QVector<TextElement1> Title_block_List1_TextDocuments1_textData_A3 = {
        { -50.5, -26.4, 3.5, 0, "HVCenter", "", "Лит.", 0 },
        { -114.467, -0.771, 3.5, 0, "", "", "Копировал", 0 },
        { -36.5, -0.771, 3.5, 0, "", "", "Формат А3", 0 },
        { -116, -9, 3.5, 0, "", "", "Ведомость покупных изднлий", 17 },//-116
        { -134.5, -31.5, 3.5, 0, "HVCenter", "", "Дата", 10 },
        { -147, -31.5, 3.5, 0, "HVCenter", "", "Подп.", 0 },
        { -170, -31.5, 3.5, 0, "HVCenter", "", "№ докум.", 0 },
        { -182.5, -31.5, 3.5, 0, "HVCenter", "", "Лист", 10 },
        { -190, -31.5, 3.5, 0, "HVCenter", "", "Изм.", 8 },
        { -189.467, -25.771, 3.5, 0, "", "", "Разраб.", 17 },
        { -189.467, -20.771, 3.5, 0, "", "", "Пров.", 17 },
        { -189.467, -15.771, 3.5, 0, "", "Должность", "", 17 },
        { -189.467, -10.771, 3.5, 0, "", "", "Н.контр.", 17 },
        { -189.467, -5.771, 3.5, 0, "", "", "Утв.", 17 },
        { -36.5, -26.4, 3.5, 0, "HVCenter", "", "Лист", 0 },
        { -21, -26.4, 3.5, 0, "HVCenter", "", "Листов", 0 }
    };

    QVector<QVector<double>> Top_block_VP1 = {
        {327, -14, 391, -14, Basicline1},
        {20, -32, 415, -32, Basicline1},
        {27, -32, 27, -5, Basicline1},
        {87, -32, 87, -5, Basicline1},
        {132, -32, 132, -5, Basicline1},
        {202, -32, 202, -5, Basicline1},
        {257, -32, 257, -5, Basicline1},
        {327, -32, 327, -5, Basicline1},
        {343, -32, 343, -14, Basicline1},
        {359, -32, 359, -14, Basicline1},
        {375, -32, 375, -14, Basicline1},
        {391, -32, 391, -5, Basicline1}
    };

    QVector<TextElement1> Top_block_VP1_textElements = {
        {25, -26, 3.5, 90, "HVCenter", "", "№ строки", 0},

        {39, -19, 5, 0, "HVCenter", "", "Наименование", 0},

        {105, -14, 5, 0, "HVCenter", "", "Код", 0},
        {95, -23, 5, 0, "HVCenter", "", "продукции", 0},

        {133, -14, 5, 0, "HVCenter", "", "Обозначение документа", 70},
        {149, -23, 5, 0, "HVCenter", "", "на поставку", 0},

        {215, -18.5, 5, 0, "HVCenter", "", "Поставщик", 0},

        {275, -14, 5, 0, "HVCenter", "", "Куда входит", 0},
        {273, -23, 5, 0, "HVCenter", "", "(обозначение)", 0},

        {345, -11, 5, 0, "HVCenter", "", "Количество", 0},

        {329, -20, 3.5, 0, "HVCenter", "", "на из-", 0},
        {330, -26, 3.5, 0, "HVCenter", "", "делие", 0},

        {344, -20, 3.5, 0, "HVCenter", "", "в комп-", 0},
        {345, -26, 3.5, 0, "HVCenter", "", "лекты", 0},

        {361, -20, 3.5, 0, "HVCenter", "", "на ре-", 0},
        {362, -26, 3.5, 0, "HVCenter", "", "гулир.", 0},

        {375.8, -23, 4.9, 0, "HVCenter", "", "Всего", 16},

        {394, -14, 5, 0, "HVCenter", "", "Приме-", 24},
        {395, -23, 5, 0, "HVCenter", "", "чание", 0}
    };

    QVector<QVector<double>> Lines_A3_List11 = {
        {20, 73, 415, 73, Thinline1},
        {20, 81, 415, 81, Thinline1},
        {20, 89, 415, 89, Thinline1},
        {20, 97, 415, 97, Thinline1},
        {20, 105, 415, 105, Thinline1},
        {20, 113, 415, 113, Thinline1},
        {20, 121, 415, 121, Thinline1},
        {20, 129, 415, 129, Thinline1},
        {20, 137, 415, 137, Thinline1},
        {20, 145, 415, 145, Thinline1},
        {20, 153, 415, 153, Thinline1},
        {20, 161, 415, 161, Thinline1},
        {20, 169, 415, 169, Thinline1},
        {20, 177, 415, 177, Thinline1},
        {20, 185, 415, 185, Thinline1},
        {20, 193, 415, 193, Thinline1},
        {20, 201, 415, 201, Thinline1},
        {20, 209, 415, 209, Thinline1},
        {20, 217, 415, 217, Thinline1},
        {20, 225, 415, 225, Thinline1},
        {20, 233, 415, 233, Thinline1},
        {20, 241, 415, 241, Thinline1},
        {20, 249, 415, 249, Thinline1},
        {20, 257, 415, 257, Thinline1},
        {27, 73, 27, 265, Basicline1},
        {87, 73, 87, 265, Basicline1},
        {132, 73, 132, 265, Basicline1},
        {202, 73, 202, 265, Basicline1},
        {257, 73, 257, 265, Basicline1},
        {327, 73, 327, 265, Basicline1},
        {343, 73, 343, 265, Basicline1},
        {359, 73, 359, 265, Basicline1},
        {375, 73, 375, 265, Basicline1},
        {391, 73, 391, 265, Basicline1}
    };

    QVector<QVector<double>> Lines_A3_List21 = {
        {20, 5.5, 20, 292, Basicline1},
        {20, 292, 415, 292, Basicline1},
        {415, 292, 415, 5, Basicline1},
        {415, 5, 20, 5, Basicline1},
        {20, 33, 415, 33, Thinline1},
        {20, 41, 415, 41, Thinline1},
        {20, 49, 415, 49, Thinline1},
        {20, 57, 415, 57, Thinline1},
        {20, 65, 415, 65, Thinline1},
        {20, 73, 415, 73, Thinline1},
        {20, 81, 415, 81, Thinline1},
        {20, 89, 415, 89, Thinline1},
        {20, 97, 415, 97, Thinline1},
        {20, 105, 415, 105, Thinline1},
        {20, 113, 415, 113, Thinline1},
        {20, 121, 415, 121, Thinline1},
        {20, 129, 415, 129, Thinline1},
        {20, 137, 415, 137, Thinline1},
        {20, 145, 415, 145, Thinline1},
        {20, 153, 415, 153, Thinline1},
        {20, 161, 415, 161, Thinline1},
        {20, 169, 415, 169, Thinline1},
        {20, 177, 415, 177, Thinline1},
        {20, 185, 415, 185, Thinline1},
        {20, 193, 415, 193, Thinline1},
        {20, 201, 415, 201, Thinline1},
        {20, 209, 415, 209, Thinline1},
        {20, 217, 415, 217, Thinline1},
        {20, 225, 415, 225, Thinline1},
        {20, 233, 415, 233, Thinline1},
        {20, 241, 415, 241, Thinline1},
        {20, 249, 415, 249, Thinline1},
        {20, 257, 415, 257, Thinline1},
        {27, 33, 27, 265, Basicline1},
        {87, 33, 87, 265, Basicline1},
        {132, 33, 132, 265, Basicline1},
        {202, 33, 202, 265, Basicline1},
        {257, 33, 257, 265, Basicline1},
        {327, 33, 327, 265, Basicline1},
        {343, 33, 343, 265, Basicline1},
        {359, 33, 359, 265, Basicline1},
        {375, 33, 375, 265, Basicline1},
        {391, 33, 391, 265, Basicline1}
    };

    QVector<QVector<double>> A4_ListRegistration1 = {
        {20, 5, 20, 292, Basicline1},
        {20, 292, 205, 292, Basicline1},
        {205, 292, 205, 5, Basicline1},
        {205, 5, 20, 5, Basicline1},
        {20, 45, 205, 45, Basicline1},
        {20, 16, 205, 16, Basicline1},
        {30, 24, 93, 24, Basicline1},
        {30, 277, 30, 16, Basicline1},
        {45.75, 277, 45.75, 24, Basicline1},
        {61.5, 277, 61.5, 24, Basicline1},
        {77.25, 277, 77.25, 24, Basicline1},
        {93, 277, 93, 16, Basicline1},
        {118, 277, 118, 16, Basicline1},
        {140, 277, 140, 16, Basicline1},
        {173, 277, 173, 16, Basicline1},
        {189, 277, 189, 16, Basicline1},
        {20, 45, 205, 45, Thinline1},
        {20, 53, 205, 53, Thinline1},
        {20, 61, 205, 61, Thinline1},
        {20, 69, 205, 69, Thinline1},
        {20, 77, 205, 77, Thinline1},
        {20, 85, 205, 85, Thinline1},
        {20, 93, 205, 93, Thinline1},
        {20, 101, 205, 101, Thinline1},
        {20, 109, 205, 109, Thinline1},
        {20, 117, 205, 117, Thinline1},
        {20, 125, 205, 125, Thinline1},
        {20, 133, 205, 133, Thinline1},
        {20, 141, 205, 141, Thinline1},
        {20, 149, 205, 149, Thinline1},
        {20, 157, 205, 157, Thinline1},
        {20, 165, 205, 165, Thinline1},
        {20, 173, 205, 173, Thinline1},
        {20, 181, 205, 181, Thinline1},
        {20, 189, 205, 189, Thinline1},
        {20, 197, 205, 197, Thinline1},
        {20, 205, 205, 205, Thinline1},
        {20, 213, 205, 213, Thinline1},
        {20, 221, 205, 221, Thinline1},
        {20, 229, 205, 229, Thinline1},
        {20, 237, 205, 237, Thinline1},
        {20, 245, 205, 245, Thinline1},
        {20, 253, 205, 253, Thinline1},
        {20, 261, 205, 261, Thinline1},
        {20, 269, 205, 269, Thinline1}
    };

    QVector<TextElement1> A4_ListRegistration1_textElements = {
        {80, 12, 5, 0, "HVCenter", "", "Лист регистрации изменений", 0},

        {21, 30, 3.5, 0, "HVCenter", "", "Изм.", 11},

        {38, 21, 3.5, 0, "HVCenter", "", "Номера листов (страниц)", 63},

        {33, 29, 3.5, 0, "HVCenter", "", "изме-", 0},
        {34, 34, 3.5, 0, "HVCenter", "", "нен-", 0},
        {35, 39, 3.5, 0, "HVCenter", "", "ных", 0},

        {49, 29, 3.5, 0, "HVCenter", "", "заме-", 0},
        {50, 34, 3.5, 0, "HVCenter", "", "нен-", 0},
        {51, 39, 3.5, 0, "HVCenter", "", "ных", 0},

        {64, 29, 3.5, 0, "HVCenter", "", "новых", 0},

        {80, 29, 3.5, 0, "HVCenter", "", "анну-", 0},
        {80, 33, 3.5, 0, "HVCenter", "", "лиро-", 0},
        {81, 37, 3.5, 0, "HVCenter", "", "ван-", 0},
        {82, 40, 3.5, 0, "HVCenter", "", "ных", 0},

        {100, 22, 3.5, 0, "HVCenter", "", "Всего", 0},
        {99, 29, 3.5, 0, "HVCenter", "", "листов", 0},
        {97, 36, 3.5, 0, "HVCenter", "", "(страниц)", 25},
        {94, 43, 3.5, 0, "HVCenter", "", "в документе", 25},

        {124, 27, 3.5, 0, "HVCenter", "", "Номер", 0},
        {119, 34.5, 3.5, 0, "HVCenter", "", "документа", 22.5},

        {148, 21.5, 3.5, 0, "HVCenter", "", "Входящий", 0},
        {140, 26, 3.5, 0, "HVCenter", "", "номер сопроводи-", 33},
        {148, 30.4, 3.5, 0, "HVCenter", "", "тельного", 0},
        {147, 34.9, 3.5, 0, "HVCenter", "", "документа", 0},
        {150, 39.6, 3.5, 0, "HVCenter", "", "и дата", 0},

        {173.5, 30, 3.5, 0, "HVCenter", "", "Подпись", 15.5},

        {192, 30, 3.5, 0, "HVCenter", "", "Дата", 0}
    };

    // Вспомогательные методы для печати и отрисовки
    void drawText(QPainter &painter, const TextElement1 &textElements, bool xMirror, bool yMirror, double marginX, double marginY);
    void drawTextPrint(QPainter &painter, const TextElement1 &textElements, bool xMirror, bool yMirror, double marginX, double marginY);
    void drawTextFromMatrix(QPainter &painter, const QVector<TextElement1> &textMatrix, bool xMirror, bool yMirror, double marginX, double marginY);
    void drawTextFromMatrixPrint(QPainter &painter, const QVector<TextElement1> &textMatrix, bool xMirror, bool yMirror, double marginX, double marginY);
    void drawTextInCenter(QPainter &painter, const QRect &rect, const TextElement1 &textElements);
    void drawTextInCenter(QPainter &painter, const QRect &rect, const QString &text);
    double mm_to_points(double mm);
    double mm_to_points_print(double mm);
    void drawTextTitle(QPainter &painter, const QRect &rect, const QString &text);
    void drawTextInCenter(QPainter &painter, const QRect &rect, const QString &text, double textSize);

    // Методы для отрисовки
//    void updateCanvas();


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

    float scale_factorPE;
};

#endif // PAINTERPRINTER_H
