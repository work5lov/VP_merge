#include "docpainter.h"

DocPainter::DocPainter(QWidget *parent) : QWidget(parent)
{
    _xOffset = 0;
    _yOffset = 0;
}

void DocPainter::setData(PageContainer pages)
{
    _dataPagesContainer = pages;
}

void DocPainter::setStamp(QMap<QString, QString> stamp)
{
    _stamp = stamp;
}

void DocPainter::updateX(int x)
{
    _xOffset = 10*x;

    update();
    repaint();
}

void DocPainter::updateY(int y)
{
    _yOffset = 50*y;

    update();
    repaint();
}

void DocPainter::setDocType(QString doc)
{
    _docType = doc;
}

void DocPainter::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);

    QSize widgetSize = this->size();
    int width = widgetSize.width();
    int height = widgetSize.height();

    // Сохраняем текущее состояние системы координат
//    painter.save();

    QPointF center(width / 2.0, height / 2.0);

    // Применяем текущее смещение
    painter.translate(-_xOffset, -_yOffset);

    if(_dataPagesContainer.pages.isEmpty()){
//        qDebug() << width << height;
        //отправить сигнал для блокирования scroll bar'ов

//        painter.setBrush(Qt::white); // Белый цвет для заполнения
//        painter.setPen(Qt::NoPen); // Нет контура

//        QRect squre(center.x()-50,center.y(), 100, 10);// Координаты и размеры квадрата 10 столбец ВП

//        painter.fillRect(0, 0, (width), (height), Qt::white); // x, y - координаты верхнего левого угла, width, height - ширина и высота квадрата
//        painter.drawRect(squre);
//        QString text = "Данных для отрисовки нет!";
//        drawTextInCenter(painter, squre, text, 5);
    }
    else{
//        painter.setBrush(Qt::white); // Белый цвет для заполнения
//        painter.setPen(Qt::NoPen); // Нет контура
        drawDocument(painter, _dataPagesContainer);
    }

    update();
//    repaint();
}



void DocPainter::drawSquare(QPainter &painter, const QVector<double> &squareData, bool xMirror, bool yMirror)
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

void DocPainter::drawSquarePrint(QPainter &painter, const QVector<double> &squareData, bool xMirror, bool yMirror)
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

