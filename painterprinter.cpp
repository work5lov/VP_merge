#include "painterprinter.h"

painterPrinter::painterPrinter(QObject *parent) : QObject(parent)
{

}

void painterPrinter::printDocPDF(QString docType, PageContainer pageContainer)
{
    // Создаем объект QPrinter
    QPrinter printer;
    printer.setOutputFormat(QPrinter::PdfFormat);
    printer.setPageSize(QPrinter::A3);
    printer.setOrientation(QPrinter::Landscape);
    printer.setOutputFileName("output.pdf");

    // Создаем объект QPainter, связанный с QPrinter
    QPainter painter;
    painter.begin(&printer);

    double pageWidth = printer.width();
    double pageHeight = printer.height();

    qDebug() << pageHeight << pageWidth << printer.heightMM() << printer.widthMM();

    // Устанавливаем параметры рисования
    painter.setRenderHint(QPainter::Antialiasing);

    const int pageCount = 5; // Установите количество страниц

    // Отрисовываем каждую страницу
    for (int i = 0; i < pageCount; ++i) {
        if (i > 0) {
            printer.newPage(); // Переходим на новую страницу
        }
        drawPage(painter,"VP", i + 1);
    }

    printer.setPageSize(QPrinter::A4);
    printer.setOrientation(QPrinter::Portrait);
    printer.newPage();
    drawPage(painter,"LRI", 1);
    // Завершаем рисование
    painter.end();
}

void painterPrinter::updateCanvas()
{
    x_offset = 10*ui->slider_x->value();
    y_offset = 50*ui->slider_y->value();
    scale_factor = 100 *ui->slider_scale->value() / 100.0;
    update();
    repaint();
}

void painterPrinter::drawSquare(QPainter &painter, const QVector<double> &squareData, bool xMirror, bool yMirror)
{
    // Переводим размеры из миллиметров в точки
    double signX = 1;
    double signY = 1;
    if(xMirror){
        signX = -1;
    }
    if(yMirror){
        signY = -1;
    }
    double width = signX * mm_to_points(squareData[0]);
    double height = signY * mm_to_points(squareData[1]);
    double x = signX * mm_to_points(squareData[2]);
    double y = signY * mm_to_points(squareData[3]);
    // Задаем толщину линии из пятого столбца
    double lineWidthMm = squareData[4];
    // Проверяем, если ширина и высота равны 0, то рисуем линию
    QPen pen;
    // Установка толщины линии
    pen.setStyle(Qt::SolidLine);
    // Переводим толщину линии из миллиметров в пиксели
    double lineWidthPx = mm_to_points(lineWidthMm);
    // Устанавливаем толщину линии в пикселях
    pen.setWidthF(lineWidthPx);
    pen.setBrush(Qt::NoBrush);
    pen.setColor(Qt::black);
    painter.setPen(pen);
    painter.drawLine(width, height, x, y);
}

void painterPrinter::drawSquarePrint(QPainter &painter, const QVector<double> &squareData, bool xMirror, bool yMirror)
{
    // Переводим размеры из миллиметров в точки
    double signX = 1;
    double signY = 1;
    if(xMirror){
        signX = -1;
    }
    if(yMirror){
        signY = -1;
    }
    double width = signX * mm_to_points_print(squareData[0]);
    double height = signY * mm_to_points_print(squareData[1]);
    double x = signX * mm_to_points_print(squareData[2]);
    double y = signY * mm_to_points_print(squareData[3]);
    // Задаем толщину линии из пятого столбца
    double lineWidthMm = squareData[4];
    // Проверяем, если ширина и высота равны 0, то рисуем линию
    QPen pen;
    // Установка толщины линии
    pen.setStyle(Qt::SolidLine);
    // Переводим толщину линии из миллиметров в пиксели
    double lineWidthPx = mm_to_points_print(lineWidthMm);
    // Устанавливаем толщину линии в пикселях
    pen.setWidthF(lineWidthPx);
    pen.setBrush(Qt::NoBrush);
    pen.setColor(Qt::black);
    painter.setPen(pen);
    painter.drawLine(width, height, x, y);
}

void drawText(QPainter &painter, const TextElement &textElements, bool xMirror, bool yMirror, double marginX, double marginY){
    painter.translate(mm_to_points(marginX),mm_to_points(marginY));
    double signX = 1;
    double signY = 1;
    if(xMirror){
        signX = -1;
    }
    if(yMirror){
        signY = -1;
    }
    double x = signX * mm_to_points(textElements.x1);
    double y = signY * mm_to_points(textElements.y1);
    double angle = textElements.angle;
    QString text = textElements.content;
    // Получаем метрики шрифта
    QFont font("GOST type B", mm_to_points(textElements.size)); // Настраиваем шрифт по желанию
    painter.setFont(font);
    QFontMetrics metrics(font);
    int textWidth = metrics.width(text); // Ширина текста
    int textHeight = metrics.height();   // Высота текста
    // Рассчитываем координаты левого верхнего угла так, чтобы текст был нарисован по центру
    int adjustedX = x - textWidth / 2;
    int adjustedY = y - textHeight / 2;
    if(angle == 90.0){
        painter.rotate(-90); // Указываем отрицательный угол, так как поворот происходит против часовой стрелки
        painter.drawText(-y, x, text); // Рисуем текст по центру прямоугольника
        painter.rotate(90);
    }
    else{
        QFont font("GOST type B", mm_to_points(textElements.size)); // Настраиваем шрифт по желанию
        painter.setFont(font);
        painter.drawText(x, y, text); // Рисуем текст по центру прямоугольника
//        painter.rotate(90);
    }

    painter.translate(mm_to_points(-marginX),mm_to_points(-marginY));
}

