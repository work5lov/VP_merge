#ifndef SPECIFICATIONDRWINGCONTANTS_H
#define SPECIFICATIONDRWINGCONTANTS_H

#include <QVector>
#include "perechenDrwingConstants.h"

double BasiclineSP = 0.6;
double ThinlineSP = 0.2;

//struct TextElement {
//    double x1;      // Координата x верхнего левого угла
//    double y1;      // Координата y верхнего левого угла
//    double size;    // Размер шрифта
//    double angle;   // Угол поворота текста
//    QString position;   // Позиция текста (HVCenter, и т.д.)
//    QString var;    // Переменная (для vtext)
//    QString content;    // Текстовое содержимое элемента
//    double width;   // Ширина элемента (для текстовых элементов, где указана)
//};

QVector<QVector<double>> Top_block_SP = {
    {26, -5, 26, -20, BasiclineSP},
    {32, -5, 32, -20, BasiclineSP},
    {40, -5, 40, -20, BasiclineSP},
    {110, -5, 110, -20, BasiclineSP},
    {173, -5, 173, -20, BasiclineSP},
    {183, -5, 183, -20, BasiclineSP},
    {20, -20, 205, -20, BasiclineSP}
};

QVector<TextElement> Top_block_SPtextElements = {
    {24, -20, 3.5, 90, "HVCenter", "", "Формат", 16},
    {30, -17, 3.5, 90, "HVCenter", "", "Зона", 0},
    {37, -16, 3.5, 90, "HVCenter", "", "Поз.", 0},
    {63, -14, 3.5, 0, "HVCenter", "", "Обозначение", 0},
    {130, -14, 3.5, 0, "HVCenter", "", "Наименование", 0},
    {179, -16, 3.5, 90, "HVCenter", "", "Кол.", 0},
    {188.5, -11, 3.5, 0, "HVCenter", "", "Приме-", 0},
    {190, -16, 3.5, 0, "HVCenter", "", "чание", 0}
};

QVector<QVector<double>> Lines_A4_SP_25lines_List1 = {
    {20, 269, 205, 269, ThinlineSP},
    {20, 261, 205, 261, ThinlineSP},
    {20, 253, 205, 253, ThinlineSP},
    {20, 245, 205, 245, ThinlineSP},
    {20, 237, 205, 237, ThinlineSP},
    {20, 229, 205, 229, ThinlineSP},
    {20, 221, 205, 221, ThinlineSP},
    {20, 213, 205, 213, ThinlineSP},
    {20, 205, 205, 205, ThinlineSP},
    {20, 197, 205, 197, ThinlineSP},
    {20, 189, 205, 189, ThinlineSP},
    {20, 181, 205, 181, ThinlineSP},
    {20, 173, 205, 173, ThinlineSP},
    {20, 165, 205, 165, ThinlineSP},
    {20, 157, 205, 157, ThinlineSP},
    {20, 149, 205, 149, ThinlineSP},
    {20, 141, 205, 141, ThinlineSP},
    {20, 133, 205, 133, ThinlineSP},
    {20, 125, 205, 125, ThinlineSP},
    {20, 117, 205, 117, ThinlineSP},
    {20, 109, 205, 109, ThinlineSP},
    {20, 101, 205, 101, ThinlineSP},
    {20, 93, 205, 93, ThinlineSP},
    {20, 85, 205, 85, ThinlineSP},
    {20, 77, 205, 77, ThinlineSP},
    {26, 77, 26, 277, BasiclineSP},
    {32, 77, 32, 277, BasiclineSP},
    {40, 77, 40, 277, BasiclineSP},
    {110, 77, 110, 277, BasiclineSP},
    {173, 77, 173, 277, BasiclineSP},
    {183, 77, 183, 277, BasiclineSP}
};

QVector<QVector<double>> Lines_A4_SP_25lines_List2 = {
    {20, 269, 205, 269, ThinlineSP},
    {20, 261, 205, 261, ThinlineSP},
    {20, 253, 205, 253, ThinlineSP},
    {20, 245, 205, 245, ThinlineSP},
    {20, 237, 205, 237, ThinlineSP},
    {20, 229, 205, 229, ThinlineSP},
    {20, 221, 205, 221, ThinlineSP},
    {20, 213, 205, 213, ThinlineSP},
    {20, 205, 205, 205, ThinlineSP},
    {20, 197, 205, 197, ThinlineSP},
    {20, 189, 205, 189, ThinlineSP},
    {20, 181, 205, 181, ThinlineSP},
    {20, 173, 205, 173, ThinlineSP},
    {20, 165, 205, 165, ThinlineSP},
    {20, 157, 205, 157, ThinlineSP},
    {20, 149, 205, 149, ThinlineSP},
    {20, 141, 205, 141, ThinlineSP},
    {20, 133, 205, 133, ThinlineSP},
    {20, 125, 205, 125, ThinlineSP},
    {20, 117, 205, 117, ThinlineSP},
    {20, 109, 205, 109, ThinlineSP},
    {20, 101, 205, 101, ThinlineSP},
    {20, 93, 205, 93, ThinlineSP},
    {20, 85, 205, 85, ThinlineSP},
    {20, 77, 205, 77, ThinlineSP},
    {20, 69, 205, 69, ThinlineSP},
    {20, 61, 205, 61, ThinlineSP},
    {20, 53, 205, 53, ThinlineSP},
    {20, 45, 205, 45, ThinlineSP},
    {20, 37, 205, 37, ThinlineSP},
    {20, 29, 205, 29, ThinlineSP},
    {26, 29, 26, 277, BasiclineSP},
    {32, 29, 32, 277, BasiclineSP},
    {40, 29, 40, 277, BasiclineSP},
    {110, 29, 110, 277, BasiclineSP},
    {173, 29, 173, 277, BasiclineSP},
    {183, 29, 183, 277, BasiclineSP}
};


#endif // SPECIFICATIONDRWINGCONTANTS_H