void DocPainter::drawText(QPainter &painter, const TextElement1 &textElements, bool xMirror, bool yMirror, double marginX, double marginY){
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

void DocPainter::drawTextInCenter(QPainter &painter, const QRect &rect, const TextElement1 &textElements)
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

void DocPainter::drawTextInCenter(QPainter &painter, const QRect &rect, const QString &text){
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

void DocPainter::drawTextInCenter(QPainter &painter, const QRect &rect, const QString &text, double textSize){
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

void DocPainter::drawTextTitle(QPainter &painter, const QRect &rect, const QString &text){
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

void DocPainter::drawTextPrint(QPainter &painter, const TextElement1 &textElements, bool xMirror, bool yMirror, double marginX, double marginY){
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

void DocPainter::drawSquaresFromMatrix(QPainter &painter, const QVector<QVector<double> > &squaresMatrix, bool xMirror, bool yMirror)
{
    for (const auto &squareData : squaresMatrix) {
        drawSquare(painter, squareData, xMirror, yMirror);
    }
}

void DocPainter::drawSquaresFromMatrixPrint(QPainter &painter, const QVector<QVector<double> > &squaresMatrix, bool xMirror, bool yMirror)
{
    for (const auto &squareData : squaresMatrix) {
        drawSquarePrint(painter, squareData, xMirror, yMirror);
    }
}

void DocPainter::drawFirstPagePE(QPainter &painter)
{
    painter.fillRect(0, 0, mm_to_points(210), mm_to_points(297), Qt::white); // x, y - координаты верхнего левого угла, width, height - ширина и высота квадрата
    drawSquaresFromMatrix(painter, ramka_A4_PE1, 0, 0);
    painter.translate(mm_to_points(210), mm_to_points(252));
    painter.rotate(-180);
    drawSquaresFromMatrix(painter, Title_block_List1vp_TextDocuments1, 0, 0);
    painter.translate(mm_to_points(0), mm_to_points(-45));
    drawSquaresFromMatrix(painter, Title_block_List1_TextDocuments1, 0, 0);

    painter.rotate(180);
    painter.translate(mm_to_points(-210), mm_to_points(0));
    drawSquaresFromMatrix(painter, Left_block_List11, 0, 1);
    painter.translate(mm_to_points(0), mm_to_points(-297));
    drawSquaresFromMatrix(painter, Top_block_PE1, 0, 0);
    painter.translate(mm_to_points(0), mm_to_points(297));
    drawSquaresFromMatrix(painter, Lines_A4_PE_25lines_List11, 0, 1);
    painter.translate(mm_to_points(0), mm_to_points(-297));//Вернулись на первоначальное положение

    //начинаем писать текст
    drawTextFromMatrix(painter, Title_block_List1_TextDocuments1_textData_A41_PE1, 0, 0, 210, 297);
    drawTextFromMatrix(painter, Left_block_List11_textElements, 0, 1, 0, 297);
    drawTextFromMatrix(painter, Top_block_PE1_textElements, 0, 0, 1, 0);
}

void DocPainter::drawOtherPagePE(QPainter &painter)
{
    painter.fillRect(0, 0, mm_to_points(210), mm_to_points(297), Qt::white); // x, y - координаты верхнего левого угла, width, height - ширина и высота квадрата
    drawSquaresFromMatrix(painter, ramka_A4_PE1, 0, 0);
    painter.translate(mm_to_points(210), mm_to_points(297));
    drawSquaresFromMatrix(painter, Title_block_List21, 0, 1);
    painter.translate(mm_to_points(-210), mm_to_points(0));
    drawSquaresFromMatrix(painter, Left_block_List21, 0, 1);
    painter.translate(mm_to_points(0), mm_to_points(-297));
    drawSquaresFromMatrix(painter, Top_block_PE1, 0, 0);
    painter.translate(mm_to_points(0), mm_to_points(297));
    drawSquaresFromMatrix(painter, Lines_A4_PE_List21, 0, 1);
    painter.translate(mm_to_points(0), mm_to_points(-297));//Вернулись на первоначальное положение

    //начинаем писать текст
    drawTextFromMatrix(painter, Title_block_List21_textElements_A4, 0, 1, 210, 297);
    drawTextFromMatrix(painter, Left_block_List21_textElements, 0, 1, 0, 297);
    drawTextFromMatrix(painter, Top_block_PE1_textElements, 0, 0, 0, 0);
}

void DocPainter::drawFirstPageSP(QPainter &painter)
{
    painter.fillRect(0, 0, mm_to_points(210), mm_to_points(297), Qt::white); // x, y - координаты верхнего левого угла, width, height - ширина и высота квадратаdebug\mainwindow.o:C:\LezhnevV\workprjcts\build-VP_merge-Desktop_Qt_5_12_12_MinGW_64_bit-Debug\..\VP_merge\perechenDrwingConstants.h:21: ошибка: multiple definition of `ramka_A4_PE'
    drawSquaresFromMatrix(painter, ramka_A4_PE1, 0, 0);
    painter.translate(mm_to_points(210), mm_to_points(252));
    painter.rotate(-180);
    drawSquaresFromMatrix(painter, Title_block_List1vp_TextDocuments1, 0, 0);
    painter.translate(mm_to_points(0), mm_to_points(-45));
    drawSquaresFromMatrix(painter, Title_block_List1_TextDocuments1, 0, 0);
    painter.rotate(180);
    painter.translate(mm_to_points(-210), mm_to_points(0));
    drawSquaresFromMatrix(painter, Left_block_List11, 0, 1);
    painter.translate(mm_to_points(0), mm_to_points(-297));
    drawSquaresFromMatrix(painter, Top_block_SP1, 0, 1);
    painter.translate(mm_to_points(0), mm_to_points(297));
    drawSquaresFromMatrix(painter, Lines_A4_SP_25lines_List11, 0, 1);
    painter.translate(mm_to_points(0), mm_to_points(-297));//Вернулись на первоначальное положение

    //начинаем писать текст
    drawTextFromMatrix(painter, Title_block_List1_TextDocuments1_textData_A41, 0, 0, 210, 297);
    drawTextFromMatrix(painter, Left_block_List11_textElements, 0, 1, 0, 297);
    drawTextFromMatrix(painter, Top_block_SP1textElements, 0, 1, 0, 0);
}

void DocPainter::drawOtherPageSP(QPainter &painter)
{
    painter.fillRect(0, 0, mm_to_points(210), mm_to_points(297), Qt::white); // x, y - координаты верхнего левого угла, width, height - ширина и высота квадрата
    drawSquaresFromMatrix(painter, ramka_A4_PE1, 0, 0);
    painter.translate(mm_to_points(210), mm_to_points(297));
    drawSquaresFromMatrix(painter, Title_block_List21, 0, 1);
    painter.translate(mm_to_points(-210), mm_to_points(0));
    drawSquaresFromMatrix(painter, Left_block_List21, 0, 1);
    painter.translate(mm_to_points(0), mm_to_points(-297));
    drawSquaresFromMatrix(painter, Top_block_SP1, 0, 1);
    painter.translate(mm_to_points(0), mm_to_points(297));
    drawSquaresFromMatrix(painter, Lines_A4_SP_25lines_List21, 0, 1);
    painter.translate(mm_to_points(0), mm_to_points(-297));//Вернулись на первоначальное положение

    //начинаем писать текст
    drawTextFromMatrix(painter, Title_block_List21_textElements_A4, 0, 1, 210, 297);
    drawTextFromMatrix(painter, Left_block_List21_textElements, 0, 1, 0, 297);
    drawTextFromMatrix(painter, Top_block_SP1textElements, 0, 1, 0, 0);
}

void DocPainter::drawFirstPageVP(QPainter &painter)
{
    painter.fillRect(0, 0, mm_to_points(420), mm_to_points(297), Qt::white); // x, y - координаты верхнего левого угла, width, height - ширина и высота квадрата
    drawSquaresFromMatrix(painter, ramka_A3_VP1, 0, 0);
    painter.translate(mm_to_points(420), mm_to_points(252));
    painter.rotate(-180);
    drawSquaresFromMatrix(painter, Title_block_List1vp_TextDocuments1, 0, 0);
    painter.translate(mm_to_points(0), mm_to_points(-45));
    drawSquaresFromMatrix(painter, Title_block_List1_TextDocuments1, 0, 0);
    painter.rotate(180);
    painter.translate(mm_to_points(-420), mm_to_points(0));
    drawSquaresFromMatrix(painter, Left_block_List11, 0, 1);
    painter.translate(mm_to_points(0), mm_to_points(-297));
    drawSquaresFromMatrix(painter, Top_block_VP1, 0, 1);
    painter.translate(mm_to_points(0), mm_to_points(297));
    drawSquaresFromMatrix(painter, Lines_A3_List11, 0, 1);
    painter.translate(mm_to_points(0), mm_to_points(-297));//Вернулись на первоначальное положение

    //начинаем писать текст
    drawTextFromMatrix(painter, Title_block_List1_TextDocuments1_textData_A3, 0, 0, 420, 297);
    drawTextFromMatrix(painter, Left_block_List11_textElements, 0, 1, 0, 297);
    drawTextFromMatrix(painter, Top_block_VP1_textElements, 0, 1, 0, 0);
}

void DocPainter::drawOtherPageVP(QPainter &painter)
{
    painter.fillRect(0, 0, mm_to_points(420), mm_to_points(297), Qt::white); // x, y - координаты верхнего левого угла, width, height - ширина и высота квадрата
    drawSquaresFromMatrix(painter, ramka_A3_VP1, 0, 0);
    painter.translate(mm_to_points(420), mm_to_points(297));
    drawSquaresFromMatrix(painter, Title_block_List21, 0, 1);
    painter.translate(mm_to_points(-420), mm_to_points(0));
    drawSquaresFromMatrix(painter, Left_block_List21, 0, 1);
    painter.translate(mm_to_points(0), mm_to_points(-297));
    drawSquaresFromMatrix(painter, Top_block_VP1, 0, 1);
    painter.translate(mm_to_points(0), mm_to_points(297));
    drawSquaresFromMatrix(painter, Lines_A3_List21, 0, 1);
    painter.translate(mm_to_points(0), mm_to_points(-297));//Вернулись на первоначальное положение

    //начинаем писать текст
    drawTextFromMatrix(painter, Title_block_List21_textElements_A3, 0, 1, 420, 297);
    drawTextFromMatrix(painter, Left_block_List21_textElements, 0, 1, 0, 297);
    drawTextFromMatrix(painter, Top_block_VP1_textElements, 0, 1, 0, 0);
}

void DocPainter::drawLRIPage(QPainter &painter)
{
    painter.fillRect(0, 0, mm_to_points(210), mm_to_points(297), Qt::white); // x, y - координаты верхнего левого угла, width, height - ширина и высота квадрата
    drawSquaresFromMatrix(painter, A4_ListRegistration1, 0, 0);
    painter.translate(mm_to_points(210), mm_to_points(297));
    drawSquaresFromMatrix(painter, Title_block_List21, 0, 1);
    painter.translate(mm_to_points(-210), mm_to_points(0));
    drawSquaresFromMatrix(painter, Left_block_List21, 0, 1);
    painter.translate(mm_to_points(0), mm_to_points(-297));//Вернулись на первоначальное положение

    //начинаем писать текст
    drawTextFromMatrix(painter, Title_block_List21_textElements_A4, 0, 1, 210, 297);
    drawTextFromMatrix(painter, Left_block_List21_textElements, 0, 1, 0, 297);
    drawTextFromMatrix(painter, A4_ListRegistration1_textElements, 0, 0, 0, 0);
}

void DocPainter::drawDocument(QPainter &painter, PageContainer pages)
{
    int pageCount = pages.pages.size();
    painter.setBrush(Qt::white); // Белый цвет для заполнения
    painter.setPen(Qt::NoPen); // Нет контура
    int currentPage = 0; // Номер текущей страницы
    if(_docType == "PE"){
        foreach (const Page& page, pages.pages){
            ++currentPage;
            int row = 3;
            if(currentPage > 1){

                //  Рисуем лист
                drawOtherPagePE(painter);

                // Заполняем строки
                foreach (const peline& pipeline, page.pipelines){                    
                    QRect oboz(mm_to_points(26),mm_to_points(6+row*8), mm_to_points(9), mm_to_points(4));// Координаты и размеры квадрата 1 столбец ПЭ
                    QRect name(mm_to_points(41),mm_to_points(6+row*8), mm_to_points(109), mm_to_points(4));// Координаты и размеры квадрата 2 столбец ПЭ
                    QRect num(mm_to_points(150.5),mm_to_points(6+row*8), mm_to_points(9), mm_to_points(4));// Координаты и размеры квадрата 3 столбец ПЭ
                    QRect comment(mm_to_points(179),mm_to_points(6+row*8), mm_to_points(9), mm_to_points(4));// Координаты и размеры квадрата 4 столбец ПЭ
                    // Рисование квадрата
//                    painter.drawRect(oboz);
//                    painter.drawRect(name);
//                    painter.drawRect(num);
//                    painter.drawRect(comment);
                    // Рисование текста по центру квадрата
                    drawTextInCenter(painter, oboz, pipeline.oboz, 3.5);
                    drawTextInCenter(painter, name, pipeline.name, 3.5);
                    drawTextInCenter(painter, num, pipeline.num, 3.5);
                    drawTextInCenter(painter, comment, pipeline.comment, 3.5);
                    ++row;
                }

                //  Заполняем штамп

//                painter.rotate(-90);
//                QRect perv(mm_to_points(-64),mm_to_points(14.5), mm_to_points(58), mm_to_points(4));// Перв. применен.
//                painter.drawRect(perv);
//                drawTextInCenter(painter, perv, "RSAL XXXXXX.XXX", 3.5);
//                painter.rotate(90);

                painter.translate(mm_to_points_print(161), mm_to_points_print(227.5));

                //  Заполняем остальной штамп
                QRect rsal(mm_to_points(-124),mm_to_points(-19), mm_to_points(105), mm_to_points(12));// RSAL
                QRect pageNumper(mm_to_points(-13.5),mm_to_points(-11), mm_to_points(7), mm_to_points(4));// Лист
                // Рисование квадрата
//                painter.drawRect(rsal);
//                painter.drawRect(pageNumper);
                // Рисование текста по центру квадрата
                drawTextInCenter(painter, rsal, _stamp.value("Децимальный номер") + "ПЭ3");
                drawTextInCenter(painter, pageNumper, QString::number(currentPage));

                //  Смещаемся для отрисовки следующего листа
                painter.translate(mm_to_points_print(-161), mm_to_points_print(5));

            }
            else{
                //  Рисуем лист
                drawFirstPagePE(painter);

                // Заполняем строки
                foreach (const peline& pipeline, page.pipelines){
                    QRect oboz(mm_to_points(26),mm_to_points(6+row*8), mm_to_points(9), mm_to_points(4));// Координаты и размеры квадрата 1 столбец ПЭ
                    QRect name(mm_to_points(41),mm_to_points(6+row*8), mm_to_points(109), mm_to_points(4));// Координаты и размеры квадрата 2 столбец ПЭ
                    QRect num(mm_to_points(150.5),mm_to_points(6+row*8), mm_to_points(9), mm_to_points(4));// Координаты и размеры квадрата 3 столбец ПЭ
                    QRect comment(mm_to_points(179),mm_to_points(6+row*8), mm_to_points(9), mm_to_points(4));// Координаты и размеры квадрата 4 столбец ПЭ
                    // Рисование квадрата
//                    painter.drawRect(oboz);
//                    painter.drawRect(name);
//                    painter.drawRect(num);
//                    painter.drawRect(comment);
                    // Рисование текста по центру квадрата
                    drawTextInCenter(painter, oboz, pipeline.oboz, 3.5);
                    drawTextInCenter(painter, name, pipeline.name, 3.5);
                    drawTextInCenter(painter, num, pipeline.num, 3.5);
                    drawTextInCenter(painter, comment, pipeline.comment, 3.5);
                    ++row;
                }

                //  Заполняем штамп

                painter.rotate(-90);
                QRect perv(mm_to_points(-64),mm_to_points(14.5), mm_to_points(58), mm_to_points(4));// Перв. применен.
//                painter.drawRect(perv);
                drawTextInCenter(painter, perv, _stamp.value("Децимальный номер"), 3.5);
                painter.rotate(90);

                painter.translate(mm_to_points_print(161), mm_to_points_print(227.5));

                //  Заполняем остальной штамп
                QRect razrab(mm_to_points(-172.5),mm_to_points(-29), mm_to_points(21), mm_to_points(4));// Разраб.
                QRect prov(mm_to_points(-172.5),mm_to_points(-24), mm_to_points(21), mm_to_points(4));// Пров.
                QRect chertil(mm_to_points(-189.5),mm_to_points(-19.5), mm_to_points(15), mm_to_points(4));// Чертил
                QRect chertila(mm_to_points(-172.5),mm_to_points(-19.5), mm_to_points(21), mm_to_points(4));// Чертила
                QRect nContr(mm_to_points(-172.5),mm_to_points(-14.5), mm_to_points(21), mm_to_points(4));// Н. контр.
                QRect ytv(mm_to_points(-172.5),mm_to_points(-9.7), mm_to_points(21), mm_to_points(4));// Утв.
                QRect projectName1(mm_to_points(-124.5),mm_to_points(-29), mm_to_points(68), mm_to_points(7));// Название проекта 1
                QRect projectName2(mm_to_points(-124.5),mm_to_points(-22), mm_to_points(68), mm_to_points(7));// Название проекта 2
                QRect projectName(mm_to_points(-124.5),mm_to_points(-25.5), mm_to_points(68), mm_to_points(7));// Название проекта
                QRect rsal(mm_to_points(-124.5),mm_to_points(-44), mm_to_points(118), mm_to_points(13));// RSAL
                QRect lit1(mm_to_points(-54),mm_to_points(-24.3), mm_to_points(4), mm_to_points(4));// Лит 1
                QRect lit2(mm_to_points(-49.5),mm_to_points(-24.3), mm_to_points(4), mm_to_points(4));// Лит 2
                QRect lit3(mm_to_points(-44.5),mm_to_points(-24.3), mm_to_points(4), mm_to_points(4));// Лит 3
                QRect list(mm_to_points(-39),mm_to_points(-24.3), mm_to_points(13), mm_to_points(4));// Лист
                QRect lists(mm_to_points(-24),mm_to_points(-24.3), mm_to_points(17), mm_to_points(4));// Листов
                QRect organization(mm_to_points(-54),mm_to_points(-19), mm_to_points(47), mm_to_points(12));// Организация

                // Рисование квадратов
//                painter.drawRect(razrab);
//                painter.drawRect(prov);
//                painter.drawRect(chertil);
//                painter.drawRect(chertila);
//                painter.drawRect(nContr);
//                painter.drawRect(ytv);
//                painter.drawRect(rsal);
//                painter.drawRect(lit1);
//                painter.drawRect(lit2);
//                painter.drawRect(lit3);
//                painter.drawRect(list);
//                painter.drawRect(lists);
//                painter.drawRect(organization);

                // Рисование текста по центру квадратов
                drawTextInCenter(painter, razrab, _stamp.value("Разработал"));
                drawTextInCenter(painter, prov, _stamp.value("Проверил"));
                drawTextInCenter(painter, chertila, _stamp.value("Начальник отдела"));
                drawTextInCenter(painter, nContr, _stamp.value("Нормоконтроль"));
                drawTextInCenter(painter, ytv, _stamp.value("Утвердил"));
                drawTextInCenter(painter, rsal, (_stamp.value("Децимальный номер") + "ПЭ3"));
                drawTextInCenter(painter, lit1, _stamp.value("Графа 04 - Литера"));
//                drawTextInCenter(painter, lit2, _stamp.value("Разработал"));
//                drawTextInCenter(painter, lit3, _stamp.value("Разработал"));
                drawTextInCenter(painter, list, QString::number(currentPage));
                drawTextInCenter(painter, lists, QString::number(pageCount));

                if (_stamp.value("Начальник отдела ( альтернативное название параметра)").isEmpty()){
                    drawTextInCenter(painter, chertil, "Начальник отдела");//   Начальник отдела
                }
                else{
                    drawTextInCenter(painter, chertil, _stamp.value("Начальник отдела ( альтернативное название параметра)"));//    Начальник отдела
                }

                // Работа с именем проекта
                QString projectNameStr = _stamp.value("Наименование проекта");
                if(projectNameStr.length() >= 30){
                    QStringList projectStrs = splitString(projectNameStr, 30);
//                    painter.drawRect(projectName1);
                    drawTextInCenter(painter, projectName1, projectStrs[0]);
//                    painter.drawRect(projectName2);
                    drawTextInCenter(painter, projectName2, projectStrs[1]);
                }
                else{
//                    painter.drawRect(projectName);
                    drawTextInCenter(painter, projectName, projectNameStr);
                }

                //  Смещаемся для отрисовки следующего листа
                painter.translate(mm_to_points_print(-161), mm_to_points_print(5));
            }
        }
    }
    if(_docType == "SP"){
        //
    }
    if(_docType == "VP"){
        //
    }
}

QStringList DocPainter::splitString(const QString &input, const int len)
{
    QStringList result;

    QStringList words = input.split(' ', QString::SkipEmptyParts);
    QString firstPart;
    QString secondPart;

    for (const QString &word : words) {
        if (firstPart.length() + word.length() + 1 <= len) {
            if (!firstPart.isEmpty()) {
                firstPart += ' ';
            }
            firstPart += word;
        } else {
            if (!secondPart.isEmpty()) {
                secondPart += ' ';
            }
            secondPart += word;
        }
    }

    // Добавляем части в список результата
    if (!firstPart.isEmpty()) {
        result << firstPart;
    }
    if (!secondPart.isEmpty()) {
        result << secondPart;
    }

    return result;
}

void DocPainter::drawTextFromMatrix(QPainter &painter, const QVector<TextElement1> &textMatrix, bool xMirror, bool yMirror, double marginX, double marginY){
    for (const auto &textData : textMatrix){
        drawText(painter, textData, xMirror, yMirror, marginX, marginY);
    }
}

void DocPainter::drawTextFromMatrixPrint(QPainter &painter, const QVector<TextElement1> &textMatrix, bool xMirror, bool yMirror, double marginX, double marginY){
    for (const auto &textData : textMatrix){
        drawTextPrint(painter, textData, xMirror, yMirror, marginX, marginY);
    }
}

double DocPainter::mm_to_points(double mm)
{
    return mm * 2.83465;
//    return mm*23.6;
}

double DocPainter::mm_to_points_print(double mm)
{
    return mm * 3.7;
//    return mm * 23.6;
}