void drawTextInCenter(QPainter &painter, const QRect &rect, const TextElement &textElements)
{
    QString text = textElements.content;
    QFont font("GOST type B", mm_to_points(textElements.size)); // Настраиваем шрифт по желанию
    painter.setFont(font);
    // Получение метрик шрифта для вычисления размеров текста
    QFontMetrics metrics(font);
    QRect textRect = metrics.boundingRect(rect, Qt::AlignCenter, text);

    // Рисование текста по центру квадрата
    int x = rect.x() + (rect.width() - textRect.width()) / 2;
    int y = rect.y() + (rect.height() - textRect.height()) / 2;
    painter.drawText(QRect(x, y, textRect.width(), textRect.height()), Qt::AlignCenter, text);
}

void drawTextInCenter(QPainter &painter, const QRect &rect, const QString &text){
    painter.setBrush(Qt::black); // Белый цвет для заполнения
    painter.setPen(Qt::SolidLine); // Нет контура
    QFont font("GOST type B", mm_to_points(3.5)); // Настраиваем шрифт по желанию

    // Получение метрик шрифта для вычисления размеров текста
    QFontMetrics metrics(font);
    int textWidth = metrics.horizontalAdvance(text);
//    qDebug() << "Initial text width:" << textWidth;
//    qDebug() << font.weight();
//    font.setStretch(1);
    painter.setFont(font);
    QRect textRect = metrics.boundingRect(rect, Qt::AlignCenter, text);

    // Рисование текста по центру квадрата
    int x = rect.x() + (rect.width() - textRect.width()) / 2;
    int y = rect.y() + (rect.height() - textRect.height()) / 2;
    painter.drawText(QRect(x, y, textRect.width(), textRect.height()), Qt::AlignCenter, text);
}

void drawTextInCenter(QPainter &painter, const QRect &rect, const QString &text, double textSize){
    painter.setBrush(Qt::black); // Белый цвет для заполнения
    painter.setPen(Qt::SolidLine); // Нет контура
    QFont font("GOST type B", mm_to_points(textSize)); // Настраиваем шрифт по желанию
    font.setWeight(QFont::Medium);
    // Получение метрик шрифта для вычисления размеров текста
    QFontMetrics metrics(font);
    int textWidth = metrics.horizontalAdvance(text);
//    qDebug() << "Initial text width:" << textWidth;
//    qDebug() << font.weight();
//    font.setStretch(1);
    painter.setFont(font);
    QRect textRect = metrics.boundingRect(rect, Qt::AlignCenter, text);

    // Рисование текста по центру квадрата
    int x = rect.x() + (rect.width() - textRect.width()) / 2;
    int y = rect.y() + (rect.height() - textRect.height()) / 2;
    painter.drawText(QRect(x, y, textRect.width(), textRect.height()), Qt::AlignCenter, text);
}

void drawTextTitle(QPainter &painter, const QRect &rect, const QString &text){
    painter.setBrush(Qt::black); // Белый цвет для заполнения
    painter.setPen(Qt::SolidLine); // Нет контура
    QFont font("GOST type B", mm_to_points(3.5)); // Настраиваем шрифт по желанию

    // Получение метрик шрифта для вычисления размеров текста
    QFontMetrics metrics(font);
    int textWidth = metrics.horizontalAdvance(text);
//    qDebug() << "Initial text width:" << textWidth;
    if(textWidth > 59){
        font.setPointSizeF(mm_to_points(2.5));
    }
//    qDebug() << font.weight();
//    font.setStretch(1);
    painter.setFont(font);
    QRect textRect = metrics.boundingRect(rect, Qt::AlignLeft, text);

    // Рисование текста по центру квадрата
    int x = rect.x() /*+ (rect.width() - textRect.width()) / 2*/;
    int y = rect.y() + (rect.height() - textRect.height()) / 2;
    painter.drawText(QRect(x, y, textRect.width(), textRect.height()), Qt::AlignLeft, text);
}

void drawTextPrint(QPainter &painter, const TextElement &textElements, bool xMirror, bool yMirror, double marginX, double marginY){
    painter.translate(mm_to_points_print(marginX),mm_to_points_print(marginY));
    double signX = 1;
    double signY = 1;
    if(xMirror){
        signX = -1;
    }
    if(yMirror){
        signY = -1;
    }
    double x = signX * mm_to_points_print(textElements.x1);
    double y = signY * mm_to_points_print(textElements.y1);
    double angle = textElements.angle;
    QString text = textElements.content;
    // Получаем метрики шрифта
    QFont font("GOST type B", mm_to_points_print(textElements.size)); // Настраиваем шрифт по желанию
    painter.setFont(font);
    QFontMetrics metrics(font);
    int textWidth = metrics.width(text); // Ширина текста
    int textHeight = metrics.height();   // Высота текста
    // Рассчитываем координаты левого верхнего угла так, чтобы текст был нарисован по центру
    int adjustedX = x - textWidth / 2;
    int adjustedY = y - textHeight / 2;
    if(angle == 90.0){
        painter.rotate(-90); // Указываем отрицательный угол, так как поворот происходит против часовой стрелки
        painter.drawText(-y, x, text); // Рисуем текст по центру прямоугольника
        painter.rotate(90);
    }
    else{
        QFont font("GOST type B", mm_to_points_print(textElements.size)); // Настраиваем шрифт по желанию
        painter.setFont(font);
        painter.drawText(x, y, text); // Рисуем текст по центру прямоугольника
//        painter.rotate(90);
    }

    painter.translate(mm_to_points_print(-marginX),mm_to_points_print(-marginY));
}

