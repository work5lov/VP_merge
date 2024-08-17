#ifndef LRIDRWINGCONSTANTS_H
#define LRIDRWINGCONSTANTS_H

#include <QVector>
#include "perechenDrwingConstants.h"

QVector<QVector<double>> A4_ListRegistration = {
    {20, 5, 20, 292, Basicline},
    {20, 292, 205, 292, Basicline},
    {205, 292, 205, 5, Basicline},
    {205, 5, 20, 5, Basicline},
    {20, 45, 205, 45, Basicline},
    {20, 16, 205, 16, Basicline},
    {30, 24, 93, 24, Basicline},
    {30, 277, 30, 16, Basicline},
    {45.75, 277, 45.75, 24, Basicline},
    {61.5, 277, 61.5, 24, Basicline},
    {77.25, 277, 77.25, 24, Basicline},
    {93, 277, 93, 16, Basicline},
    {118, 277, 118, 16, Basicline},
    {140, 277, 140, 16, Basicline},
    {173, 277, 173, 16, Basicline},
    {189, 277, 189, 16, Basicline},
    {20, 45, 205, 45, Thinline},
    {20, 53, 205, 53, Thinline},
    {20, 61, 205, 61, Thinline},
    {20, 69, 205, 69, Thinline},
    {20, 77, 205, 77, Thinline},
    {20, 85, 205, 85, Thinline},
    {20, 93, 205, 93, Thinline},
    {20, 101, 205, 101, Thinline},
    {20, 109, 205, 109, Thinline},
    {20, 117, 205, 117, Thinline},
    {20, 125, 205, 125, Thinline},
    {20, 133, 205, 133, Thinline},
    {20, 141, 205, 141, Thinline},
    {20, 149, 205, 149, Thinline},
    {20, 157, 205, 157, Thinline},
    {20, 165, 205, 165, Thinline},
    {20, 173, 205, 173, Thinline},
    {20, 181, 205, 181, Thinline},
    {20, 189, 205, 189, Thinline},
    {20, 197, 205, 197, Thinline},
    {20, 205, 205, 205, Thinline},
    {20, 213, 205, 213, Thinline},
    {20, 221, 205, 221, Thinline},
    {20, 229, 205, 229, Thinline},
    {20, 237, 205, 237, Thinline},
    {20, 245, 205, 245, Thinline},
    {20, 253, 205, 253, Thinline},
    {20, 261, 205, 261, Thinline},
    {20, 269, 205, 269, Thinline}
};

QVector<TextElement> A4_ListRegistration_textElements = {
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

#endif // LRIDRWINGCONSTANTS_H
