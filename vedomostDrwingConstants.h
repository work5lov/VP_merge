#ifndef VEDOMOSTDRWINGCONSTANTS_H
#define VEDOMOSTDRWINGCONSTANTS_H

#include <QVector>
#include "perechenDrwingConstants.h"

QVector<QVector<double>> ramka_A3_VP = {
    {20, 5, 20, 292, Basicline},
    {20, 292, 415, 292, Basicline},
    {415, 292, 415, 5, Basicline},
    {415, 5, 20, 5, Basicline}
};

QVector<TextElement> Title_block_List1_TextDocuments_textData_A3 = {
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

QVector<QVector<double>> Top_block_VP = {
    {327, -14, 391, -14, Basicline},
    {20, -32, 415, -32, Basicline},
    {27, -32, 27, -5, Basicline},
    {87, -32, 87, -5, Basicline},
    {132, -32, 132, -5, Basicline},
    {202, -32, 202, -5, Basicline},
    {257, -32, 257, -5, Basicline},
    {327, -32, 327, -5, Basicline},
    {343, -32, 343, -14, Basicline},
    {359, -32, 359, -14, Basicline},
    {375, -32, 375, -14, Basicline},
    {391, -32, 391, -5, Basicline}
};

QVector<TextElement> Top_block_VP_textElements = {
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

QVector<QVector<double>> Lines_A3_List1 = {
    {20, 73, 415, 73, Thinline},
    {20, 81, 415, 81, Thinline},
    {20, 89, 415, 89, Thinline},
    {20, 97, 415, 97, Thinline},
    {20, 105, 415, 105, Thinline},
    {20, 113, 415, 113, Thinline},
    {20, 121, 415, 121, Thinline},
    {20, 129, 415, 129, Thinline},
    {20, 137, 415, 137, Thinline},
    {20, 145, 415, 145, Thinline},
    {20, 153, 415, 153, Thinline},
    {20, 161, 415, 161, Thinline},
    {20, 169, 415, 169, Thinline},
    {20, 177, 415, 177, Thinline},
    {20, 185, 415, 185, Thinline},
    {20, 193, 415, 193, Thinline},
    {20, 201, 415, 201, Thinline},
    {20, 209, 415, 209, Thinline},
    {20, 217, 415, 217, Thinline},
    {20, 225, 415, 225, Thinline},
    {20, 233, 415, 233, Thinline},
    {20, 241, 415, 241, Thinline},
    {20, 249, 415, 249, Thinline},
    {20, 257, 415, 257, Thinline},
    {27, 73, 27, 265, Basicline},
    {87, 73, 87, 265, Basicline},
    {132, 73, 132, 265, Basicline},
    {202, 73, 202, 265, Basicline},
    {257, 73, 257, 265, Basicline},
    {327, 73, 327, 265, Basicline},
    {343, 73, 343, 265, Basicline},
    {359, 73, 359, 265, Basicline},
    {375, 73, 375, 265, Basicline},
    {391, 73, 391, 265, Basicline}
};

QVector<QVector<double>> Lines_A3_List2 = {
    {20, 5.5, 20, 292, Basicline},
    {20, 292, 415, 292, Basicline},
    {415, 292, 415, 5, Basicline},
    {415, 5, 20, 5, Basicline},
    {20, 33, 415, 33, Thinline},
    {20, 41, 415, 41, Thinline},
    {20, 49, 415, 49, Thinline},
    {20, 57, 415, 57, Thinline},
    {20, 65, 415, 65, Thinline},
    {20, 73, 415, 73, Thinline},
    {20, 81, 415, 81, Thinline},
    {20, 89, 415, 89, Thinline},
    {20, 97, 415, 97, Thinline},
    {20, 105, 415, 105, Thinline},
    {20, 113, 415, 113, Thinline},
    {20, 121, 415, 121, Thinline},
    {20, 129, 415, 129, Thinline},
    {20, 137, 415, 137, Thinline},
    {20, 145, 415, 145, Thinline},
    {20, 153, 415, 153, Thinline},
    {20, 161, 415, 161, Thinline},
    {20, 169, 415, 169, Thinline},
    {20, 177, 415, 177, Thinline},
    {20, 185, 415, 185, Thinline},
    {20, 193, 415, 193, Thinline},
    {20, 201, 415, 201, Thinline},
    {20, 209, 415, 209, Thinline},
    {20, 217, 415, 217, Thinline},
    {20, 225, 415, 225, Thinline},
    {20, 233, 415, 233, Thinline},
    {20, 241, 415, 241, Thinline},
    {20, 249, 415, 249, Thinline},
    {20, 257, 415, 257, Thinline},
    {27, 33, 27, 265, Basicline},
    {87, 33, 87, 265, Basicline},
    {132, 33, 132, 265, Basicline},
    {202, 33, 202, 265, Basicline},
    {257, 33, 257, 265, Basicline},
    {327, 33, 327, 265, Basicline},
    {343, 33, 343, 265, Basicline},
    {359, 33, 359, 265, Basicline},
    {375, 33, 375, 265, Basicline},
    {391, 33, 391, 265, Basicline}
};

#endif // VEDOMOSTDRWINGCONSTANTS_H