void painterPrinter::drawSquaresFromMatrix(QPainter &painter, const QVector<QVector<double> > &squaresMatrix, bool xMirror, bool yMirror)
{
    for (const auto &squareData : squaresMatrix) {
        drawSquare(painter, squareData, xMirror, yMirror);
    }
}

void painterPrinter::drawSquaresFromMatrixPrint(QPainter &painter, const QVector<QVector<double> > &squaresMatrix, bool xMirror, bool yMirror)
{
    for (const auto &squareData : squaresMatrix) {
        drawSquarePrint(painter, squareData, xMirror, yMirror);
    }
}

void painterPrinter::drawFirstPagePE(QPainter &painter)
{
    painter.fillRect(0, 0, mm_to_points(210), mm_to_points(297), Qt::white); // x, y - координаты верхнего левого угла, width, height - ширина и высота квадрата
    drawSquaresFromMatrix(painter, ramka_A4_PE, 0, 0);
    painter.translate(mm_to_points(210), mm_to_points(252));
    painter.rotate(-180);
    drawSquaresFromMatrix(painter, Title_block_List1vp_TextDocuments, 0, 0);
    painter.translate(mm_to_points(0), mm_to_points(-45));
    drawSquaresFromMatrix(painter, Title_block_List1_TextDocuments, 0, 0);

    painter.rotate(180);
    painter.translate(mm_to_points(-210), mm_to_points(0));
    drawSquaresFromMatrix(painter, Left_block_List1, 0, 1);
    painter.translate(mm_to_points(0), mm_to_points(-297));
    drawSquaresFromMatrix(painter, Top_block_PE, 0, 0);
    painter.translate(mm_to_points(0), mm_to_points(297));
    drawSquaresFromMatrix(painter, Lines_A4_PE_25lines_List1, 0, 1);
    painter.translate(mm_to_points(0), mm_to_points(-297));//Вернулись на первоначальное положение

    //начинаем писать текст
    drawTextFromMatrix(painter, Title_block_List1_TextDocuments_textData_A4_PE, 0, 0, 210, 297);
    drawTextFromMatrix(painter, Left_block_List1_textElements, 0, 1, 0, 297);
    drawTextFromMatrix(painter, Top_block_PE_textElements, 0, 0, 1, 0);
}

void painterPrinter::drawOtherPagePE(QPainter &painter)
{
    painter.fillRect(0, 0, mm_to_points(210), mm_to_points(297), Qt::white); // x, y - координаты верхнего левого угла, width, height - ширина и высота квадрата
    drawSquaresFromMatrix(painter, ramka_A4_PE, 0, 0);
    painter.translate(mm_to_points(210), mm_to_points(297));
    drawSquaresFromMatrix(painter, Title_block_List2, 0, 1);
    painter.translate(mm_to_points(-210), mm_to_points(0));
    drawSquaresFromMatrix(painter, Left_block_List2, 0, 1);
    painter.translate(mm_to_points(0), mm_to_points(-297));
    drawSquaresFromMatrix(painter, Top_block_PE, 0, 0);
    painter.translate(mm_to_points(0), mm_to_points(297));
    drawSquaresFromMatrix(painter, Lines_A4_PE_List2, 0, 1);
    painter.translate(mm_to_points(0), mm_to_points(-297));//Вернулись на первоначальное положение

    //начинаем писать текст
    drawTextFromMatrix(painter, Title_block_List2_textElements_A4, 0, 1, 210, 297);
    drawTextFromMatrix(painter, Left_block_List2_textElements, 0, 1, 0, 297);
    drawTextFromMatrix(painter, Top_block_PE_textElements, 0, 0, 0, 0);
}

void painterPrinter::drawFirstPageSP(QPainter &painter)
{
    painter.fillRect(0, 0, mm_to_points(210), mm_to_points(297), Qt::white); // x, y - координаты верхнего левого угла, width, height - ширина и высота квадрата
    drawSquaresFromMatrix(painter, ramka_A4_PE, 0, 0);
    painter.translate(mm_to_points(210), mm_to_points(252));
    painter.rotate(-180);
    drawSquaresFromMatrix(painter, Title_block_List1vp_TextDocuments, 0, 0);
    painter.translate(mm_to_points(0), mm_to_points(-45));
    drawSquaresFromMatrix(painter, Title_block_List1_TextDocuments, 0, 0);
    painter.rotate(180);
    painter.translate(mm_to_points(-210), mm_to_points(0));
    drawSquaresFromMatrix(painter, Left_block_List1, 0, 1);
    painter.translate(mm_to_points(0), mm_to_points(-297));
    drawSquaresFromMatrix(painter, Top_block_SP, 0, 1);
    painter.translate(mm_to_points(0), mm_to_points(297));
    drawSquaresFromMatrix(painter, Lines_A4_SP_25lines_List1, 0, 1);
    painter.translate(mm_to_points(0), mm_to_points(-297));//Вернулись на первоначальное положение

    //начинаем писать текст
    drawTextFromMatrix(painter, Title_block_List1_TextDocuments_textData_A4, 0, 0, 210, 297);
    drawTextFromMatrix(painter, Left_block_List1_textElements, 0, 1, 0, 297);
    drawTextFromMatrix(painter, Top_block_SPtextElements, 0, 1, 0, 0);
}

void painterPrinter::drawOtherPageSP(QPainter &painter)
{
    painter.fillRect(0, 0, mm_to_points(210), mm_to_points(297), Qt::white); // x, y - координаты верхнего левого угла, width, height - ширина и высота квадрата
    drawSquaresFromMatrix(painter, ramka_A4_PE, 0, 0);
    painter.translate(mm_to_points(210), mm_to_points(297));
    drawSquaresFromMatrix(painter, Title_block_List2, 0, 1);
    painter.translate(mm_to_points(-210), mm_to_points(0));
    drawSquaresFromMatrix(painter, Left_block_List2, 0, 1);
    painter.translate(mm_to_points(0), mm_to_points(-297));
    drawSquaresFromMatrix(painter, Top_block_SP, 0, 1);
    painter.translate(mm_to_points(0), mm_to_points(297));
    drawSquaresFromMatrix(painter, Lines_A4_SP_25lines_List2, 0, 1);
    painter.translate(mm_to_points(0), mm_to_points(-297));//Вернулись на первоначальное положение

    //начинаем писать текст
    drawTextFromMatrix(painter, Title_block_List2_textElements_A4, 0, 1, 210, 297);
    drawTextFromMatrix(painter, Left_block_List2_textElements, 0, 1, 0, 297);
    drawTextFromMatrix(painter, Top_block_SPtextElements, 0, 1, 0, 0);
}

void painterPrinter::drawFirstPageVP(QPainter &painter)
{
    painter.fillRect(0, 0, mm_to_points(420), mm_to_points(297), Qt::white); // x, y - координаты верхнего левого угла, width, height - ширина и высота квадрата
    drawSquaresFromMatrix(painter, ramka_A3_VP, 0, 0);
    painter.translate(mm_to_points(420), mm_to_points(252));
    painter.rotate(-180);
    drawSquaresFromMatrix(painter, Title_block_List1vp_TextDocuments, 0, 0);
    painter.translate(mm_to_points(0), mm_to_points(-45));
    drawSquaresFromMatrix(painter, Title_block_List1_TextDocuments, 0, 0);
    painter.rotate(180);
    painter.translate(mm_to_points(-420), mm_to_points(0));
    drawSquaresFromMatrix(painter, Left_block_List1, 0, 1);
    painter.translate(mm_to_points(0), mm_to_points(-297));
    drawSquaresFromMatrix(painter, Top_block_VP, 0, 1);
    painter.translate(mm_to_points(0), mm_to_points(297));
    drawSquaresFromMatrix(painter, Lines_A3_List1, 0, 1);
    painter.translate(mm_to_points(0), mm_to_points(-297));//Вернулись на первоначальное положение

    //начинаем писать текст
    drawTextFromMatrix(painter, Title_block_List1_TextDocuments_textData_A3, 0, 0, 420, 297);
    drawTextFromMatrix(painter, Left_block_List1_textElements, 0, 1, 0, 297);
    drawTextFromMatrix(painter, Top_block_VP_textElements, 0, 1, 0, 0);
}

void painterPrinter::drawOtherPageVP(QPainter &painter)
{
    painter.fillRect(0, 0, mm_to_points(420), mm_to_points(297), Qt::white); // x, y - координаты верхнего левого угла, width, height - ширина и высота квадрата
    drawSquaresFromMatrix(painter, ramka_A3_VP, 0, 0);
    painter.translate(mm_to_points(420), mm_to_points(297));
    drawSquaresFromMatrix(painter, Title_block_List2, 0, 1);
    painter.translate(mm_to_points(-420), mm_to_points(0));
    drawSquaresFromMatrix(painter, Left_block_List2, 0, 1);
    painter.translate(mm_to_points(0), mm_to_points(-297));
    drawSquaresFromMatrix(painter, Top_block_VP, 0, 1);
    painter.translate(mm_to_points(0), mm_to_points(297));
    drawSquaresFromMatrix(painter, Lines_A3_List2, 0, 1);
    painter.translate(mm_to_points(0), mm_to_points(-297));//Вернулись на первоначальное положение

    //начинаем писать текст
    drawTextFromMatrix(painter, Title_block_List2_textElements_A3, 0, 1, 420, 297);
    drawTextFromMatrix(painter, Left_block_List2_textElements, 0, 1, 0, 297);
    drawTextFromMatrix(painter, Top_block_VP_textElements, 0, 1, 0, 0);
}

void painterPrinter::drawLRIPage(QPainter &painter)
{
    painter.fillRect(0, 0, mm_to_points(210), mm_to_points(297), Qt::white); // x, y - координаты верхнего левого угла, width, height - ширина и высота квадрата
    drawSquaresFromMatrix(painter, A4_ListRegistration, 0, 0);
    painter.translate(mm_to_points(210), mm_to_points(297));
    drawSquaresFromMatrix(painter, Title_block_List2, 0, 1);
    painter.translate(mm_to_points(-210), mm_to_points(0));
    drawSquaresFromMatrix(painter, Left_block_List2, 0, 1);
    painter.translate(mm_to_points(0), mm_to_points(-297));//Вернулись на первоначальное положение

    //начинаем писать текст
    drawTextFromMatrix(painter, Title_block_List2_textElements_A4, 0, 1, 210, 297);
    drawTextFromMatrix(painter, Left_block_List2_textElements, 0, 1, 0, 297);
    drawTextFromMatrix(painter, A4_ListRegistration_textElements, 0, 0, 0, 0);
}

void painterPrinter::drawPage(QPainter &painter, QString docType, int pageNumber)
{
    if(docType == "PE"){
        if(pageNumber > 1){
            drawOtherPagePEPrint(painter);
        }
        else{
            drawFirstPagePEPrint(painter);
        }
    }
    if(docType == "VP"){
        if(pageNumber > 1){
            drawOtherPageVPPrint(painter);
        }
        else{
            drawFirstPageVPPrint(painter);
        }
    }
    if(docType == "SP"){
        if(pageNumber > 1){
            drawOtherPageSPPrint(painter);
        }
        else{
            drawFirstPageSPPrint(painter);
        }
    }
    if(docType == "LRI"){
        drawLRIPagePrint(painter);
    }
}

void painterPrinter::drawFirstPagePEPrint(QPainter &painter)
{
    painter.fillRect(0, 0, mm_to_points_print(210), mm_to_points_print(297), Qt::white); // x, y - координаты верхнего левого угла, width, height - ширина и высота квадрата
    drawSquaresFromMatrixPrint(painter, ramka_A4_PE, 0, 0);
    painter.translate(mm_to_points_print(210), mm_to_points_print(252));
    painter.rotate(-180);
    drawSquaresFromMatrixPrint(painter, Title_block_List1vp_TextDocuments, 0, 0);
    painter.translate(mm_to_points_print(0), mm_to_points_print(-45));
    drawSquaresFromMatrixPrint(painter, Title_block_List1_TextDocuments, 0, 0);

    painter.rotate(180);
    painter.translate(mm_to_points_print(-210), mm_to_points_print(0));
    drawSquaresFromMatrixPrint(painter, Left_block_List1, 0, 1);
    painter.translate(mm_to_points_print(0), mm_to_points_print(-297));
    drawSquaresFromMatrixPrint(painter, Top_block_PE, 0, 0);
    painter.translate(mm_to_points_print(0), mm_to_points_print(297));
    drawSquaresFromMatrixPrint(painter, Lines_A4_PE_25lines_List1, 0, 1);
    painter.translate(mm_to_points_print(0), mm_to_points_print(-297));//Вернулись на первоначальное положение

    //начинаем писать текст
    drawTextFromMatrixPrint(painter, Title_block_List1_TextDocuments_textData_A4_PE, 0, 0, 210, 297);
    drawTextFromMatrixPrint(painter, Left_block_List1_textElements, 0, 1, 0, 297);
    drawTextFromMatrixPrint(painter, Top_block_PE_textElements, 0, 0, 1, 0);
}

void painterPrinter::drawOtherPagePEPrint(QPainter &painter)
{
    painter.fillRect(0, 0, mm_to_points_print(210), mm_to_points_print(297), Qt::white); // x, y - координаты верхнего левого угла, width, height - ширина и высота квадрата
    drawSquaresFromMatrixPrint(painter, ramka_A4_PE, 0, 0);
    painter.translate(mm_to_points_print(210), mm_to_points_print(297));
    drawSquaresFromMatrixPrint(painter, Title_block_List2, 0, 1);
    painter.translate(mm_to_points_print(-210), mm_to_points_print(0));
    drawSquaresFromMatrixPrint(painter, Left_block_List2, 0, 1);
    painter.translate(mm_to_points_print(0), mm_to_points_print(-297));
    drawSquaresFromMatrixPrint(painter, Top_block_PE, 0, 0);
    painter.translate(mm_to_points_print(0), mm_to_points_print(297));
    drawSquaresFromMatrixPrint(painter, Lines_A4_PE_List2, 0, 1);
    painter.translate(mm_to_points_print(0), mm_to_points_print(-297));//Вернулись на первоначальное положение

    //начинаем писать текст
    drawTextFromMatrixPrint(painter, Title_block_List2_textElements_A4, 0, 1, 210, 297);
    drawTextFromMatrixPrint(painter, Left_block_List2_textElements, 0, 1, 0, 297);
    drawTextFromMatrixPrint(painter, Top_block_PE_textElements, 0, 0, 0, 0);
}

void painterPrinter::drawFirstPageSPPrint(QPainter &painter)
{
    painter.fillRect(0, 0, mm_to_points_print(210), mm_to_points_print(297), Qt::white); // x, y - координаты верхнего левого угла, width, height - ширина и высота квадрата
    drawSquaresFromMatrixPrint(painter, ramka_A4_PE, 0, 0);
    painter.translate(mm_to_points_print(210), mm_to_points_print(252));
    painter.rotate(-180);
    drawSquaresFromMatrixPrint(painter, Title_block_List1vp_TextDocuments, 0, 0);
    painter.translate(mm_to_points_print(0), mm_to_points_print(-45));
    drawSquaresFromMatrixPrint(painter, Title_block_List1_TextDocuments, 0, 0);
    painter.rotate(180);
    painter.translate(mm_to_points_print(-210), mm_to_points_print(0));
    drawSquaresFromMatrixPrint(painter, Left_block_List1, 0, 1);
    painter.translate(mm_to_points_print(0), mm_to_points_print(-297));
    drawSquaresFromMatrixPrint(painter, Top_block_SP, 0, 1);
    painter.translate(mm_to_points_print(0), mm_to_points_print(297));
    drawSquaresFromMatrixPrint(painter, Lines_A4_SP_25lines_List1, 0, 1);
    painter.translate(mm_to_points_print(0), mm_to_points_print(-297));//Вернулись на первоначальное положение

    //начинаем писать текст
    drawTextFromMatrixPrint(painter, Title_block_List1_TextDocuments_textData_A4, 0, 0, 210, 297);
    drawTextFromMatrixPrint(painter, Left_block_List1_textElements, 0, 1, 0, 297);
    drawTextFromMatrixPrint(painter, Top_block_SPtextElements, 0, 1, 0, 0);
}

void painterPrinter::drawOtherPageSPPrint(QPainter &painter)
{
    painter.fillRect(0, 0, mm_to_points_print(210), mm_to_points_print(297), Qt::white); // x, y - координаты верхнего левого угла, width, height - ширина и высота квадрата
    drawSquaresFromMatrixPrint(painter, ramka_A4_PE, 0, 0);
    painter.translate(mm_to_points_print(210), mm_to_points_print(297));
    drawSquaresFromMatrixPrint(painter, Title_block_List2, 0, 1);
    painter.translate(mm_to_points_print(-210), mm_to_points_print(0));
    drawSquaresFromMatrixPrint(painter, Left_block_List2, 0, 1);
    painter.translate(mm_to_points_print(0), mm_to_points_print(-297));
    drawSquaresFromMatrixPrint(painter, Top_block_SP, 0, 1);
    painter.translate(mm_to_points_print(0), mm_to_points_print(297));
    drawSquaresFromMatrixPrint(painter, Lines_A4_SP_25lines_List2, 0, 1);
    painter.translate(mm_to_points_print(0), mm_to_points_print(-297));//Вернулись на первоначальное положение

    //начинаем писать текст
    drawTextFromMatrixPrint(painter, Title_block_List2_textElements_A4, 0, 1, 210, 297);
    drawTextFromMatrixPrint(painter, Left_block_List2_textElements, 0, 1, 0, 297);
    drawTextFromMatrixPrint(painter, Top_block_SPtextElements, 0, 1, 0, 0);
}

void painterPrinter::drawFirstPageVPPrint(QPainter &painter)
{
    painter.fillRect(0, 0, mm_to_points_print(420), mm_to_points_print(297), Qt::white); // x, y - координаты верхнего левого угла, width, height - ширина и высота квадрата
    drawSquaresFromMatrixPrint(painter, ramka_A3_VP, 0, 0);
    painter.translate(mm_to_points_print(420), mm_to_points_print(252));
    painter.rotate(-180);
    drawSquaresFromMatrixPrint(painter, Title_block_List1vp_TextDocuments, 0, 0);
    painter.translate(mm_to_points_print(0), mm_to_points_print(-45));
    drawSquaresFromMatrixPrint(painter, Title_block_List1_TextDocuments, 0, 0);
    painter.rotate(180);
    painter.translate(mm_to_points_print(-420), mm_to_points_print(0));
    drawSquaresFromMatrixPrint(painter, Left_block_List1, 0, 1);
    painter.translate(mm_to_points_print(0), mm_to_points_print(-297));
    drawSquaresFromMatrixPrint(painter, Top_block_VP, 0, 1);
    painter.translate(mm_to_points_print(0), mm_to_points_print(297));
    drawSquaresFromMatrixPrint(painter, Lines_A3_List1, 0, 1);
    painter.translate(mm_to_points_print(0), mm_to_points_print(-297));//Вернулись на первоначальное положение

    //начинаем писать текст
    drawTextFromMatrixPrint(painter, Title_block_List1_TextDocuments_textData_A3, 0, 0, 420, 297);
    drawTextFromMatrixPrint(painter, Left_block_List1_textElements, 0, 1, 0, 297);
    drawTextFromMatrixPrint(painter, Top_block_VP_textElements, 0, 1, 0, 0);
}

void painterPrinter::drawOtherPageVPPrint(QPainter &painter)
{
    painter.fillRect(0, 0, mm_to_points_print(420), mm_to_points_print(297), Qt::white); // x, y - координаты верхнего левого угла, width, height - ширина и высота квадрата
    drawSquaresFromMatrixPrint(painter, ramka_A3_VP, 0, 0);
    painter.translate(mm_to_points_print(420), mm_to_points_print(297));
    drawSquaresFromMatrixPrint(painter, Title_block_List2, 0, 1);
    painter.translate(mm_to_points_print(-420), mm_to_points_print(0));
    drawSquaresFromMatrixPrint(painter, Left_block_List2, 0, 1);
    painter.translate(mm_to_points_print(0), mm_to_points_print(-297));
    drawSquaresFromMatrixPrint(painter, Top_block_VP, 0, 1);
    painter.translate(mm_to_points_print(0), mm_to_points_print(297));
    drawSquaresFromMatrixPrint(painter, Lines_A3_List2, 0, 1);
    painter.translate(mm_to_points_print(0), mm_to_points_print(-297));//Вернулись на первоначальное положение

    //начинаем писать текст
    drawTextFromMatrixPrint(painter, Title_block_List2_textElements_A3, 0, 1, 420, 297);
    drawTextFromMatrixPrint(painter, Left_block_List2_textElements, 0, 1, 0, 297);
    drawTextFromMatrixPrint(painter, Top_block_VP_textElements, 0, 1, 0, 0);
}

void painterPrinter::drawLRIPagePrint(QPainter &painter)
{
    painter.fillRect(0, 0, mm_to_points_print(210), mm_to_points_print(297), Qt::white); // x, y - координаты верхнего левого угла, width, height - ширина и высота квадрата
    drawSquaresFromMatrixPrint(painter, A4_ListRegistration, 0, 0);
    painter.translate(mm_to_points_print(210), mm_to_points_print(297));
    drawSquaresFromMatrixPrint(painter, Title_block_List2, 0, 1);
    painter.translate(mm_to_points_print(-210), mm_to_points_print(0));
    drawSquaresFromMatrixPrint(painter, Left_block_List2, 0, 1);
    painter.translate(mm_to_points_print(0), mm_to_points_print(-297));//Вернулись на первоначальное положение

    //начинаем писать текст
    drawTextFromMatrixPrint(painter, Title_block_List2_textElements_A4, 0, 1, 210, 297);
    drawTextFromMatrixPrint(painter, Left_block_List2_textElements, 0, 1, 0, 297);
    drawTextFromMatrixPrint(painter, A4_ListRegistration_textElements, 0, 0, 0, 0);
}

void painterPrinter::drawContentPE(QPainter &painter, PageContainer pageContainer, int pageNumber)
{
    //
}

void painterPrinter::drawTextFromMatrix(QPainter &painter, const QVector<TextElement> &textMatrix, bool xMirror, bool yMirror, double marginX, double marginY){
    for (const auto &textData : textMatrix){
        drawText(painter, textData, xMirror, yMirror, marginX, marginY);
    }
}

void painterPrinter::drawTextFromMatrixPrint(QPainter &painter, const QVector<TextElement> &textMatrix, bool xMirror, bool yMirror, double marginX, double marginY){
    for (const auto &textData : textMatrix){
        drawTextPrint(painter, textData, xMirror, yMirror, marginX, marginY);
    }
}

double painterPrinter::mm_to_points(double mm)
{
    return mm * 2.83465;
//    return mm*23.6;
}

double painterPrinter::mm_to_points_print(double mm)
{
    return mm * 3.7;
//    return mm * 23.6;
}

void painterPrinter::paintEvent(QPaintEvent *event)
{
    // Создаем изображение с размерами виджета canvas
    QImage image(ui->canvas->size(), QImage::Format_ARGB32);
    // Создаем объект QPainter для рисования на изображении
    QPainter painter(&image);

    // Очищаем изображение, чтобы фон был прозрачным
    image.fill(Qt::transparent);

    // Сохраняем текущее состояние системы координат
    painter.save();

    // Находим центр полотна
    QPointF center(width() / 2.0, height() / 2.0);

    // Смещаем начало координат к центру полотна
//        painter.translate(center);
    // Применяем текущее смещение
    painter.translate(-x_offset, -y_offset);

    drawOtherPagePE(painter);

//    for(int i = 0; i < 24; ++i){

        //  Координаты текста внутри документов
//        QRect squre(mm_to_points(26),mm_to_points(22+i*8), mm_to_points(9), mm_to_points(4));// Координаты и размеры квадрата 1 столбец ПЭ
//        QRect squre(mm_to_points(41),mm_to_points(22+i*8), mm_to_points(109), mm_to_points(4));// Координаты и размеры квадрата 2 столбец ПЭ
//        QRect squre(mm_to_points(150.5),mm_to_points(22+i*8), mm_to_points(9), mm_to_points(4));// Координаты и размеры квадрата 3 столбец ПЭ
//        QRect squre(mm_to_points(179),mm_to_points(22+i*8), mm_to_points(9), mm_to_points(4));// Координаты и размеры квадрата 4 столбец ПЭ

//        QRect squre(mm_to_points(34),mm_to_points(22+i*8), mm_to_points(4), mm_to_points(4));// Координаты и размеры квадрата 3 столбец СП
//        QRect squre(mm_to_points(42),mm_to_points(22+i*8), mm_to_points(66), mm_to_points(4));// Координаты и размеры квадрата 4 столбец СП
//        QRect squre(mm_to_points(112),mm_to_points(22+i*8), mm_to_points(61), mm_to_points(4));// Координаты и размеры квадрата 5 столбец СП
//        QRect squre(mm_to_points(175),mm_to_points(22+i*8), mm_to_points(5), mm_to_points(4));// Координаты и размеры квадрата 6 столбец СП
//        QRect squre(mm_to_points(184),mm_to_points(22+i*8), mm_to_points(21), mm_to_points(4));// Координаты и размеры квадрата 6 столбец СП

//        QRect squre(mm_to_points(21.6),mm_to_points(34+i*8), mm_to_points(4), mm_to_points(4));// Координаты и размеры квадрата 1 столбец ВП
//        QRect squre(mm_to_points(28),mm_to_points(34+i*8), mm_to_points(58), mm_to_points(4));// Координаты и размеры квадрата 2 столбец ВП
//        QRect squre(mm_to_points(88),mm_to_points(34+i*8), mm_to_points(44), mm_to_points(4));// Координаты и размеры квадрата 3 столбец ВП
//        QRect squre(mm_to_points(133),mm_to_points(34+i*8), mm_to_points(68), mm_to_points(4));// Координаты и размеры квадрата 4 столбец ВП
//        QRect squre(mm_to_points(203),mm_to_points(34+i*8), mm_to_points(53), mm_to_points(4));// Координаты и размеры квадрата 5 столбец ВП
//        QRect squre(mm_to_points(258),mm_to_points(34+i*8), mm_to_points(68), mm_to_points(4));// Координаты и размеры квадрата 5 столбец ВП
//        QRect squre(mm_to_points(328),mm_to_points(34+i*8), mm_to_points(14), mm_to_points(4));// Координаты и размеры квадрата 6 столбец ВП
//        QRect squre(mm_to_points(344),mm_to_points(34+i*8), mm_to_points(14), mm_to_points(4));// Координаты и размеры квадрата 7 столбец ВП
//        QRect squre(mm_to_points(360),mm_to_points(34+i*8), mm_to_points(14), mm_to_points(4));// Координаты и размеры квадрата 8 столбец ВП
//        QRect squre(mm_to_points(376),mm_to_points(34+i*8), mm_to_points(14), mm_to_points(4));// Координаты и размеры квадрата 9 столбец ВП
//        QRect squre(mm_to_points(392),mm_to_points(34+i*8), mm_to_points(22), mm_to_points(4));// Координаты и размеры квадрата 10 столбец ВП

//        // Рисование квадрата
//        painter.drawRect(squre);
//        QString text = "111";
//        // Рисование текста по центру квадрата
//        drawTextInCenter(painter, squre, text);
//    }

    painter.translate(mm_to_points_print(161), mm_to_points_print(227.5));

    painter.setBrush(Qt::white); // Белый цвет для заполнения
    painter.setPen(Qt::NoPen); // Нет контура

    //  Координаты текста штампа
//    painter.translate(mm_to_points_print(161), mm_to_points_print(227.5)); для А4
//    QRect squre(mm_to_points(-172.5),mm_to_points(-29), mm_to_points(21), mm_to_points(4));// Разраб.
//    QRect squre(mm_to_points(-172.5),mm_to_points(-24), mm_to_points(21), mm_to_points(4));// Пров.
//    QRect squre(mm_to_points(-189.5),mm_to_points(-19.5), mm_to_points(15), mm_to_points(4));// Чертил
//    QRect squre(mm_to_points(-172.5),mm_to_points(-19.5), mm_to_points(21), mm_to_points(4));// Чертила
//    QRect squre(mm_to_points(-172.5),mm_to_points(-14.5), mm_to_points(21), mm_to_points(4));// Н. контр.
//    QRect squre(mm_to_points(-172.5),mm_to_points(-9.7), mm_to_points(21), mm_to_points(4));// Утв.
//    QRect squre(mm_to_points(-124.5),mm_to_points(-29), mm_to_points(68), mm_to_points(7));// Название проекта 1
//    QRect squre(mm_to_points(-124.5),mm_to_points(-22), mm_to_points(68), mm_to_points(7));// Название проекта 2
//    QRect squre(mm_to_points(-124.5),mm_to_points(-25.5), mm_to_points(68), mm_to_points(7));// Название проекта
//    QRect squre(mm_to_points(-124.5),mm_to_points(-44), mm_to_points(118), mm_to_points(13));// RSAL
//    QRect squre(mm_to_points(-54),mm_to_points(-24.3), mm_to_points(4), mm_to_points(4));// Лит 1
//    QRect squre(mm_to_points(-49.5),mm_to_points(-24.3), mm_to_points(4), mm_to_points(4));// Лит 2
//    QRect squre(mm_to_points(-44.5),mm_to_points(-24.3), mm_to_points(4), mm_to_points(4));// Лит 3
//    QRect squre(mm_to_points(-39),mm_to_points(-24.3), mm_to_points(13), mm_to_points(4));// Лист
//    QRect squre(mm_to_points(-24),mm_to_points(-24.3), mm_to_points(17), mm_to_points(4));// Листов
//    QRect squre(mm_to_points(-54),mm_to_points(-19), mm_to_points(47), mm_to_points(12));// Организация

    // Запись значения Первогол применения
//    painter.translate(mm_to_points_print(-161), mm_to_points_print(-227.5));

//    painter.rotate(-90);
//    QRect squre(mm_to_points(-64),mm_to_points(14.5), mm_to_points(58), mm_to_points(4));// Перв. применен.

    // Координаты штампа второго листа
//    QRect squre(mm_to_points(-124),mm_to_points(-19), mm_to_points(105), mm_to_points(12));// RSAL
//    QRect squre(mm_to_points(-13.5),mm_to_points(-11), mm_to_points(7), mm_to_points(4));// Лист

//    // Рисование квадрата
//    painter.drawRect(squre);
    QString text = "2";
////    qDebug() << mm_to_points(21);
//    // Рисование текста по центру квадрата
//    drawTextInCenter(painter, squre, text, 3.5);

//    painter.rotate(90);

    painter.translate(mm_to_points_print(-161), mm_to_points_print(5));
    //перемещаемся к началу координат следующего листа
    drawOtherPageVP(painter);
    for(int i = 0; i < 29; ++i){
        painter.setBrush(Qt::white); // Белый цвет для заполнения
        painter.setPen(Qt::NoPen); // Нет контура
    //    QRect squareRect(40, 85, 150, 93); // Примерные координаты и размеры квадрата
        QRect squre(mm_to_points(41),mm_to_points(34+i*8), mm_to_points(109), mm_to_points(4));// Разраб.


        // Рисование квадрата
        painter.drawRect(squre);
        QString text = "Конденсатор";
        // Рисование текста по центру квадрата
        drawTextInCenter(painter, squre, text);
    }
    // Установка цвета квадрата и его размеров
    painter.setBrush(Qt::white); // Белый цвет для заполнения
    painter.setPen(Qt::NoPen); // Нет контура
//    QRect squareRect(40, 85, 150, 93); // Примерные координаты и размеры квадрата
//    QRect squre(mm_to_points(41),mm_to_points(30), mm_to_points(109), mm_to_points(4));

    // Рисование квадрата
//    painter.drawRect(squre);
//    QString text = "Конденсатор";
    // Рисование текста по центру квадрата
//    drawTextInCenter(painter, squre, text);
    // Восстанавливаем предыдущее состояние системы координат
    painter.restore();
//    painter.end();
    update();

    // Создаем объект QPainter для рисования на виджете
    QPainter widgetPainter(this);

    // Устанавливаем виджет canvas в качестве маски для QPainter
    widgetPainter.setClipRect(ui->canvas->geometry());

    // Рисуем изображение на виджете с помощью QPainter
    widgetPainter.drawImage(ui->canvas->geometry(), image);
//        repaint();
}
