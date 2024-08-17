#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <Windows.h>

using namespace QXlsx;
QMap <int,double> columnWidth, rowHeight, columnWidthPE, rowHeightPE, columnWidthSP, rowHeightSP;
QMap<QString,QString> razrabs, checks, utvs, norms, nachalnicks, nachalnickAlts;
QMap<QString, QString> cellsWithText, stampPE, stampVP, stampSP, cellsWithTextPE, cellsWithTextSP, fileMap;
QList<QXlsx::CellRange> merged_cellsVP, merged_cellsPE, merged_cellsSP;
QMap<QString, QXlsx::Format> cellFormatsVP, cellFormatsPE, cellFormatsSP;
QStringList fileFinals;
QString projectOboz, inDir,outDir, outFileName, inDirXml;

//pyinstaller --onefile excelPageMargin.py

QMap<QString, QString> bigPostVP = {
    {"Murata Manufacturing Япония", "Murata Manufacturing:Япония"},
    {"АО \"НЗПП с ОКБ\",г.Новосибирск", "АО \"НЗПП с ОКБ\",:г.Новосибирск"},
    {"АО \"НПП \"Восток\",г.Новосибирск", "АО \"НПП \"Восток\",:г.Новосибирск"},
    {"ЗАО \"Группа Кремний Эл\",г.Брянск", "ЗАО \"Группа Кремний Эл\",:г.Брянск"},
    {"ПАО \"Мстатор\",г.Боровичи","ПАО \"Мстатор\",:г.Боровичи"},
    {"ОАО «ОКБ «ЭКСИТОН», г.:Павловский Посад, Московская обл.", "ОАО «ОКБ «ЭКСИТОН», г.:Павловский Посад,:Московская обл."},
    {"АО «СВЕТЛАНА-:ПОЛУПРОВОДНИКИ», г. Санкт-:Петербург", "АО «СВЕТЛАНА-:ПОЛУПРОВОДНИКИ»,:г. Санкт-Петербург"}
};

int columsPE[2][4] = {
    {4, 7, 12, 15},
    {4, 7, 11, 13}
};

int columsVP[2][8] = {
    {5, 6, 7, 8, 14, 19, 25, 28},//Наименование (Е), Обозначение док. (G),
    {5, 6, 7, 8, 12, 16, 19, 20}
};

int columsSP[2][6] = {
    {4, 7, 9, 14, 20, 21},
    {4, 7, 9, 14, 20, 21}
};

struct peline {
    QString num;
    QString oboz;
    QString name;
    QString comment;
    bool underline;
    QString type;
}pipeline;

struct vpline {
    QString num;
    QString oboz;
    QString kod;
    QString name;
    QString comment;
    bool underline;
    QString type;
    bool merge;
    QString vhodit;
    QString post;
}pivpline;

struct specline {
    QString num;
    QString oboz;
    QString name;
    QString comment;
    bool underline;
    QString type;
    bool merge;
    bool needtu;
    QString GroupLine1;
    QString GroupLine2;
    QString format;
    QString pos;
    QString posinelement;
}pispecline;

QList<peline> pipelines;
QList<vpline> pivplines, pivplinesFin;
QList<specline> pispeclines;

struct Page {
    QList<peline> pipelines;
    QList<vpline> pivplines;
    QList<specline> pispeclines;
};

struct PageContainer {
    QList<Page> pages;
}containerPE, containerVP, containerSP, containerVPs;

//functions
QString projectObozn(const QString &filePath);
QMap<QString, QList<vpline>> readAndProcessXMLFiles(const QStringList& xmlFiles);
void addVpline(QList<vpline>& pivplines, const QString& num, const QString& name, const QString& type);
QString checkMatch(const QString& data, const QStringList& list);
void searchFile(const QString &folderPath);

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setMouseTracking(true);
//    ui->pushButton_3->hide();

    QXlsx::Document xlsx1("Vedomost.xlsx");
    xlsx1.selectSheet("2");
    for(int i = 1; i <= 21; ++i){
        double column_width_pixels = xlsx1.columnWidth(i);
        columnWidth.insert(i,column_width_pixels);
    }
    for(int i = 1; i <= 37; ++i){
        double column_width_pixels = xlsx1.rowHeight(i);
        rowHeight.insert(i,column_width_pixels);
    }
    QXlsx::Worksheet *worksheet = xlsx1.currentWorksheet();
    merged_cellsVP = worksheet->mergedCells();
    for (int row = 1; row <= worksheet->dimension().lastRow(); ++row) {
        for (int column = 1; column <= worksheet->dimension().lastColumn(); ++column) {
            QString str = xlsx1.read(row,column).toString();
            if (!(str.isEmpty())) {
                QString cell_name = QString("%1%2").arg(QChar('A' + column - 1)).arg(row);
//                qDebug() << "Cell" << cell_name << "contains text:" << str;
                cellsWithText.insert(cell_name,str);
            }
        }
    }
    for (int row = 1; row <= worksheet->dimension().lastRow(); ++row) {
        for (int column = 1; column <= worksheet->dimension().lastColumn(); ++column) {
            QXlsx::Cell *cell = worksheet->cellAt(row, column);
            if (cell) {
                QXlsx::Format format = cell->format();
                QString cell_name = QString("%1%2").arg(QChar('A' + column - 1)).arg(row);
                cellFormatsVP.insert(cell_name,format);
//                qDebug() << "Cell" << cell_name << "has format:" << format;
            }
        }
    }

    QXlsx::Document xlsxSP("specification.xlsx");
    xlsxSP.selectSheet("2");
    for(int i = 1; i <= 21; ++i){
        double column_width_pixels = xlsxSP.columnWidth(i);
        columnWidthSP.insert(i,column_width_pixels);
    }
    for(int i = 1; i <= 37; ++i){
        double column_width_pixels = xlsxSP.rowHeight(i);
        rowHeightSP.insert(i,column_width_pixels);
    }
    QXlsx::Worksheet *worksheetSP = xlsxSP.currentWorksheet();
    merged_cellsSP = worksheetSP->mergedCells();
    for (int row = 1; row <= worksheetSP->dimension().lastRow(); ++row) {
        for (int column = 1; column <= worksheetSP->dimension().lastColumn(); ++column) {
            QString str = xlsxSP.read(row,column).toString();
            if (!(str.isEmpty())) {
                QString cell_name = QString("%1%2").arg(QChar('A' + column - 1)).arg(row);
    //                qDebug() << "Cell" << cell_name << "contains text:" << str;
                cellsWithTextSP.insert(cell_name,str);
            }
        }
    }
    for (int row = 1; row <= worksheetSP->dimension().lastRow(); ++row) {
        for (int column = 1; column <= worksheetSP->dimension().lastColumn(); ++column) {
            QXlsx::Cell *cell = worksheetSP->cellAt(row, column);
            if (cell) {
                QXlsx::Format format = cell->format();
                QString cell_name = QString("%1%2").arg(QChar('A' + column - 1)).arg(row);
                cellFormatsSP.insert(cell_name,format);
    //                qDebug() << "Cell" << cell_name << "has format:" << format;
            }
        }
    }

    QMap<QString, QXlsx::Format> cellFormat;
    for (int column = 5; column <= worksheet->dimension().lastColumn(); ++column) {
        int row = 4;
        QXlsx::Cell *cell = worksheet->cellAt(row, column);
        if (cell) {
            QXlsx::Format format = cell->format();
            QString cell_name = QString("%1%2").arg(QChar('A' + column - 1)).arg(row);
            cellFormat.insert(cell_name,format);
//                qDebug() << "Cell" << cell_name << "has format:" << format;
        }
    }
    QXlsx::Format name = worksheet->cellAt("E3")->format();
    QXlsx::Format kod = worksheet->cellAt("F3")->format();
    QXlsx::Format TU = worksheet->cellAt("G3")->format();
    QXlsx::Format manufacturer = worksheet->cellAt("H3")->format();
    QXlsx::Format whereUsed = worksheet->cellAt("L3")->format();
    QXlsx::Format count = worksheet->cellAt("P3")->format();

    ui->boasList->setAcceptDrops(true);

    ui->tabWidget->setCurrentWidget(ui->options);
    ui->tabWidget_2->setCurrentWidget(ui->perechen1);
    ui->perechen->hide();
    ui->specification->hide();
    ui->tabWidget->setTabEnabled(2,false);
    ui->tabWidget->setTabEnabled(3,false);
    ui->tabWidget->setTabEnabled(4,false);

//    ui->boasList->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);

//    ui->razrabLine_2->setText("Иванов");
//    ui->provLine_2->setText("Петров");
//    ui->normLine_2->setText("Сидоров");
//    ui->utvLine_2->setText("Иванов");
//    ui->obozVPs_2->setText("Test RSAL");

//    // Установка белого цвета фона
//    ui->openGLWidget->setStyleSheet("background-color: white;");

//    // Получение размеров виджета и вывод их в консоль
//    QSize widgetSize = ui->openGLWidget->size();
//    qDebug() << "OpenGL Widget Size:" << widgetSize;

//    oGL = new OGLWidget();
//    // Установка размеров листа A4 и рамки с отступами
//    ui->openGLWidget->setPageSize(QSizeF(210, 297)); // мм
//    ui->openGLWidget->setMargins(50); // мм
//    connect(this, &MainWindow::sendOffsetX, oGL, &OGLWidget::getHorizontalOffst);
//    connect(this, &MainWindow::sendOffsetY, oGL, &OGLWidget::getVerticalOffst);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::createSheetXlsxVP(QString filemane, QString sheetName)
{
    QXlsx::Document xlsx(filemane);
    QString sheetName1 = "Лист регистрации изменений";
//    int sheetIndex = xlsx.sheet(sheetName);
    xlsx.addSheet(sheetName);
    xlsx.selectSheet(sheetName);
    xlsx.insertSheet(3, sheetName);
//    QXlsx::Worksheet *worksheet = xlsx.currentWorksheet();

    for (auto it = columnWidth.begin(); it != columnWidth.end(); ++it) {
        xlsx.setColumnWidth(it.key(), it.value());
    }
    for (auto it = rowHeight.begin(); it != rowHeight.end(); ++it) {
        xlsx.setRowHeight(it.key(), it.value());
    }
    for (auto it = cellsWithText.begin(); it != cellsWithText.end(); ++it) {
        xlsx.write(it.key(), it.value());
    }
    for (auto it = cellFormatsVP.begin(); it != cellFormatsVP.end(); ++it) {
        QString cell_name = it.key();
//        QXlsx::Cell *cell = xlsx.cellAt(cell_name);
        QString sellText = xlsx.read(cell_name).toString();
        QXlsx::Format format = it.value();
        xlsx.write(cell_name,sellText,format);
    }
    for (const QXlsx::CellRange &cell_range : merged_cellsVP) {
        xlsx.currentWorksheet()->mergeCells(cell_range.toString());
    }
    xlsx.selectSheet("2");
    QXlsx::Worksheet *worksheet2 = xlsx.currentWorksheet();
    xlsx.selectSheet(sheetName);
    QXlsx::Worksheet *worksheet3 = xlsx.currentWorksheet();
    for (int row = 1; row <= worksheet2->dimension().lastRow(); ++row) {
        for (int column = 1; column <= worksheet2->dimension().lastColumn(); ++column) {
            QXlsx::Cell *cell = worksheet2->cellAt(row, column);
//            QXlsx::Cell *cell2 = worksheet3->cellAt(row, column);
            if (cell) {
                worksheet3->write(row,column,cell->value(), cell->format());
            }
        }
    }
    xlsx.save();
}

void MainWindow::copyPE2listStyle(QMap<int, double> &columnWidthPE, QMap<int, double> &rowHeightPE, QMap<QString, QString> &cellsWithTextPE, QList<QXlsx::CellRange> &merged_cellsPE, QMap<QString, QXlsx::Format> &cellFormatsPE)
{
    QXlsx::Document xlsxPE("perechen.xlsx");
    xlsxPE.selectSheet("2");
    for(int i = 1; i <= 20; ++i){
        double column_width_pixels = xlsxPE.columnWidth(i);
        columnWidthPE.insert(i,column_width_pixels);
    }
    for(int i = 1; i <= 39; ++i){
        double column_width_pixels = xlsxPE.rowHeight(i);
        rowHeightPE.insert(i,column_width_pixels);
    }
    QXlsx::Worksheet *worksheetPE = xlsxPE.currentWorksheet();
    merged_cellsPE = worksheetPE->mergedCells();
    for (int row = 1; row <= worksheetPE->dimension().lastRow(); ++row) {
        for (int column = 1; column <= worksheetPE->dimension().lastColumn(); ++column) {
            QString str = xlsxPE.read(row,column).toString();
            if (!(str.isEmpty())) {
                QString cell_name = QString("%1%2").arg(QChar('A' + column - 1)).arg(row);
    //                qDebug() << "Cell" << cell_name << "contains text:" << str;
                cellsWithTextPE.insert(cell_name,str);
            }
        }
    }
    for (int row = 1; row <= worksheetPE->dimension().lastRow(); ++row) {
        for (int column = 1; column <= worksheetPE->dimension().lastColumn(); ++column) {
            QXlsx::Cell *cell = worksheetPE->cellAt(row, column);
            if (cell) {
                QXlsx::Format format = cell->format();
                QString cell_name = QString("%1%2").arg(QChar('A' + column - 1)).arg(row);
                cellFormatsPE.insert(cell_name,format);
    //                qDebug() << "Cell" << cell_name << "has format:" << format;
            }
        }
    }
}

void MainWindow::copySP2listStyle(QMap<int, double> &columnWidthPE, QMap<int, double> &rowHeightPE, QMap<QString, QString> &cellsWithTextPE, QList<QXlsx::CellRange> &merged_cellsPE, QMap<QString, QXlsx::Format> &cellFormatsPE)
{
    QXlsx::Document xlsxPE("specification.xlsx");
    xlsxPE.selectSheet("2");
    for(int i = 1; i <= 20; ++i){
        double column_width_pixels = xlsxPE.columnWidth(i);
        columnWidthPE.insert(i,column_width_pixels);
    }
    for(int i = 1; i <= 39; ++i){
        double column_width_pixels = xlsxPE.rowHeight(i);
        rowHeightPE.insert(i,column_width_pixels);
    }
    QXlsx::Worksheet *worksheetPE = xlsxPE.currentWorksheet();
    merged_cellsPE = worksheetPE->mergedCells();
    for (int row = 1; row <= worksheetPE->dimension().lastRow(); ++row) {
        for (int column = 1; column <= worksheetPE->dimension().lastColumn(); ++column) {
            QString str = xlsxPE.read(row,column).toString();
            if (!(str.isEmpty())) {
                QString cell_name = QString("%1%2").arg(QChar('A' + column - 1)).arg(row);
    //                qDebug() << "Cell" << cell_name << "contains text:" << str;
                cellsWithTextPE.insert(cell_name,str);
            }
        }
    }
    for (int row = 1; row <= worksheetPE->dimension().lastRow(); ++row) {
        for (int column = 1; column <= worksheetPE->dimension().lastColumn(); ++column) {
            QXlsx::Cell *cell = worksheetPE->cellAt(row, column);
            if (cell) {
                QXlsx::Format format = cell->format();
                QString cell_name = QString("%1%2").arg(QChar('A' + column - 1)).arg(row);
                cellFormatsPE.insert(cell_name,format);
    //                qDebug() << "Cell" << cell_name << "has format:" << format;
            }
        }
    }
}

void MainWindow::createSheetXlsxPE(QString filemane, QString sheetName)
{
    QXlsx::Document xlsx(filemane);
    QMap<int, double> columnWidthPE;
    QMap<int, double> rowHeightPE;
    QMap<QString, QString> cellsWithTextPE;
    QList<QXlsx::CellRange> merged_cellsPE;
    QMap<QString, QXlsx::Format> cellFormatsPE;
    copyPE2listStyle(columnWidthPE, rowHeightPE, cellsWithTextPE, merged_cellsPE, cellFormatsPE);
//    QString sheetName1 = "ЛРИ";
//    int sheetIndex = xlsx.sheet(sheetName);
//    xlsx.copySheet("2",sheetName);
    xlsx.addSheet(sheetName);
    xlsx.selectSheet(sheetName);
//    xlsx.insertSheet(4, sheetName);
//    QXlsx::Worksheet *worksheet = xlsx.currentWorksheet();

    for (auto it = columnWidthPE.begin(); it != columnWidthPE.end(); ++it) {
        xlsx.setColumnWidth(it.key(), it.value());
    }
    for (auto it = rowHeightPE.begin(); it != rowHeightPE.end(); ++it) {
        xlsx.setRowHeight(it.key(), it.value());
    }
    for (auto it = cellsWithTextPE.begin(); it != cellsWithTextPE.end(); ++it) {
        xlsx.write(it.key(), it.value());
    }
    for (const QXlsx::CellRange &cell_range : merged_cellsPE) {
        xlsx.currentWorksheet()->mergeCells(cell_range.toString());
    }
    for (auto it = cellFormatsPE.begin(); it != cellFormatsPE.end(); ++it) {
        QString cell_name = it.key();
//        QXlsx::Cell *cell = xlsx.cellAt(cell_name);
        QString sellText = xlsx.read(cell_name).toString();
        QXlsx::Format format = it.value();
        xlsx.write(cell_name,sellText,format);
    }
    xlsx.selectSheet("2");
    QXlsx::Worksheet *worksheet2 = xlsx.currentWorksheet();
    xlsx.selectSheet(sheetName);
    QXlsx::Worksheet *worksheet3 = xlsx.currentWorksheet();
    for (int row = 1; row <= worksheet2->dimension().lastRow(); ++row) {
        for (int column = 1; column <= worksheet2->dimension().lastColumn(); ++column) {
            QXlsx::Cell *cell = worksheet2->cellAt(row, column);
//            QXlsx::Cell *cell2 = worksheet3->cellAt(row, column);
            if (cell) {
                worksheet3->write(row,column,cell->value(), cell->format());
            }
        }
    }

    xlsx.save();
}

void MainWindow::createSheetXlsxSP(QString filemane, QString sheetName)
{
    QXlsx::Document xlsx(filemane);
    QMap<int, double> columnWidthPE;
    QMap<int, double> rowHeightPE;
    QMap<QString, QString> cellsWithTextPE;
    QList<QXlsx::CellRange> merged_cellsPE;
    QMap<QString, QXlsx::Format> cellFormatsPE;
    copySP2listStyle(columnWidthPE, rowHeightPE, cellsWithTextPE, merged_cellsPE, cellFormatsPE);
//    QString sheetName1 = "ЛРИ";
//    int sheetIndex = xlsx.sheet(sheetName);
//    xlsx.copySheet("2",sheetName);
    xlsx.addSheet(sheetName);
    xlsx.selectSheet(sheetName);
//    xlsx.insertSheet(4, sheetName);
//    QXlsx::Worksheet *worksheet = xlsx.currentWorksheet();

    for (auto it = columnWidthPE.begin(); it != columnWidthPE.end(); ++it) {
        xlsx.setColumnWidth(it.key(), it.value());
    }
    for (auto it = rowHeightPE.begin(); it != rowHeightPE.end(); ++it) {
        xlsx.setRowHeight(it.key(), it.value());
    }
    for (auto it = cellsWithTextPE.begin(); it != cellsWithTextPE.end(); ++it) {
        xlsx.write(it.key(), it.value());
    }
    for (const QXlsx::CellRange &cell_range : merged_cellsPE) {
        xlsx.currentWorksheet()->mergeCells(cell_range.toString());
    }
    for (auto it = cellFormatsPE.begin(); it != cellFormatsPE.end(); ++it) {
        QString cell_name = it.key();
//        QXlsx::Cell *cell = xlsx.cellAt(cell_name);
        QString sellText = xlsx.read(cell_name).toString();
        QXlsx::Format format = it.value();
        xlsx.write(cell_name,sellText,format);
    }
    xlsx.selectSheet("2");
    QXlsx::Worksheet *worksheet2 = xlsx.currentWorksheet();
    xlsx.selectSheet(sheetName);
    QXlsx::Worksheet *worksheet3 = xlsx.currentWorksheet();
    for (int row = 1; row <= worksheet2->dimension().lastRow(); ++row) {
        for (int column = 1; column <= worksheet2->dimension().lastColumn(); ++column) {
            QXlsx::Cell *cell = worksheet2->cellAt(row, column);
//            QXlsx::Cell *cell2 = worksheet3->cellAt(row, column);
            if (cell) {
                worksheet3->write(row,column,cell->value(), cell->format());
            }
        }
    }

    xlsx.save();
}

void MainWindow::containersClear()
{
    // Очистка списка pipelines в каждой структуре
    for (int i = 0; i < containerPE.pages.size(); ++i) {
        containerPE.pages[i].pipelines.clear();
    }
    for (int i = 0; i < containerVP.pages.size(); ++i) {
        containerVP.pages[i].pipelines.clear();
    }
    for (int i = 0; i < containerVPs.pages.size(); ++i) {
        containerVPs.pages[i].pipelines.clear();
    }
    for (int i = 0; i < containerSP.pages.size(); ++i) {
        containerSP.pages[i].pipelines.clear();
    }

    // Очистка списка pivplines в каждой структуре
    for (int i = 0; i < containerPE.pages.size(); ++i) {
        containerPE.pages[i].pivplines.clear();
    }
    for (int i = 0; i < containerVP.pages.size(); ++i) {
        containerVP.pages[i].pivplines.clear();
    }
    for (int i = 0; i < containerVPs.pages.size(); ++i) {
        containerVPs.pages[i].pivplines.clear();
    }
    for (int i = 0; i < containerSP.pages.size(); ++i) {
        containerSP.pages[i].pivplines.clear();
    }

    // Очистка списка pispeclines в каждой структуре
    for (int i = 0; i < containerPE.pages.size(); ++i) {
        containerPE.pages[i].pispeclines.clear();
    }
    for (int i = 0; i < containerVP.pages.size(); ++i) {
        containerVP.pages[i].pispeclines.clear();
    }
    for (int i = 0; i < containerVPs.pages.size(); ++i) {
        containerVPs.pages[i].pispeclines.clear();
    }
    for (int i = 0; i < containerSP.pages.size(); ++i) {
        containerSP.pages[i].pispeclines.clear();
    }

    pipelines.clear();
    pivplines.clear();
    pispeclines.clear();
    pivplinesFin.clear();

    // Очистка списков страниц в каждой структуре
    containerPE.pages.clear();
    containerVP.pages.clear();
    containerVPs.pages.clear();
    containerSP.pages.clear();
}

void MainWindow::createPE(QString filemane)
{
    //создание копии файла
    QString sourceFilePath = "perechen.xlsx";
    QString destinationFilePath = "Перечень элементов "+filemane+"ПЭ3.xlsx";
    QFile sourceFile(sourceFilePath);
    if (!sourceFile.exists()) {
        qDebug() << "Файл не найден";
//          return 1;
    }
    QFileInfo destinationFileInfo(destinationFilePath);
    if (destinationFileInfo.exists()) {
        qDebug() << "Файл уже существует";
        if (!QFile::remove(destinationFilePath)) {
            qDebug() << "Не удалось удалить существующий файл";
            //return 1;
        }
//          return 1;
    }
    if (!sourceFile.copy(destinationFilePath)) {
        qDebug() << "Не удалось скопировать файл";
//          return 1;
    }   

    int pageCount = containerPE.pages.size();

    for (int i = 3; i <= pageCount; i++) {
        createSheetXlsxPE(destinationFilePath,QString::number(i));
    }

    int currentPage = 0; // Номер текущей страницы

    QXlsx::Document forCopy (destinationFilePath);
//    forCopy.deleteSheet("ЛРИ");

    foreach (const Page& page, containerPE.pages) {
        ++currentPage;

        int row = 3;
        forCopy.selectSheet(QString::number(currentPage));
        foreach (const peline& pipeline, page.pipelines) {
            // Определение, в какие ячейки писать данные в зависимости от номера страницы
            int cols[4];
            if(currentPage > 1){
                int rowIndex = 1;
                for (int i = 0; i < 4; ++i) {
                    cols[i] = columsPE[rowIndex][i];
                }
                forCopy.write(36, 16, currentPage);
                forCopy.write(34, 10, filemane + "ПЭ3");
            }
            else{
                int rowIndex = 0;
                for (int i = 0; i < 4; ++i) {
                    cols[i] = columsPE[rowIndex][i];
                }
                forCopy.write(34, 18, pageCount + 1);
                forCopy.write(1, 3, filemane);//перв применяемость
                forCopy.write(33, 6, stampPE.value("Разработал"));//Разработал
                forCopy.write(34, 6, stampPE.value("Проверил"));//Разработал
                forCopy.write(36, 6, stampPE.value("Нормоконтроль"));//Нормоконтроль
                forCopy.write(37, 6, stampPE.value("Утвердил"));//Утвердил
                forCopy.write(35, 6, stampPE.value("Начальник отдела"));//Начальник отдела
                if (stampPE.value("Начальник отдела ( альтернативное название параметра)").isEmpty()){
                    forCopy.write(35, 4, stampPE.value("Начальник отдела"));//Начальник отдела
                }
                else{
                    forCopy.write(35, 4, stampPE.value("Начальник отдела ( альтернативное название параметра)"));//Начальник отдела
                }
                forCopy.write(30, 10, filemane + "ПЭ3");

            }

            if (pipeline.type == "ElementGroup") {
                    // Запись в середину, не жирным шрифтом и с подчеркиванием
                Format font_bold;
                font_bold.setFontBold(false);
                font_bold.setFontUnderline(Format::FontUnderlineSingle);
                font_bold.setHorizontalAlignment(Format::AlignHCenter);
                font_bold.setVerticalAlignment(Format::AlignVCenter);
                QFont gostFont("GOST type B", 14);
                gostFont.setBold(false);
                gostFont.setUnderline(true);
                font_bold.setFont(gostFont);
                forCopy.write(row, cols[1], pipeline.name, font_bold);
            } else {
                // Обычная запись без подчеркивания
                forCopy.write(row, cols[1], pipeline.name);
            }

            forCopy.write(row, cols[0], pipeline.oboz);
//            forCopy.write(row, cols[1], pipeline.name);
            forCopy.write(row, cols[2], pipeline.num);
            forCopy.write(row, cols[3], pipeline.comment);

            ++row;
        }

    }

    // Найти индекс листа с определенным именем
    int index = forCopy.sheetNames().indexOf("ЛРИ");

    // Если лист найден, переместить его в конец
    if (index != -1) {
        forCopy.moveSheet("ЛРИ", forCopy.sheetNames().size() - 1);
    }

    forCopy.selectSheet("ЛРИ");

    forCopy.write(34,12, filemane + "ПЭ3");
    forCopy.write(36,19, pageCount + 1);

    QXlsx::Worksheet *worksheet = forCopy.currentWorksheet();
    QXlsx::Cell *cell = worksheet->cellAt(5, 10);
    QString sellText = forCopy.read(5, 10).toString();
    QXlsx::Format format = cell->format();
    qDebug() << format.bottomBorderStyle() << format.leftBorderStyle() << format.rightBorderStyle();
    QXlsx::Format::BorderStyle style(Format::BorderMedium);
    format.setRightBorderStyle(style);
    qDebug() << format.leftBorderColor() << format.leftBorderStyle() << format.rightBorderStyle();
    forCopy.write(6, 18, sellText, format);
    forCopy.write(6, 19, sellText, format);
    for (int row = 5; row <= 32; ++row){
        for (int column = 13; column <= 17; ++column){
            forCopy.write(row, column, sellText, format);
        }
    }

    forCopy.save();
}

void MainWindow::createPE(QString filemane, QString distPath)
{
    //создание копии файла
    QString sourceFilePath = "perechen.xlsx";
    QString destinationFilePath = distPath + "/Документация/" + "Перечень элементов "+filemane+"ПЭ3.xlsx";
    QFile sourceFile(sourceFilePath);
    if (!sourceFile.exists()) {
        qDebug() << "Файл не найден";
//          return 1;
    }
    QFileInfo destinationFileInfo(destinationFilePath);
    if (destinationFileInfo.exists()) {
        qDebug() << "Файл уже существует";
        if (!QFile::remove(destinationFilePath)) {
            qDebug() << "Не удалось удалить существующий файл";
            //return 1;
        }
//          return 1;
    }
    if (!sourceFile.copy(destinationFilePath)) {
        qDebug() << "Не удалось скопировать файл";
//          return 1;
    }

    QString projectName;
    QLineEdit *lineEdit = findChild<QLineEdit*>(filemane);

    // Проверить, был ли найден QLineEdit
    if (lineEdit) {
        // QLineEdit был найден, можно использовать его
        projectName = lineEdit->text();
        qDebug() << "Текст из QLineEdit: " << projectName;
    } else {
        // QLineEdit с указанным именем не был найден
        qDebug() << "QLineEdit с именем 'имя_объекта' не найден.";
    }

    int pageCount = containerPE.pages.size();

    for (int i = 3; i <= pageCount; i++) {
        createSheetXlsxPE(destinationFilePath,QString::number(i));
    }

    int currentPage = 0; // Номер текущей страницы

    QXlsx::Document forCopy (destinationFilePath);
//    forCopy.deleteSheet("ЛРИ");

    foreach (const Page& page, containerPE.pages) {
        ++currentPage;        
        int row = 3;
        forCopy.selectSheet(QString::number(currentPage));
        qDebug() << "colW" << forCopy.columnWidth(21) << forCopy.columnWidth(21);
        foreach (const peline& pipeline, page.pipelines) {
            QXlsx::Format format;
            QXlsx::Format::BorderStyle styleM(Format::BorderMedium);
            QXlsx::Format::BorderStyle styleT(Format::BorderThin);
            format.setRightBorderStyle(styleM);
            format.setLeftBorderStyle(styleM);
            format.setTopBorderStyle(styleT);
            format.setBottomBorderStyle(styleT);
            format.setHorizontalAlignment(Format::AlignHCenter);
            format.setVerticalAlignment(Format::AlignVCenter);
            QFont gostFont12("GOST type B", 12);
            gostFont12.setBold(false);
            gostFont12.setUnderline(false);
            gostFont12.setItalic(false);
            format.setFont(gostFont12);
            // Определение, в какие ячейки писать данные в зависимости от номера страницы
            int cols[4];
            if(currentPage > 1){
                int rowIndex = 1;
                for (int i = 0; i < 4; ++i) {
                    cols[i] = columsPE[rowIndex][i];
                }
                QXlsx::Format formatFilename;
                QXlsx::Format::BorderStyle styleM(Format::BorderMedium);
                formatFilename.setRightBorderStyle(styleM);
                formatFilename.setLeftBorderStyle(styleM);
                formatFilename.setTopBorderStyle(styleM);
                formatFilename.setBottomBorderStyle(styleM);
                formatFilename.setHorizontalAlignment(Format::AlignHCenter);
                formatFilename.setVerticalAlignment(Format::AlignVCenter);
                QFont gostFont20("GOST type B", 20);
                gostFont20.setBold(false);
                gostFont20.setUnderline(false);
                gostFont20.setItalic(false);
                formatFilename.setFont(gostFont20);
                forCopy.write(36, 16, currentPage);
                forCopy.write(34, 10, filemane + "ПЭ3", formatFilename);
            }
            else{
                int rowIndex = 0;
                for (int i = 0; i < 4; ++i) {
                    cols[i] = columsPE[rowIndex][i];
                }
                forCopy.write(34, 18, pageCount + 1);
                forCopy.write(33, 10, projectName);
                forCopy.write(1, 3, filemane);//перв применяемость
                forCopy.write(33, 6, stampPE.value("Разработал"));//Разработал
                forCopy.write(34, 6, stampPE.value("Проверил"));//Разработал
                forCopy.write(36, 6, stampPE.value("Нормоконтроль"));//Нормоконтроль
                forCopy.write(37, 6, stampPE.value("Утвердил"));//Утвердил
                forCopy.write(35, 6, stampPE.value("Начальник отдела"));//Начальник отдела
                if (stampPE.value("Начальник отдела ( альтернативное название параметра)").isEmpty()){
                    forCopy.write(35, 4, stampPE.value("Начальник отдела"));//Начальник отдела
                }
                else{
                    forCopy.write(35, 4, stampPE.value("Начальник отдела ( альтернативное название параметра)"));//Начальник отдела
                }
                forCopy.write(30, 10, filemane + "ПЭ3");

            }

            if (pipeline.type == "ElementGroup") {
                    // Запись в середину, не жирным шрифтом и с подчеркиванием
                Format font_bold;
                font_bold.setFontBold(false);
                font_bold.setFontUnderline(Format::FontUnderlineSingle);
                font_bold.setHorizontalAlignment(Format::AlignHCenter);
                font_bold.setVerticalAlignment(Format::AlignVCenter);
                font_bold.setRightBorderStyle(styleM);
                font_bold.setLeftBorderStyle(styleM);
                font_bold.setTopBorderStyle(styleT);
                font_bold.setBottomBorderStyle(styleT);
                QFont gostFont("GOST type B", 14);
                gostFont.setBold(false);
                gostFont.setUnderline(true);
                font_bold.setFont(gostFont);
                forCopy.write(row, cols[1], pipeline.name, font_bold);
            } else {
                // Обычная запись без подчеркивания
                forCopy.write(row, cols[1], pipeline.name);
            }

            forCopy.write(row, cols[0], pipeline.oboz, format);
//            forCopy.write(row, cols[1], pipeline.name);
            forCopy.write(row, cols[2], pipeline.num, format);
            forCopy.write(row, cols[3], pipeline.comment, format);

            ++row;
        }
        if(currentPage == pageCount){
            QXlsx::Worksheet *worksheet = forCopy.currentWorksheet();
            for (int row = 2; row <= 32; row++) {
                for (int col = 4; col <= 16; col++) {
                    QXlsx::Cell *cell = worksheet->cellAt(row, col);
                    QString sellText = forCopy.read(row, col).toString();
                    QXlsx::Format format = cell->format();
                    QXlsx::Format::BorderStyle style(Format::BorderMedium);
                    format.setRightBorderStyle(style);
                    forCopy.write(row, col, sellText, format);
                }
            }
        }
    }

    // Найти индекс листа с определенным именем
    int index = forCopy.sheetNames().indexOf("ЛРИ");

    // Если лист найден, переместить его в конец
    if (index != -1) {
        forCopy.moveSheet("ЛРИ", forCopy.sheetNames().size() - 1);
    }

    forCopy.selectSheet("ЛРИ");

    forCopy.write(34,12, filemane + "ПЭ3");
    forCopy.write(36,19, pageCount + 1);

    QXlsx::Worksheet *worksheet = forCopy.currentWorksheet();
    QXlsx::Cell *cell = worksheet->cellAt(5, 10);
    QString sellText = forCopy.read(5, 10).toString();
    QXlsx::Format format = cell->format();
    qDebug() << format.bottomBorderStyle() << format.leftBorderStyle() << format.rightBorderStyle();
    QXlsx::Format::BorderStyle style(Format::BorderMedium);
    format.setRightBorderStyle(style);
    qDebug() << format.leftBorderColor() << format.leftBorderStyle() << format.rightBorderStyle();
    forCopy.write(6, 18, sellText, format);
    forCopy.write(6, 19, sellText, format);
    for (int row = 5; row <= 32; ++row){
        for (int column = 13; column <= 17; ++column){
            forCopy.write(row, column, sellText, format);
        }
    }

    forCopy.save();
}

void MainWindow::createSP(QString filemane)
{
    //создание копии файла
    QString sourceFilePath = "specification.xlsx";
    QString destinationFilePath = "Спецификация "+filemane+".xlsx";
    QFile sourceFile(sourceFilePath);    
    if (!sourceFile.exists()) {
        qDebug() << "Файл не найден";
//          return 1;
    }
    QFileInfo destinationFileInfo(destinationFilePath);
    if (destinationFileInfo.exists()) {
        qDebug() << "Файл уже существует";
        if (!QFile::remove(destinationFilePath)) {
            qDebug() << "Не удалось удалить существующий файл";
            //return 1;
        }
//          return 1;
    }
    if (!sourceFile.copy(destinationFilePath)) {
        qDebug() << "Не удалось скопировать файл";
//          return 1;
    }    

    int pageCount = containerSP.pages.size();

    for (int i = 3; i <= pageCount; i++) {
        createSheetXlsxSP(destinationFilePath,QString::number(i));
    }

    int currentPage = 0; // Номер текущей страницы    

    QXlsx::Document forCopy (destinationFilePath);
//    forCopy.deleteSheet("ЛРИ");

    foreach (const Page& page, containerSP.pages) {
        ++currentPage;

        int row = 3;
        forCopy.selectSheet(QString::number(currentPage));
        foreach (const specline& pispecline, page.pispeclines) {
            // Определение, в какие ячейки писать данные в зависимости от номера страницы
            int cols[6];
            if(currentPage > 1){
                int rowIndex = 1;
                for (int i = 0; i < 6; ++i) {
                    cols[i] = columsSP[rowIndex][i];
                }
                forCopy.write(37, 22, currentPage);
                forCopy.write(35, 12, filemane);
            }
            else{
                int rowIndex = 0;
                for (int i = 0; i < 6; ++i) {
                    cols[i] = columsSP[rowIndex][i];
                }
                forCopy.write(36, 22, pageCount + 1);
                forCopy.write(1, 3, filemane);//перв применяемость
                forCopy.write(35, 8, stampSP.value("Разработал"));//Разработал
                forCopy.write(36, 8, stampSP.value("Проверил"));//Разработал
                forCopy.write(38, 8, stampSP.value("Нормоконтроль"));//Нормоконтроль
                forCopy.write(39, 8, stampSP.value("Утвердил"));//Утвердил
                forCopy.write(37, 8, stampSP.value("Начальник отдела"));//Начальник отдела
                if (stampSP.value("Начальник отдела ( альтернативное название параметра)").isEmpty()){
                    forCopy.write(37, 4, stampSP.value("Начальник отдела"));//Начальник отдела
                }
                else{
                    forCopy.write(37, 4, stampSP.value("Начальник отдела ( альтернативное название параметра)"));//Начальник отдела
                }
                forCopy.write(32, 12, filemane);

            }

            if (pispecline.underline) {
                    // Запись в середину, не жирным шрифтом и с подчеркиванием
                Format font_bold;
                font_bold.setFontBold(false);
                font_bold.setFontUnderline(Format::FontUnderlineSingle);
                font_bold.setHorizontalAlignment(Format::AlignHCenter);
                font_bold.setVerticalAlignment(Format::AlignVCenter);
                QFont gostFont("GOST type B", 14);
                gostFont.setBold(false);
                gostFont.setUnderline(true);
                font_bold.setFont(gostFont);
                forCopy.write(row, cols[3], pispecline.name, font_bold);
            } else {
                // Обычная запись без подчеркивания
                forCopy.write(row, cols[3], pispecline.name);
            }

            forCopy.write(row, cols[0], pispecline.format);
            forCopy.write(row, cols[1], pispecline.pos);
            forCopy.write(row, cols[2], pispecline.oboz);
            forCopy.write(row, cols[4], pispecline.num);
            forCopy.write(row, cols[5], pispecline.comment);

            ++row;
        }

    }
    forCopy.selectSheet("ЛРИ");
    forCopy.write(37, 19, pageCount + 1);
    forCopy.write(35, 12, filemane);

    // Найти индекс листа с определенным именем
    int index = forCopy.sheetNames().indexOf("ЛРИ");

    // Если лист найден, переместить его в конец
    if (index != -1) {
        forCopy.moveSheet("ЛРИ", forCopy.sheetNames().size() - 1);
    }

    forCopy.save();
}

void MainWindow::createSP(QString filemane, QString distPath)
{
    //создание копии файла
    QString sourceFilePath = "specification.xlsx";
    QString destinationFilePath = distPath + "/Документация/Спецификация "+filemane+".xlsx";
    QFile sourceFile(sourceFilePath);
    if (!sourceFile.exists()) {
        qDebug() << "Файл не найден";
//          return 1;
    }
    QFileInfo destinationFileInfo(destinationFilePath);
    if (destinationFileInfo.exists()) {
        qDebug() << "Файл уже существует";
        if (!QFile::remove(destinationFilePath)) {
            qDebug() << "Не удалось удалить существующий файл";
            //return 1;
        }
//          return 1;
    }
    if (!sourceFile.copy(destinationFilePath)) {
        qDebug() << "Не удалось скопировать файл";
//          return 1;
    }

    QString projectName;
    QLineEdit *lineEdit = findChild<QLineEdit*>(filemane);

    // Проверить, был ли найден QLineEdit
    if (lineEdit) {
        // QLineEdit был найден, можно использовать его
        projectName = lineEdit->text();
        qDebug() << "Текст из QLineEdit: " << projectName;
    } else {
        // QLineEdit с указанным именем не был найден
        qDebug() << "QLineEdit с именем 'имя_объекта' не найден.";
    }

    int pageCount = containerSP.pages.size();

    for (int i = 3; i <= pageCount; i++) {
        createSheetXlsxSP(destinationFilePath,QString::number(i));
    }

    int currentPage = 0; // Номер текущей страницы

    QXlsx::Document forCopy (destinationFilePath);
//    forCopy.deleteSheet("ЛРИ");

    foreach (const Page& page, containerSP.pages) {
        ++currentPage;

        int row = 3;
        forCopy.selectSheet(QString::number(currentPage));
        qDebug() << "colWsp" << forCopy.columnWidth(21) << forCopy.columnWidth(22);
        foreach (const specline& pispecline, page.pispeclines) {
            // Определение, в какие ячейки писать данные в зависимости от номера страницы
            int cols[6];
            if(currentPage > 1){
                int rowIndex = 1;
                for (int i = 0; i < 6; ++i) {
                    cols[i] = columsSP[rowIndex][i];
                }
                QXlsx::Format formatFilename;
                QXlsx::Format::BorderStyle styleM(Format::BorderMedium);
                formatFilename.setRightBorderStyle(styleM);
                formatFilename.setLeftBorderStyle(styleM);
                formatFilename.setTopBorderStyle(styleM);
                formatFilename.setBottomBorderStyle(styleM);
                formatFilename.setHorizontalAlignment(Format::AlignHCenter);
                formatFilename.setVerticalAlignment(Format::AlignVCenter);
                QFont gostFont20("GOST type B", 12);
                gostFont20.setBold(false);
                gostFont20.setUnderline(false);
                gostFont20.setItalic(false);
                formatFilename.setFont(gostFont20);
                forCopy.write(37, 22, currentPage, formatFilename);
                forCopy.write(35, 12, filemane);
                forCopy.setColumnWidth(21, 5.71094);
                forCopy.setColumnWidth(22, 5.14063);
            }
            else{
                int rowIndex = 0;
                for (int i = 0; i < 6; ++i) {
                    cols[i] = columsSP[rowIndex][i];
                }
                QXlsx::Format formatFilename;
                QXlsx::Format::BorderStyle styleM(Format::BorderMedium);
                formatFilename.setRightBorderStyle(styleM);
                formatFilename.setLeftBorderStyle(styleM);
                formatFilename.setTopBorderStyle(styleM);
                formatFilename.setBottomBorderStyle(styleM);
                formatFilename.setHorizontalAlignment(Format::AlignHCenter);
                formatFilename.setVerticalAlignment(Format::AlignVCenter);
                QFont gostFont20("GOST type B", 12);
                gostFont20.setBold(false);
                gostFont20.setUnderline(false);
                gostFont20.setItalic(false);
                formatFilename.setFont(gostFont20);
                forCopy.write(36, 22, pageCount + 1,formatFilename);
                forCopy.write(35, 12, projectName);
                forCopy.write(1, 3, filemane);//перв применяемость
                forCopy.write(35, 8, stampSP.value("Разработал"));//Разработал
                forCopy.write(36, 8, stampSP.value("Проверил"));//Разработал
                forCopy.write(38, 8, stampSP.value("Нормоконтроль"));//Нормоконтроль
                forCopy.write(39, 8, stampSP.value("Утвердил"));//Утвердил
                forCopy.write(37, 8, stampSP.value("Начальник отдела"));//Начальник отдела
                if (stampSP.value("Начальник отдела ( альтернативное название параметра)").isEmpty()){
                    forCopy.write(37, 4, stampSP.value("Начальник отдела"));//Начальник отдела
                }
                else{
                    forCopy.write(37, 4, stampSP.value("Начальник отдела ( альтернативное название параметра)"));//Начальник отдела
                }
                forCopy.write(32, 12, filemane);

            }

            if (pispecline.underline) {
                    // Запись в середину, не жирным шрифтом и с подчеркиванием
                Format font_bold;
                font_bold.setFontBold(false);
                font_bold.setFontUnderline(Format::FontUnderlineSingle);
                font_bold.setHorizontalAlignment(Format::AlignHCenter);
                font_bold.setVerticalAlignment(Format::AlignVCenter);
                QFont gostFont("GOST type B", 14);
                gostFont.setBold(false);
                gostFont.setUnderline(true);
                font_bold.setFont(gostFont);
                forCopy.write(row, cols[3], pispecline.name, font_bold);
            } else {
                // Обычная запись без подчеркивания
                forCopy.write(row, cols[3], pispecline.name);
            }

            forCopy.write(row, cols[0], pispecline.format);
            forCopy.write(row, cols[1], pispecline.pos);
            forCopy.write(row, cols[2], pispecline.oboz);
            forCopy.write(row, cols[4], pispecline.num);
            forCopy.write(row, cols[5], pispecline.comment);            
            qDebug() << "colWspNew" << forCopy.columnWidth(21) << forCopy.columnWidth(22);
            ++row;
        }

    }
    forCopy.selectSheet("ЛРИ");
    forCopy.write(37, 19, pageCount + 1);
    forCopy.write(35, 12, filemane);

    // Найти индекс листа с определенным именем
    int index = forCopy.sheetNames().indexOf("ЛРИ");

    // Если лист найден, переместить его в конец
    if (index != -1) {
        forCopy.moveSheet("ЛРИ", forCopy.sheetNames().size() - 1);
    }

    forCopy.save();
}

void MainWindow::createVP(QString filemane)
{
    //создание копии файла
    QString sourceFilePath = "Vedomost.xlsx";
    QString destinationFilePath = "Ведомость покупных "+filemane+"ВП.xlsx";
    QFile sourceFile(sourceFilePath);
    if (!sourceFile.exists()) {
        qDebug() << "Файл не найден";
//          return 1;
    }
    QFileInfo destinationFileInfo(destinationFilePath);
    if (destinationFileInfo.exists()) {
        qDebug() << "Файл уже существует";
        if (!QFile::remove(destinationFilePath)) {
            qDebug() << "Не удалось удалить существующий файл";
            //return 1;
        }
//          return 1;
    }
    if (!sourceFile.copy(destinationFilePath)) {
        qDebug() << "Не удалось скопировать файл";
//          return 1;
    }

    QString projectName;
    QLineEdit *lineEdit = findChild<QLineEdit*>(filemane);

    // Проверить, был ли найден QLineEdit
    if (lineEdit) {
        // QLineEdit был найден, можно использовать его
        projectName = lineEdit->text();
        qDebug() << "Текст из QLineEdit: " << projectName;
    } else {
        // QLineEdit с указанным именем не был найден
        qDebug() << "QLineEdit с именем 'имя_объекта' не найден.";
    }

    int pageCount = containerVP.pages.size();

    for (int i = 3; i <= pageCount; i++) {
        createSheetXlsxVP(destinationFilePath,QString::number(i));
    }

    int currentPage = 0; // Номер текущей страницы

    QXlsx::Document forCopy (destinationFilePath);
//    forCopy.deleteSheet("ЛРИ");

    foreach (const Page& page, containerVP.pages) {
        ++currentPage;

        int row = 4;
        forCopy.selectSheet(QString::number(currentPage));
        foreach (const vpline& pivpline, page.pivplines) {
            // Определение, в какие ячейки писать данные в зависимости от номера страницы
            int cols[8];
            if(currentPage > 1){
                int rowIndex = 1;
                for (int i = 0; i < 8; ++i) {
                    cols[i] = columsVP[rowIndex][i];
                }
                forCopy.write(36, 21, currentPage);
                forCopy.write(34, 15, filemane + "ВП");
            }
            else{
                int rowIndex = 0;
                for (int i = 0; i < 8; ++i) {
                    cols[i] = columsVP[rowIndex][i];
                }
                forCopy.write(36, 29, pageCount + 1);
                forCopy.write(35, 17, projectName);
                forCopy.write(1, 3, filemane);//перв применяемость
                forCopy.write(35, 13, ui->razrabLine_2->text());//Разработал
                forCopy.write(36, 13, ui->provLine_2->text());//Разработал
                forCopy.write(38, 13, ui->normLine_2->text());//Нормоконтроль
                forCopy.write(39, 13, ui->utvLine_2->text());//Утвердил
                forCopy.write(37, 13, ui->nachalnicLine_2->text());//Начальник отдела
                if (stampSP.value("Начальник отдела ( альтернативное название параметра)").isEmpty()){
                    forCopy.write(37, 9, "Начальник отдела");//Начальник отдела
                }
                else{
                    forCopy.write(37, 9, ui->nachalnicAltLine_2->text());//Начальник отдела
                }
                forCopy.write(32, 17, filemane + "ВП");

            }

            if (pivpline.type == "ElementGroup") {
                    // Запись в середину, не жирным шрифтом и с подчеркиванием
                Format font_bold;
                font_bold.setFontBold(false);
                font_bold.setFontUnderline(Format::FontUnderlineSingle);
                font_bold.setHorizontalAlignment(Format::AlignHCenter);
                font_bold.setVerticalAlignment(Format::AlignVCenter);
                QFont gostFont("GOST type B", 14);
                gostFont.setBold(false);
                gostFont.setUnderline(true);
                font_bold.setFont(gostFont);
                forCopy.write(row, cols[0], pivpline.name, font_bold);
            } else {
                // Обычная запись без подчеркивания
                forCopy.write(row, cols[0], pivpline.name);
            }

            forCopy.write(row, cols[1], pivpline.kod);
//            forCopy.write(row, cols[1], pivpline.pos);
            forCopy.write(row, cols[2], pivpline.oboz);
            forCopy.write(row, cols[3], pivpline.post);
            forCopy.write(row, cols[4], pivpline.vhodit);
            forCopy.write(row, cols[5], pivpline.num);
            forCopy.write(row, cols[6], pivpline.num);
            forCopy.write(row, cols[7], pivpline.comment);

            ++row;
        }

    }
    forCopy.selectSheet("Лист регистрации изменений");
    forCopy.write(37, 19, pageCount + 1);
    forCopy.write(35, 12, filemane + "ВП");

    // Найти индекс листа с определенным именем
    int index = forCopy.sheetNames().indexOf("Лист регистрации изменений");

    // Если лист найден, переместить его в конец
    if (index != -1) {
        forCopy.moveSheet("Лист регистрации изменений", forCopy.sheetNames().size() - 1);
    }

    forCopy.save();
}

void MainWindow::createVP(QString filemane, QString distPath)
{
    //создание копии файла
    QString sourceFilePath = "Vedomost.xlsx";
    QString destinationFilePath = distPath + "/Документация/Ведомость покупных "+filemane+"ВП.xlsx";
    QFile sourceFile(sourceFilePath);
    if (!sourceFile.exists()) {
        qDebug() << "Файл не найден";
//          return 1;
    }
    QFileInfo destinationFileInfo(destinationFilePath);
    if (destinationFileInfo.exists()) {
        qDebug() << "Файл уже существует";
        if (!QFile::remove(destinationFilePath)) {
            qDebug() << "Не удалось удалить существующий файл";
            //return 1;
        }
//          return 1;
    }
    if (!sourceFile.copy(destinationFilePath)) {
        qDebug() << "Не удалось скопировать файл";
//          return 1;
    }

    QString projectName;
    QLineEdit *lineEdit = findChild<QLineEdit*>(filemane);

    // Проверить, был ли найден QLineEdit
    if (lineEdit) {
        // QLineEdit был найден, можно использовать его
        projectName = lineEdit->text();
        qDebug() << "Текст из QLineEdit: " << projectName;
    } else {
        // QLineEdit с указанным именем не был найден
        qDebug() << "QLineEdit с именем 'имя_объекта' не найден.";
    }

    int pageCount = containerVP.pages.size();

    for (int i = 3; i <= pageCount; i++) {
        createSheetXlsxVP(destinationFilePath,QString::number(i));
    }

    int currentPage = 0; // Номер текущей страницы

    QXlsx::Document forCopy (destinationFilePath);
//    forCopy.deleteSheet("ЛРИ");

    foreach (const Page& page, containerVP.pages) {
        ++currentPage;

        int row = 4;
        forCopy.selectSheet(QString::number(currentPage));
        foreach (const vpline& pivpline, page.pivplines) {
            // Определение, в какие ячейки писать данные в зависимости от номера страницы
            int cols[8];
            if(currentPage > 1){
                int rowIndex = 1;
                for (int i = 0; i < 8; ++i) {
                    cols[i] = columsVP[rowIndex][i];
                }
                forCopy.write(36, 21, currentPage);
                forCopy.write(34, 15, filemane + "ВП");
            }
            else{
                int rowIndex = 0;
                for (int i = 0; i < 8; ++i) {
                    cols[i] = columsVP[rowIndex][i];
                }
                forCopy.write(36, 29, pageCount + 1);
                forCopy.write(35, 17, projectName);
                forCopy.write(1, 3, filemane);//перв применяемость
                forCopy.write(35, 13, stampVP.value("Разработал"));//Разработал
                forCopy.write(36, 13, stampVP.value("Проверил"));//Проверил
                forCopy.write(38, 13, stampVP.value("Нормоконтроль"));//Нормоконтроль
                forCopy.write(39, 13, stampVP.value("Утвердил"));//Утвердил
                forCopy.write(37, 13, stampVP.value("Начальник отдела"));//Начальник отдела
                if (stampVP.value("Начальник отдела ( альтернативное название параметра)").isEmpty()){
                    forCopy.write(37, 9, "Начальник отдела");//Начальник отдела
                }
                else{
                    forCopy.write(37, 9, stampVP.value("Начальник отдела ( альтернативное название параметра)"));//Начальник отдела
                }
                forCopy.write(32, 17, filemane + "ВП");

            }

            if (pivpline.type == "ElementGroup") {
                    // Запись в середину, не жирным шрифтом и с подчеркиванием
                Format font_bold;
                font_bold.setFontBold(false);
                font_bold.setFontUnderline(Format::FontUnderlineSingle);
                font_bold.setHorizontalAlignment(Format::AlignHCenter);
                font_bold.setVerticalAlignment(Format::AlignVCenter);
                QFont gostFont("GOST type B", 14);
                gostFont.setBold(false);
                gostFont.setUnderline(true);
                font_bold.setFont(gostFont);
                forCopy.write(row, cols[0], pivpline.name, font_bold);
            } else {
                // Обычная запись без подчеркивания
                forCopy.write(row, cols[0], pivpline.name);
                if(!pivpline.name.isEmpty()){
                    if(pivpline.vhodit.isEmpty()){
                        forCopy.write(row, cols[4], projectOboz);
                    }else{
                        forCopy.write(row, cols[4], pivpline.vhodit);
                    }
                }

            }

            forCopy.write(row, cols[1], pivpline.kod);
//            forCopy.write(row, cols[1], pivpline.pos);
            forCopy.write(row, cols[2], pivpline.oboz);
            forCopy.write(row, cols[3], pivpline.post);            
            forCopy.write(row, cols[5], pivpline.num);
            forCopy.write(row, cols[6], pivpline.num);
            forCopy.write(row, cols[7], pivpline.comment);

            ++row;
        }

    }
    forCopy.selectSheet("Лист регистрации изменений");
    forCopy.write(37, 19, pageCount + 1);
    forCopy.write(35, 12, filemane + "ВП");

    // Найти индекс листа с определенным именем
    int index = forCopy.sheetNames().indexOf("Лист регистрации изменений");

    // Если лист найден, переместить его в конец
    if (index != -1) {
        forCopy.moveSheet("Лист регистрации изменений", forCopy.sheetNames().size() - 1);
    }

    forCopy.save();
}

void MainWindow::createVPs(QString filemane)
{
    //создание копии файла
    QString sourceFilePath = "Vedomost.xlsx";    
    QString destinationFilePath = "Ведомость покупных "+ui->obozVPs_2->text()+"ВП.xlsx";
    QFile sourceFile(sourceFilePath);    
    if (!sourceFile.exists()) {
        qDebug() << "Файл не найден";
//          return 1;
    }
    QFileInfo destinationFileInfo(destinationFilePath);
    if (destinationFileInfo.exists()) {
        qDebug() << "Файл уже существует";
        if (!QFile::remove(destinationFilePath)) {
            qDebug() << "Не удалось удалить существующий файл";
            //return 1;
        }
//          return 1;
    }
    if (!sourceFile.copy(destinationFilePath)) {
        qDebug() << "Не удалось скопировать файл";
//          return 1;
    }    

    int pageCount = containerVPs.pages.size();

    for (int i = 3; i <= pageCount; i++) {
        createSheetXlsxVP(destinationFilePath,QString::number(i));
    }

    int currentPage = 0; // Номер текущей страницы    

    QXlsx::Document forCopy (destinationFilePath);
//    forCopy.deleteSheet("ЛРИ");
    foreach (const Page& page, containerVPs.pages) {
        ++currentPage;
        int rowCount, lastRow;
        if(currentPage < 2){
            rowCount = 25;
            lastRow = 24;
        }
        else{
            rowCount = 31;
            lastRow = 30;
        }
        int row = 4;
        forCopy.selectSheet(QString::number(currentPage));
        forCopy.write(3,4, QString::number((currentPage-1) * rowCount + 1));
        foreach (const vpline& pivpline, page.pivplines) {
            // Определение, в какие ячейки писать данные в зависимости от номера страницы
            int cols[8];
            if(currentPage > 1){
                int rowIndex = 1;
                for (int i = 0; i < 8; ++i) {
                    cols[i] = columsVP[rowIndex][i];
                }
                forCopy.write(36, 21, currentPage);
                forCopy.write(34, 15, filemane + "ВП");
            }
            else{
                int rowIndex = 0;
                for (int i = 0; i < 8; ++i) {
                    cols[i] = columsVP[rowIndex][i];
                }
                forCopy.write(36, 29, pageCount + 1);
                forCopy.write(1, 3, filemane);//перв применяемость
                forCopy.write(35, 13, ui->razrabLine_2->text());//Разработал
                forCopy.write(36, 13, ui->provLine_2->text());//Разработал
                forCopy.write(38, 13, ui->normLine_2->text());//Нормоконтроль
                forCopy.write(39, 13, ui->utvLine_2->text());//Утвердил
                forCopy.write(37, 13, ui->nachalnicLine_2->text());//Начальник отдела
                if (stampSP.value("Начальник отдела ( альтернативное название параметра)").isEmpty()){
                    forCopy.write(37, 9, "Начальник отдела");//Начальник отдела
                }
                else{
                    forCopy.write(37, 9, ui->nachalnicAltLine_2->text());//Начальник отдела
                }
                forCopy.write(32, 17, filemane + "ВП");

            }

            if (pivpline.type == "ElementGroup") {
                    // Запись в середину, не жирным шрифтом и с подчеркиванием
                Format font_bold;
                font_bold.setFontBold(false);
                font_bold.setFontUnderline(Format::FontUnderlineSingle);
                font_bold.setHorizontalAlignment(Format::AlignHCenter);
                font_bold.setVerticalAlignment(Format::AlignVCenter);
                QFont gostFont("GOST type B", 14);
                gostFont.setBold(false);
                gostFont.setUnderline(true);
                font_bold.setFont(gostFont);
                forCopy.write(row, cols[0], pivpline.name, font_bold);
            } else {
                // Обычная запись без подчеркивания
                forCopy.write(row, cols[0], pivpline.name);
            }

            forCopy.write(row, cols[1], pivpline.kod);
//            forCopy.write(row, cols[1], pivpline.pos);
            if(pivpline.oboz.length() > 30){
                Format font_bold;
                font_bold.setFontBold(false);
                font_bold.setTextWrap(true);
                font_bold.setHorizontalAlignment(Format::AlignHCenter);
                font_bold.setVerticalAlignment(Format::AlignVCenter);
                QFont gostFont("GOST type B", 10);
                gostFont.setBold(false);
                gostFont.setUnderline(false);
                font_bold.setFont(gostFont);
                forCopy.write(row, cols[2], pivpline.oboz, font_bold);
            }
            else{
                forCopy.write(row, cols[2], pivpline.oboz);
            }
            if(pivpline.post.length() > 35){
                Format font_bold;
                font_bold.setFontBold(false);
                font_bold.setTextWrap(true);
                font_bold.setHorizontalAlignment(Format::AlignHCenter);
                font_bold.setVerticalAlignment(Format::AlignVCenter);
                QFont gostFont("GOST type B", 10);
                gostFont.setBold(false);
                gostFont.setUnderline(false);
                font_bold.setFont(gostFont);
                forCopy.write(row, cols[3], pivpline.post, font_bold);
            }
            else{
                forCopy.write(row, cols[3], pivpline.post);
            }
            forCopy.write(lastRow + 2,4, QString::number((currentPage-1) * rowCount + lastRow));
            forCopy.write(lastRow + 3,4, QString::number((currentPage-1) * rowCount + lastRow + 1));
            forCopy.write(row,4, QString::number((currentPage-1) * rowCount + row - 2));
            forCopy.write(row, cols[4], pivpline.vhodit);
            forCopy.write(row, cols[5], pivpline.num);
            forCopy.write(row, cols[6], pivpline.num);
            forCopy.write(row, cols[7], pivpline.comment);

            ++row;
        }

    }
    forCopy.selectSheet("Лист регистрации изменений");
    forCopy.write(37, 19, pageCount + 1);
    forCopy.write(35, 12, filemane + "ВП");

    // Найти индекс листа с определенным именем
    int index = forCopy.sheetNames().indexOf("Лист регистрации изменений");

    // Если лист найден, переместить его в конец
    if (index != -1) {
        forCopy.moveSheet("Лист регистрации изменений", forCopy.sheetNames().size() - 1);
    }


    forCopy.save();
}

void MainWindow::createVPs(QString filemane, QString distPath)
{
    //создание копии файла
    QString sourceFilePath = "Vedomost.xlsx";
    QString destinationFilePath = outDir + "/Групповая ведомость покупных "+ui->obozVPs_2->text()+"ВП.xlsx";
    QFile sourceFile(sourceFilePath);
    if (!sourceFile.exists()) {
        qDebug() << "Файл не найден";
//          return 1;
    }
    QFileInfo destinationFileInfo(destinationFilePath);
    if (destinationFileInfo.exists()) {
        qDebug() << "Файл уже существует";
        if (!QFile::remove(destinationFilePath)) {
            qDebug() << "Не удалось удалить существующий файл";
            //return 1;
        }
//          return 1;
    }
    if (!sourceFile.copy(destinationFilePath)) {
        qDebug() << "Не удалось скопировать файл";
//          return 1;
    }

    int pageCount = containerVPs.pages.size();

    for (int i = 3; i <= pageCount; i++) {
        createSheetXlsxVP(destinationFilePath,QString::number(i));
    }

    int currentPage = 0; // Номер текущей страницы

    QXlsx::Document forCopy (destinationFilePath);
//    forCopy.deleteSheet("ЛРИ");
    foreach (const Page& page, containerVPs.pages) {
        ++currentPage;
        int rowCount, lastRow;
        if(currentPage < 2){
            rowCount = 25;
            lastRow = 24;
        }
        else{
            rowCount = 31;
            lastRow = 30;
        }
        int row = 4;
        forCopy.selectSheet(QString::number(currentPage));
        forCopy.write(3,4, QString::number((currentPage-1) * rowCount + 1));
        foreach (const vpline& pivpline, page.pivplines) {
            // Определение, в какие ячейки писать данные в зависимости от номера страницы
            int cols[8];
            if(currentPage > 1){
                int rowIndex = 1;
                for (int i = 0; i < 8; ++i) {
                    cols[i] = columsVP[rowIndex][i];
                }
                forCopy.write(36, 21, currentPage);
                forCopy.write(34, 15, filemane + "ВП");
            }
            else{
                int rowIndex = 0;
                for (int i = 0; i < 8; ++i) {
                    cols[i] = columsVP[rowIndex][i];
                }
                forCopy.write(36, 29, pageCount + 1);
                forCopy.write(35, 17, ui->projectName->text());
                forCopy.write(1, 3, filemane);//перв применяемость
                forCopy.write(35, 13, ui->razrabLine_2->text());//Разработал
                forCopy.write(36, 13, ui->provLine_2->text());//Разработал
                forCopy.write(38, 13, ui->normLine_2->text());//Нормоконтроль
                forCopy.write(39, 13, ui->utvLine_2->text());//Утвердил
                forCopy.write(37, 13, ui->nachalnicLine_2->text());//Начальник отдела
                if (stampSP.value("Начальник отдела ( альтернативное название параметра)").isEmpty()){
                    forCopy.write(37, 9, "Начальник отдела");//Начальник отдела
                }
                else{
                    forCopy.write(37, 9, ui->nachalnicAltLine_2->text());//Начальник отдела
                }
                forCopy.write(32, 17, filemane + "ВП");

            }

            if (pivpline.type == "ElementGroup") {
                    // Запись в середину, не жирным шрифтом и с подчеркиванием
                Format font_bold;
                font_bold.setFontBold(false);
                font_bold.setFontUnderline(Format::FontUnderlineSingle);
                font_bold.setHorizontalAlignment(Format::AlignHCenter);
                font_bold.setVerticalAlignment(Format::AlignVCenter);
                QFont gostFont("GOST type B", 14);
                gostFont.setBold(false);
                gostFont.setUnderline(true);
                font_bold.setFont(gostFont);
                forCopy.write(row, cols[0], pivpline.name, font_bold);
            } else {
                // Обычная запись без подчеркивания
                forCopy.write(row, cols[0], pivpline.name);
            }

            forCopy.write(row, cols[1], pivpline.kod);
//            forCopy.write(row, cols[1], pivpline.pos);
            if(pivpline.oboz.length() > 30){
                Format font_bold;
                font_bold.setFontBold(false);
                font_bold.setTextWrap(true);
                font_bold.setHorizontalAlignment(Format::AlignHCenter);
                font_bold.setVerticalAlignment(Format::AlignVCenter);
                QFont gostFont("GOST type B", 10);
                gostFont.setBold(false);
                gostFont.setUnderline(false);
                font_bold.setFont(gostFont);
                forCopy.write(row, cols[2], pivpline.oboz, font_bold);
            }
            else{
                forCopy.write(row, cols[2], pivpline.oboz);
            }
            if(pivpline.post.length() > 35){
                Format font_bold;
                font_bold.setFontBold(false);
                font_bold.setTextWrap(true);
                font_bold.setHorizontalAlignment(Format::AlignHCenter);
                font_bold.setVerticalAlignment(Format::AlignVCenter);
                QFont gostFont("GOST type B", 10);
                gostFont.setBold(false);
                gostFont.setUnderline(false);
                font_bold.setFont(gostFont);
                forCopy.write(row, cols[3], pivpline.post, font_bold);
            }
            else{
                forCopy.write(row, cols[3], pivpline.post);
            }
            forCopy.write(lastRow + 2,4, QString::number((currentPage-1) * rowCount + lastRow));
            forCopy.write(lastRow + 3,4, QString::number((currentPage-1) * rowCount + lastRow + 1));
            forCopy.write(row,4, QString::number((currentPage-1) * rowCount + row - 2));
            forCopy.write(row, cols[4], pivpline.vhodit);
            forCopy.write(row, cols[5], pivpline.num);
            forCopy.write(row, cols[6], pivpline.num);
            forCopy.write(row, cols[7], pivpline.comment);

            ++row;
        }

    }
    forCopy.selectSheet("Лист регистрации изменений");
    forCopy.write(37, 19, pageCount + 1);
    forCopy.write(35, 12, filemane + "ВП");

    // Найти индекс листа с определенным именем
    int index = forCopy.sheetNames().indexOf("Лист регистрации изменений");

    // Если лист найден, переместить его в конец
    if (index != -1) {
        forCopy.moveSheet("Лист регистрации изменений", forCopy.sheetNames().size() - 1);
    }


    forCopy.save();
}

void MainWindow::excelPost(QString filemane, QString format)
{
    QDir dir;
    QString rootPath = dir.path();
    // Путь к исполняемому файлу
    QString program;

    if(format == "A3"){
        program = rootPath + "/a3PageMargin/dist/excelPageMargin.exe"; // Путь к исполняемому файлу
    }
    else if(format == "A4"){
        program = rootPath + "/a4PageMargin/dist/excelPageMarginA4.exe"; // Путь к исполняемому файлу
    }
    // Параметры для передачи исполняемому файлу
    QStringList arguments;
    arguments << filemane; // Путь к вашему файлу Excel

    // Создаем экземпляр QProcess
    QProcess process;

    // Запускаем процесс с параметрами
    process.start(program, arguments);

    // Проверяем, запустился ли процесс успешно
    if (!process.waitForStarted()) {
        qDebug() << "Cannot start excelPageMargin.exe";
        return;
    }

    qDebug() << "excelPageMargin.exe started successfully";

    // Ждем завершения процесса
    process.waitForFinished();

    // Получаем результат выполнения
    QByteArray res = process.readAllStandardOutput();
    qDebug() << "Result:" << res;

}

void MainWindow::setupStampData()
{
    // Получаем виджет второй вкладки
    QWidget *tab2Widget = ui->tabWidget->widget(1); // Вторая вкладка имеет индекс 1 (индексация начинается с 0)

    // Находим все комбо боксы в виджете второй вкладки
    QList<QComboBox*> comboBoxes = tab2Widget->findChildren<QComboBox*>();

    // Очищаем найденные комбо боксы
    for (QComboBox *comboBox : comboBoxes) {
        comboBox->clear(); // Очистка комбо бокса
    }

    //парсим данные по разрабам
    razrabs.insert(stampPE["Разработал"], projectOboz);
    razrabs.insert(stampSP["Разработал"], projectOboz);
    razrabs.insert(stampVP["Разработал"], projectOboz);

    //парсим данные по проверяющим
    checks.insert(stampPE["Проверил"], projectOboz);
    checks.insert(stampSP["Проверил"], projectOboz);
    checks.insert(stampVP["Проверил"], projectOboz);

    //парсим данные по утверждающих
    utvs.insert(stampPE["Утвердил"], projectOboz);
    utvs.insert(stampSP["Утвердил"], projectOboz);
    utvs.insert(stampVP["Утвердил"], projectOboz);

    //парсим данные по нормоконтролеров
    norms.insert(stampPE["Нормоконтроль"], projectOboz);
    norms.insert(stampSP["Нормоконтроль"], projectOboz);
    norms.insert(stampVP["Нормоконтроль"], projectOboz);

    //парсим данные по начальникам
    nachalnicks.insert(stampPE["Начальник отдела"], projectOboz);
    nachalnicks.insert(stampSP["Начальник отдела"], projectOboz);
    nachalnicks.insert(stampVP["Начальник отдела"], projectOboz);

    //парсим данные по начальникам
    nachalnickAlts.insert(stampPE["Начальник отдела ( альтернативное название параметра)"], projectOboz);
    nachalnickAlts.insert(stampSP["Начальник отдела ( альтернативное название параметра)"], projectOboz);
    nachalnickAlts.insert(stampVP["Начальник отдела ( альтернативное название параметра)"], projectOboz);

    for(auto it = razrabs.begin(); it != razrabs.end(); it++){
        ui->razrabCB->addItem(it.key());
    }

    for(auto it = checks.begin(); it != checks.end(); it++){
        ui->checkCB->addItem(it.key());
    }

    for(auto it = utvs.begin(); it != utvs.end(); it++){
        ui->utvCB->addItem(it.key());
    }

    for(auto it = norms.begin(); it != norms.end(); it++){
        ui->normCB->addItem(it.key());
    }

    for(auto it = nachalnicks.begin(); it != nachalnicks.end(); it++){
        ui->nachalnicCB->addItem(it.key());
    }

    for(auto it = nachalnickAlts.begin(); it != nachalnickAlts.end(); it++){
        ui->nachalnicAltCB->addItem(it.key());
    }
}

void MainWindow::reloadStamp()
{
    if(!ui->razrabLine_2->text().isEmpty()){
        stampPE["Разработал"] = ui->razrabLine_2->text();
        stampVP["Разработал"] = ui->razrabLine_2->text();
        stampSP["Разработал"] = ui->razrabLine_2->text();
    }
    if(!ui->provLine_2->text().isEmpty()){
        stampPE["Проверил"] = ui->provLine_2->text();
        stampVP["Проверил"] = ui->provLine_2->text();
        stampSP["Проверил"] = ui->provLine_2->text();
    }
    if(!ui->utvLine_2->text().isEmpty()){
        stampPE["Утвердил"] = ui->utvLine_2->text();
        stampVP["Утвердил"] = ui->utvLine_2->text();
        stampSP["Утвердил"] = ui->utvLine_2->text();
    }
    if(!ui->normLine_2->text().isEmpty()){
        stampPE["Нормоконтроль"] = ui->normLine_2->text();
        stampVP["Нормоконтроль"] = ui->normLine_2->text();
        stampSP["Нормоконтроль"] = ui->normLine_2->text();
    }
    if(!ui->nachalnicLine_2->text().isEmpty()){
        stampPE["Начальник отдела"] = ui->nachalnicLine_2->text();
        stampVP["Начальник отдела"] = ui->nachalnicLine_2->text();
        stampSP["Начальник отдела"] = ui->nachalnicLine_2->text();
    }
    if(!ui->nachalnicAltLine_2->text().isEmpty()){
        stampPE["Начальник отдела ( альтернативное название параметра)"] = ui->nachalnicAltLine_2->text();
        stampVP["Начальник отдела ( альтернативное название параметра)"] = ui->nachalnicAltLine_2->text();
        stampSP["Начальник отдела ( альтернативное название параметра)"] = ui->nachalnicAltLine_2->text();
    }
}

QString projectObozn(const QString &filePath){
//    QString projectObozn;

    QFile file(filePath);
    if (!file.open(QFile::ReadOnly | QFile::Text)) {
        qDebug() << "Ошибка открытия файла!";
        /*return;*/
    }

    QXmlStreamReader reader(&file);
    while (!reader.atEnd() && !reader.hasError()){
        QXmlStreamReader::TokenType token = reader.readNext();
        if (token == QXmlStreamReader::StartElement){
            if (reader.name() == "specline"){
                pispecline.num = reader.attributes().value("num").toString();
                pispecline.oboz = reader.attributes().value("oboz").toString();
                pispecline.name = reader.attributes().value("name").toString();
                pispecline.comment = reader.attributes().value("comment").toString();
                pispecline.underline = (reader.attributes().value("underline").toString() == "True");
                pispecline.type = reader.attributes().value("type").toString();
                pispecline.merge = (reader.attributes().value("merge").toString() == "True");
                pispecline.needtu = (reader.attributes().value("needtu").toString() == "True");
                pispecline.GroupLine1 = reader.attributes().value("GroupLine1").toString();
                pispecline.GroupLine2 = reader.attributes().value("GroupLine2").toString();
                pispecline.format = reader.attributes().value("format").toString();
                pispecline.pos = reader.attributes().value("pos").toString();
                pispecline.posinelement = reader.attributes().value("posinelement").toString();
                pispeclines.append(pispecline);
                if (pispecline.name == "Перечень элементов"){
                    projectOboz = pispecline.oboz.left(pispecline.oboz.length() - 3);
                }
            }
        }
    }

    pispeclines.clear();
    file.close();

    return projectOboz;
}

QMap<QString, QList<vpline>> readAndProcessXMLFiles(const QStringList& xmlFiles) {
    QMap<QString, QList<vpline>> categoriesMap; // Категории элементов

    foreach (const QString& fileName, xmlFiles) {
        QFile file(fileName);
        if (!file.open(QFile::ReadOnly | QFile::Text)) {
            qDebug() << "Не удалось открыть файл" << fileName;
            continue;
        }

        QXmlStreamReader reader(&file);
        QString currentGroup; // Текущая группа элементов

        while (!reader.atEnd() && !reader.hasError()) {
            QXmlStreamReader::TokenType token = reader.readNext();
            if (token == QXmlStreamReader::StartElement) {
                if (reader.name() == "vpline") {
                    QString elementType = reader.attributes().value("type").toString();
                    if (elementType == "ElementGroup") {
                        currentGroup = reader.attributes().value("name").toString();
                    } else {
                        vpline element;
                        element.num = reader.attributes().value("num").toString();
                        element.oboz = reader.attributes().value("oboz").toString();
                        element.kod = reader.attributes().value("kod").toString();
                        element.name = reader.attributes().value("name").toString();
                        element.comment = reader.attributes().value("comment").toString();
                        element.underline = (reader.attributes().value("underline").toString() == "True");
                        element.type = reader.attributes().value("type").toString();
                        element.merge = (reader.attributes().value("merge").toString() == "True");
                        element.vhodit = reader.attributes().value("vhodit").toString();
                        element.post = reader.attributes().value("post").toString();
                        if(element.vhodit.isEmpty()){
                            QString oboz = projectObozn(fileName);
                            qDebug() << "filename" << fileName;
                            element.vhodit = oboz;
                        }
                        qDebug() << element.vhodit;
                        categoriesMap[currentGroup].append(element);
                    }
                }
            }
        }

        if (reader.hasError()) {
            qDebug() << "Ошибка разбора XML:" << reader.errorString();
        }

        file.close();
    }

    return categoriesMap;
}

void addVpline(QList<vpline>& pivplines, const QString& num, const QString& name, const QString& type) {
    vpline newVpline;
    newVpline.num = num;
    newVpline.oboz = "";
    newVpline.name = name;
    newVpline.comment = "";
    newVpline.underline = false;
    newVpline.type = type;
    newVpline.merge = false;
    newVpline.vhodit = "";
    newVpline.post = "";

    pivplines.append(newVpline);
}

void MainWindow::parseXML(const QString &filePath)
{
    parseStamp(filePath);
//    qDebug() << "filename" << filePath;
    parseElementLines(filePath);    
}

void MainWindow::parseStamp(const QString &filePath)
{
    QFile file(filePath);
    if (!file.open(QFile::ReadOnly | QFile::Text)) {
        qDebug() << "Ошибка открытия файла!";
        return;
    }

    QXmlStreamReader xmlReader(&file);
    QString currentStampType;
    while (!xmlReader.atEnd() && !xmlReader.hasError()) {
        QXmlStreamReader::TokenType token = xmlReader.readNext();
        if (token == QXmlStreamReader::StartElement) {
            if (xmlReader.name() == "Stamp") {
                currentStampType = xmlReader.attributes().value("document").toString();
            } else if (xmlReader.name() == "param") {
                QString paramName = xmlReader.attributes().value("name").toString();
                QString paramValue = xmlReader.attributes().value("value").toString();
                if (currentStampType == "VP") {
                    stampVP[paramName] = paramValue;
                } else if (currentStampType == "PE") {
                    stampPE[paramName] = paramValue;
                } else if (currentStampType == "SP") {
                    stampSP[paramName] = paramValue;
                }
            }
        }
    }
    if (xmlReader.hasError()) {
        qDebug() << "Ошибка парсинга XML:" << xmlReader.errorString();
    }

    file.close();
}



void MainWindow::parseElementLines(const QString &filePath)
{
    QFile file(filePath);
    if (!file.open(QFile::ReadOnly | QFile::Text)) {
        qDebug() << "Ошибка открытия файла!";
        return;
    }

    QXmlStreamReader reader(&file);
    while (!reader.atEnd() && !reader.hasError()) {
        QXmlStreamReader::TokenType token = reader.readNext();
        if (token == QXmlStreamReader::StartElement) {
            if (reader.name() == "peline") {
                pipeline.num = reader.attributes().value("num").toString();
                pipeline.oboz = reader.attributes().value("oboz").toString();
                pipeline.name = reader.attributes().value("name").toString();
                pipeline.comment = reader.attributes().value("comment").toString();
                pipeline.underline = (reader.attributes().value("underline").toString() == "True");
                pipeline.type = reader.attributes().value("type").toString();
                pipelines.append(pipeline);
            }
            if (reader.name() == "vpline"){
                pivpline.num = reader.attributes().value("num").toString();
                pivpline.oboz = reader.attributes().value("oboz").toString();
                pivpline.name = reader.attributes().value("name").toString();
                pivpline.comment = reader.attributes().value("comment").toString();
                pivpline.underline = (reader.attributes().value("underline").toString() == "True");
                pivpline.type = reader.attributes().value("type").toString();
                pivpline.merge = (reader.attributes().value("merge").toString() == "True");
                pivpline.vhodit = reader.attributes().value("vhodit").toString();
                pivpline.post = reader.attributes().value("post").toString();
                pivplines.append(pivpline);
            }
            if (reader.name() == "specline") {
                pispecline.num = reader.attributes().value("num").toString();
                pispecline.oboz = reader.attributes().value("oboz").toString();
                pispecline.name = reader.attributes().value("name").toString();
                pispecline.comment = reader.attributes().value("comment").toString();
                pispecline.underline = (reader.attributes().value("underline").toString() == "True");
                pispecline.type = reader.attributes().value("type").toString();
                pispecline.merge = (reader.attributes().value("merge").toString() == "True");
                pispecline.needtu = (reader.attributes().value("needtu").toString() == "True");
                pispecline.GroupLine1 = reader.attributes().value("GroupLine1").toString();
                pispecline.GroupLine2 = reader.attributes().value("GroupLine2").toString();
                pispecline.format = reader.attributes().value("format").toString();
                pispecline.pos = reader.attributes().value("pos").toString();
                pispecline.posinelement = reader.attributes().value("posinelement").toString();
                pispeclines.append(pispecline);
                if (pispecline.name == "Перечень элементов"){
                    projectOboz = pispecline.oboz.left(pispecline.oboz.length() - 3);
                }
            }
            /*for (int i = 0; i < pivplines.size(); i++) {
                const vpline& elementC = pivplines[i];
                vpline newVpline;
                if(!elementC.name.isEmpty() && !(elementC.type == "ElementGroup")){
                    if(elementC.vhodit.isEmpty()){
                        newVpline = elementC;
                        newVpline.vhodit = projectOboz;
                    }
                }
            }*/
        }
    }

    // Максимальное количество элементов на первой странице
    const int firstPageMax = 23;
    // Максимальное количество элементов на последующих страницах
    const int otherPagesMax = 29;

    // Разделение списка pipelines на страницы
    Page currentPagePE, currentPageVP, currentPageSP;
    for (int i = 0; i < pipelines.size(); ++i) {
        if ((i == firstPageMax && currentPagePE.pipelines.size() == firstPageMax) ||
            (i > firstPageMax && (i - firstPageMax) % otherPagesMax == 0)) {
            containerPE.pages.append(currentPagePE);
            currentPagePE = Page();
        }
        currentPagePE.pipelines.append(pipelines.at(i));
    }
    if (!currentPagePE.pipelines.isEmpty()) {
        containerPE.pages.append(currentPagePE);
    }

    // Разделение списка pivplines на страницы
    for (int i = 0; i < pivplines.size(); ++i) {
        if ((i == firstPageMax - 1 && currentPageVP.pivplines.size() == firstPageMax - 1) ||
            (i > firstPageMax - 1 && (i - firstPageMax - 1) % otherPagesMax + 1 == 0)) {
            containerVP.pages.append(currentPageVP);
            currentPageVP = Page();
        }
        currentPageVP.pivplines.append(pivplines.at(i));
    }
    if (!currentPageVP.pivplines.isEmpty()) {
        containerVP.pages.append(currentPageVP);
    }

    // Разделение списка pispeclines на страницы
    for (int i = 0; i < pispeclines.size(); ++i) {
        if ((i == firstPageMax && currentPageSP.pispeclines.size() == firstPageMax) ||
            (i > firstPageMax && (i - firstPageMax) % otherPagesMax == 0)) {
            containerSP.pages.append(currentPageSP);
            currentPageSP = Page();
        }
        currentPageSP.pispeclines.append(pispeclines.at(i));
    }
    if (!currentPageSP.pispeclines.isEmpty()) {
        containerSP.pages.append(currentPageSP);
    }

    // Проверяем, есть ли страницы в контейнере
    if (!containerSP.pages.isEmpty()) {
        Page &lastPage = containerSP.pages.last();
        if (lastPage.pispeclines.size() >= 6) {
            int startIndex = lastPage.pispeclines.size() - 6;
            auto iter = lastPage.pispeclines.begin() + startIndex;
            lastPage.pispeclines.erase(iter, iter + 6);
        } else {
            lastPage.pispeclines.clear();
        }
    }    

    if (reader.hasError()) {
        qDebug() << "Ошибка парсинга XML:" << reader.errorString();
    }

    file.close();
}

//Обработчик нажатия кнопки для формирования КД
void MainWindow::on_pushButton_clicked()
{
//    inDir = QFileDialog::getOpenFileName(0,"Выберите исходный файл для создания ВП","","*.xlsx");
//     delete ui;
    int fileCount = fileFinals.size();
    if(fileCount < 2){
        containersClear();
        parseXML(fileFinals[0]);
        setupStampData();
        reloadStamp();
        // Создаем объект QFileInfo, используя путь к файлу
        QFileInfo fileInfo(fileFinals[0]);
        // Извлекаем директорию из пути к файлу
        QString directoryPath = fileInfo.absolutePath();
        // Выводим директорию на консоль (или выполняем с ней другие действия)
        qDebug() << "Директория файла:" << directoryPath;
        QDir directory(directoryPath);
        directory.mkdir("Документация");
        createPE(projectOboz, directoryPath);
        createSP(projectOboz, directoryPath);
        createVP(projectOboz, directoryPath);
        excelPost(directoryPath + "/Документация/Ведомость покупных " + projectOboz+"ВП.xlsx", "A3");
        excelPost(directoryPath + "/Документация/Спецификация " + projectOboz+".xlsx", "A4");
        excelPost(directoryPath + "/Документация/Перечень элементов " + projectOboz+"ПЭ3.xlsx", "A4");
        containersClear();
    } else{
        for(int i = 0; i < fileCount; i++){
            containersClear();
            parseXML(fileFinals[i]);
            setupStampData();
            reloadStamp();
            // Создаем объект QFileInfo, используя путь к файлу
            QFileInfo fileInfo(fileFinals[i]);
            // Извлекаем директорию из пути к файлу
            QString directoryPath = fileInfo.absolutePath();
            // Выводим директорию на консоль (или выполняем с ней другие действия)
            qDebug() << "Директория файла:" << directoryPath;
            QDir directory(directoryPath);
            directory.mkdir("Документация");
            createPE(projectOboz, directoryPath);
            createSP(projectOboz, directoryPath);
            createVP(projectOboz, directoryPath);
            excelPost(directoryPath + "/Документация/Ведомость покупных " + projectOboz+"ВП.xlsx", "A3");
            excelPost(directoryPath + "/Документация/Спецификация " + projectOboz+".xlsx", "A4");
            excelPost(directoryPath + "/Документация/Перечень элементов " + projectOboz+"ПЭ3.xlsx", "A4");
            containersClear();
        }
    }
}

void MainWindow::on_pushButton_2_clicked()
{
    /*outDir = QFileDialog::getExistingDirectory(0," Выберите папку, в которую хотите сохранить ВП","");
//    qDebug()<<outDir;
    outFileName = ui->vedomostName->text();
    //извлечение обозначений из файла
    QFile file("oboz.csv");
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        return;
    QTextStream in(&file);
    bool found = false;
    QStringList categories;
    while (!in.atEnd()) {
        QString line = in.readLine();
        if (line.contains("//Категории")) {
            found = true;
            continue;
        }
        if (found) {
            categories.append(line);
        }
    }
//    qDebug()<<categories;
    file.close();

    //создание копии файла
    QString sourceFilePath = "Vedomost.xlsx";
    QString destinationFilePath = outDir+"/"+"Ведомость покупных "+outFileName+".xlsx";
    QFile sourceFile(sourceFilePath);
    if (!sourceFile.exists()) {
        qDebug() << "Файл не найден";
//          return 1;
    }
    QFileInfo destinationFileInfo(destinationFilePath);
    if (destinationFileInfo.exists()) {
        qDebug() << "Файл уже существует";
//          return 1;
    }
    if (!sourceFile.copy(destinationFilePath)) {
        qDebug() << "Не удалось скопировать файл";
//          return 1;
    }
    //разделление извлеченных категорий по спискам для дальнейшего использования
    QStringList categoryDesignator;
    QStringList categoryNames;
    QStringList categoryName;
    for(auto i : categories){
        QStringList str = i.split(":");
        categoryDesignator.append(str[0]);
        categoryNames.append(str[1]);
        categoryName.append(str[2]);
    }

    //читаем и обрабатываем файл из альтиума
    QMap<QString, int> componentCategories;
    QXlsx::Document xlsx(inDir);//testo
    for (int row = 1; row <= xlsx.dimension().rowCount(); ++row) {
        QString str = xlsx.read(row, 1).toString();
        if (str.length() >= 1 && str.at(0).isLetter()) {
            QString category;
            if (str.length() >= 2 && str.at(1).isLetter()) {
                category = str.left(2);
            } else {
                category = str.left(1);
            }
            if (componentCategories.contains(category)) {
                componentCategories[category]++;
            } else {
                componentCategories.insert(category, 1);
            }
        }
    }

//    qDebug()<<componentCategories;

    //определение много компонентов в группе или нет, *возможно можно и удалить*
    QStringList totalComponents;
    QStringList  result;
    for (auto it = componentCategories.begin(); it != componentCategories.end(); ++it) {
        QString key = it.key();
        int value = it.value();
        int index = categoryDesignator.indexOf(key);
        if (index != -1) {
            QString name;
            QString kolvo;
            if (value == 1) {
                name = categoryName[index];
                kolvo = " строка";
            } else {
                name = categoryNames[index];
                kolvo = " строк";
            }
            result.append(name + " " + QString::number(value) + kolvo);
        }
    }
//    qDebug()<<result;

    // Создаем QMap для хранения данных
    //алгоритм разбора полученных ранее данных с целью оформления списка для ВП
    QMap<QString, QMap<QString, QStringList>> components;
    for (int row = 2; row <= xlsx.dimension().lastRow(); row++) {
        // Получаем значения ячеек
        QString designator = xlsx.read(row, 1).toString();
        QString partNumber = xlsx.read(row, 2).toString();
        QString standartDoc = xlsx.read(row, 3).toString();
        QString whereUsed = xlsx.read(row, 5).toString();
        QString manufacturer = xlsx.read(row, 4).toString();

        // Используем регулярное выражение для получения буквенного обозначения
        QRegularExpression re("([A-Z]+)");
        QRegularExpressionMatch match = re.match(designator);
        QString letter = match.captured(1);
        // Добавляем данные в QMap
        components[letter][whereUsed].append(partNumber+";"+standartDoc+";"+manufacturer);
    }
    // Выводим данные
    xlsx.save();
    xlsx.deleteLater();
      int totalComponents1 = 0;
      int row = 1;
      QXlsx::Document xlsxW;
      QMapIterator<QString, QMap<QString, QStringList>> i(components);
      while (i.hasNext()) {
          i.next();
          QString letter = i.key();
          QMap<QString, QStringList> whereUsed = i.value();
          int numComponents = 0;
          QMapIterator<QString, QStringList> j(whereUsed);
          while (j.hasNext()) {
              j.next();
              QString where = j.key();
              QStringList parts = j.value();
              numComponents += parts.size();
          }
          totalComponents1 += numComponents;
//        int value = i.value();
          QString name;
          int index = categoryDesignator.indexOf(letter);
          if (index != -1) {
              QString kolvo;
              if (numComponents == 1) {
                  name = categoryName[index];
                  kolvo = " строка";
              } else {
                  name = categoryNames[index];
                  kolvo = " строк";
              }
          }
//        qDebug() << "Категория" << name << "содержит" << numComponents << "компонентов:";
          QVariant srt = QString(name);
          xlsxW.write(row, 1, srt);
                  row +=2;
          QMapIterator<QString, QStringList> k(whereUsed);
          while (k.hasNext()) {

              k.next();
              QString where = k.key();
              QStringList parts = k.value();
              QVariant writeValue = QString(where);
//          qDebug()<<where<<"where"<<row;
//          xlsxW.write(row, 1, writeValue);
//                      row+=2;xlsxW.write(row, 1, writeValue);
//          qDebug() << "Группа компонентов" << where << "содержит" << parts.size() << "компонентов:";
              QMap<QString, int> word_count;
              for (const QString& word : parts) {
                  word_count.insert(word, word_count.value(word, 0) + 1);
              }

              for (auto it = word_count.begin(); it != word_count.end(); ++it) {
                  xlsxW.write(row,1,it.key());
                  xlsxW.write(row, 3, it.value());
                  xlsxW.write(row, 2, writeValue);
                  ++row;
//              qDebug() << it.key() << ": " << it.value();
              }
//          ++row;
            }
            ++row;
            xlsxW.saveAs("tstOut.xlsx");
      }

      QString sourceFilePath1 = "tstOut.xlsx";
      QString destinationFilePath1 = "tstOut1.xlsx";
      QFile sourceFile1(sourceFilePath1);
      if (!sourceFile1.exists()) {
          qDebug() << "Файл не найден";
  //          return 1;
      }
      QFileInfo destinationFileInfo1(destinationFilePath1);
      if (destinationFileInfo1.exists()) {
          qDebug() << "Файл уже существует";
  //          return 1;
      }
      if (!sourceFile1.copy(destinationFilePath1)) {
          qDebug() << "Не удалось скопировать файл";
  //          return 1;
      }

      int rowCount =xlsxW.dimension().rowCount();
      int emptyRowCount = 0;
      for (int i = 1; i <= rowCount; ++i) {
          bool isEmpty = true;
          for (int j = 1; j <= xlsxW.dimension().columnCount(); ++j) {
              if (!xlsxW.read(i, j).isNull()) {
                  isEmpty = false;
                  break;
              }
          }
          if (isEmpty) {
              ++emptyRowCount;
          }
      }

      int numberSheets = 0;
      if(((xlsxW.dimension().lastRow()-21)%27)>0.5){
            numberSheets = qCeil(((xlsxW.dimension().lastRow()-21)/27))+1;
        }
      else {
          numberSheets = qCeil(((xlsxW.dimension().lastRow()-21)/27)+0.5)+1;
      }

//      qDebug() << "Всего листов:"<<numberSheets;

      //читаем файл и выписываем номера строк с названиями групп
      QMap<int,QString> categoryMap;
      for (int row = 1; row <= xlsxW.dimension().rowCount(); ++row) {
          QString str = xlsxW.read(row, 1).toString();
          QString str2 = xlsxW.read(row, 2).toString();
          if(!str.isEmpty()&&str2.isEmpty()) categoryMap.insert(row,str);
      }
      //сортировка списка
        QMap<int, QString> uniqueCategoryMap;
        for (auto it = categoryMap.begin(); it != categoryMap.end(); ++it) {
            if (!uniqueCategoryMap.values().contains(it.value())) {
                uniqueCategoryMap.insert(it.key(), it.value());
            }
        }

    QXlsx::Document sortedWrite;
    int strCounter = 1;
    int currentIndex;
    int nextIndex;
      for (auto it = uniqueCategoryMap.begin(); it != uniqueCategoryMap.end(); it++) {
    //          qDebug() << it.key() << ":" << it.value();
          if (it == std::prev(uniqueCategoryMap.end())) {
                  // последний элемент
    //              qDebug()<<"end";
              currentIndex = it.key()+1;
              nextIndex = currentIndex+(xlsxW.dimension().rowCount() - currentIndex)+2;
    //              qDebug()<<"currentIndex"<<currentIndex<<"nextIndex"<<nextIndex;
              }
          else{
              currentIndex = it.key()+1;
              nextIndex = (it+1).key();
          }
          QStringList columsData;
          for (int row = currentIndex; row < nextIndex; ++row) {
              QString column2 = xlsxW.read(row, 2).toString();
              QString column3 = xlsxW.read(row, 3).toString();
              if (!column2.isNull() && !column3.isNull()) {
                  columsData.append(xlsxW.read(row, 1).toString() + ":" + column2 + ":" + column3);
              }
          }
          columsData.sort();
          columsData.removeAll("::");
    //        qDebug()<<columsData;
          QStringList columName, columWhereUsed, columQuantity;
          for (int i = 0; i < columsData.size(); i++ ) {
              QStringList strList = columsData[i].split(":");
              columName.append(strList[0]);
              columWhereUsed.append(strList[1]);
              columQuantity.append(strList[2]);
          }
          for(int i = 0; i < columsData.size(); i++ ){
              int currentRow = currentIndex+i+1;
                  sortedWrite.write(it.key(),2,it.value());
    //                  sortedWrite.write(it.key(),8,currentRow);
                  QStringList splitedName = columName[i].split(";");
                  sortedWrite.write(currentRow,1,strCounter);
                  sortedWrite.write(currentRow,2,splitedName[0]);//имя
                  sortedWrite.write(currentRow,4,splitedName[1]); // ТУ
                  sortedWrite.write(currentRow,5,splitedName[2]); // Поставщик
                  sortedWrite.write(currentRow,6,columWhereUsed[i]); // Куда входит
                  sortedWrite.write(currentRow,7,columQuantity[i]); // количество
    //                  sortedWrite.write(currentRow,8,currentRow);
                  qDebug()<<splitedName[2]<<"размер"<<splitedName[2].size();
                  int manufactStrSize = splitedName[2].size();
//                  if (manufactStrSize > 25){
//                        QStringList res = splitString(splitedName[2]);
//                        qDebug()<<"splited str"<<res;
//                  }

                  strCounter++;
          }

//          qDebug()<<"work is done!"<<strCounterNew-1;
        sortedWrite.saveAs(outDir+"/"+outFileName+".xlsx");
    }
    //добавление пустых строк
    QXlsx::Document addEmptyStrings (outDir+"/"+outFileName+".xlsx");
    qDebug()<<"opened";
    QXlsx::Worksheet *worksheet2 = addEmptyStrings.currentWorksheet();
    addEmptyStrings.copySheet("Sheet1","Sheet2");
    qDebug()<<"sheet copyed";
    addEmptyStrings.selectSheet("Sheet2");
    int doubleRow;
    int match = 0;
    QXlsx::Worksheet *worksheet3 = addEmptyStrings.currentWorksheet();
    qDebug()<<"sheet copyed selected";
    for (int row = 2; row <= worksheet3->dimension().lastRow()+1; row++) {
        QString cellPerv = worksheet3->read(row-1,2).toString();
        QString cell = worksheet3->read(row,2).toString();
        QString cellNext = worksheet3->read(row+1,2).toString();
        if((cell==cellPerv)&&(cell!=cellNext)){
            match ++;
//            qDebug()<< match;
            doubleRow = row;
            for(int row1 = doubleRow+match; row1 <= worksheet2->dimension().lastRow(); row1++){
                for (int col = 1; col <= worksheet2->dimension().lastColumn(); col++){
                    worksheet2->write(row1,col,"");
                }
            }
            addEmptyStrings.save();
            for (int row1 = doubleRow; row1 <= worksheet3->dimension().lastRow(); row1++){
                for (int col = 1; col <= worksheet3->dimension().lastColumn(); col++)
                {
                    QXlsx::Cell *cell1 = worksheet3->cellAt(row1+1,col);
                    if(cell1){
                        worksheet2->write(row1+1+match,col,cell1->value());
                    }
                }
            }
        }
        else{
//            qDebug()<<row;
            continue;
        }
    }
    addEmptyStrings.save();

    //подсчет суммы повторяющихся компонентов
    addEmptyStrings.selectSheet("Sheet1");
    int summ = 0;
    for (int row = 2; row <= worksheet3->dimension().lastRow()+1; row++) {
        QString cellPerv = worksheet2->read(row-1,2).toString();
        QString cell = worksheet2->read(row,2).toString();
        QString cellNext = worksheet2->read(row+1,2).toString();
        if(cell==cellNext){
            int count = worksheet2->read(row,7).toInt();
            summ += count;
        }
        if((cell==cellPerv)&&(cell!=cellNext)){
//            qDebug()<<"match";
            int count = worksheet2->read(row,7).toInt();
            summ += count;
            worksheet2->write(row+1,7,summ);
            summ = 0;
        }
    }
    addEmptyStrings.save();
    //добавления номера строки для итогового файла
    int strCounterNew = 1;
    for (int row = 1; row <= addEmptyStrings.dimension().rowCount(); row++){
        if(!(addEmptyStrings.read(row,2).toString().isEmpty())){
            if(!(addEmptyStrings.read(row,1).toString().isEmpty())){
                addEmptyStrings.write(row,8,strCounterNew);
//                qDebug()<<row<<strCounterNew;
                QString str = addEmptyStrings.read(row,5).toString();
                if(str.size() > 25){
                    strCounterNew+=3;
                }
                else strCounterNew++;
            }
            else{
                if(row!=1)strCounterNew++;
                if((strCounterNew-21)%27==0){
                    strCounterNew++;
//                    qDebug()<<"match!"<<strCounterNew;
                    addEmptyStrings.write(row,8,strCounterNew);
                }
                addEmptyStrings.write(row,8,strCounterNew);
//                qDebug()<<row<<strCounterNew;
                strCounterNew+=2;
            }
        }
        if((!(addEmptyStrings.read(row,7).toString().isEmpty()))&&(addEmptyStrings.read(row,2).toString().isEmpty())){
            addEmptyStrings.write(row,8,strCounterNew);
//            qDebug()<<row<<strCounterNew<<"ggg";
            strCounterNew++;
        }
    }
    addEmptyStrings.save();

    qDebug()<<"start copying";
    QString destinationFP = outDir+"/"+"Для копирования в ВП "+outFileName+".xlsx";
    if (!sourceFile.copy(destinationFP)) {
        qDebug() << "Не удалось скопировать файл";
//          return 1;
    }
    QXlsx::Document forCopy (outDir+"/"+"Для копирования в ВП "+outFileName+".xlsx");
    QString sheetName = "text";
    forCopy.addSheet(sheetName);
    forCopy.selectSheet(sheetName);
    for (auto it = columnWidth.begin(); it != columnWidth.end(); ++it) {
        forCopy.setColumnWidth(it.key(), it.value());
    }
    forCopy.selectSheet("1");
    QXlsx::Worksheet *worksheetFC = forCopy.currentWorksheet();
    forCopy.selectSheet("2");
    QXlsx::Worksheet *worksheetFC1 = forCopy.currentWorksheet();
    forCopy.selectSheet(sheetName);
    QXlsx::Worksheet *worksheetFC2 = forCopy.currentWorksheet();
    int cnt = addEmptyStrings.read(addEmptyStrings.dimension().rowCount(),8).toInt();
    qDebug()<<cnt;
    for (int i = 1; i <= cnt ; ++i ) {
        forCopy.setRowHeight(i, 23.25);
        if(i > 21){
            forCopy.mergeCells("H" + QString::number(i) + ":K" + QString::number(i));
            forCopy.mergeCells("L" + QString::number(i) + ":O" + QString::number(i));
        }
        else{
            forCopy.mergeCells("H" + QString::number(i) + ":M" + QString::number(i));
            forCopy.mergeCells("N" + QString::number(i) + ":R" + QString::number(i));
            forCopy.mergeCells("U" + QString::number(i) + ":X" + QString::number(i));
            forCopy.mergeCells("Y" + QString::number(i) + ":AA" + QString::number(i));
            forCopy.mergeCells("AB" + QString::number(i) + ":AD" + QString::number(i));
        }

    }
    for (int row = 1; row <= cnt; ++row) {
        if (row > 21){
            for (int column = 1; column <= worksheetFC1->dimension().lastColumn(); ++column) {
                QXlsx::Cell *cell = worksheetFC1->cellAt(4, column);
    //            QXlsx::Cell *cell2 = worksheet3->cellAt(row, column);
                if (cell) {
                    worksheetFC2->write(row,column,cell->value(), cell->format());
                }
            }
        }
        else{
            qDebug()<<"row<21";
            for (int column = 1; column <= worksheetFC->dimension().lastColumn(); ++column) {
                QXlsx::Cell *cell = worksheetFC->cellAt(4, column);
    //            QXlsx::Cell *cell2 = worksheet3->cellAt(row, column);
                if (cell) {
                    worksheetFC2->write(row,column,cell->value(), cell->format());
                }
            }
        }

    }
    forCopy.selectSheet(sheetName);
//    xlsx.selectSheet(0);
    forCopy.save();
//    qDebug()<<"start copying writing";

    QMap<int,int> mappedSheets;
    mappedSheets.insert(1,21);
    for(int i = 2; i <= 20; ++i){
        mappedSheets.insert(i, mappedSheets.value(i-1)+27);
    }
    for (int row = 1; row <= cnt ; ++row ) {
        QString name, TU, manufacturer, whereUsed, quantity;
        int number, newStrNumber, sheetNumber;
        number = addEmptyStrings.read(row, 8).toInt();
        if(number != 0){
//            qDebug()<<number;
            name = addEmptyStrings.read(row, 2).toString();
            TU = addEmptyStrings.read(row, 4).toString();
            manufacturer = addEmptyStrings.read(row, 5).toString();
            whereUsed = addEmptyStrings.read(row, 6).toString();
            quantity = addEmptyStrings.read(row, 7).toString();            
//            zhszh
            if (number < 22){
                sheetNumber = 1;
                forCopy.write(number,5,name);
                forCopy.write(number,7,TU);
                forCopy.write(number,8,manufacturer);
                forCopy.write(number,14,whereUsed);
                forCopy.write(number,19,quantity);
                forCopy.write(number,25,quantity);
//                forCopy.write(number,31,sheetNumber);
            }
            else{
                forCopy.write(number,5,name);
                forCopy.write(number,7,TU);
                forCopy.write(number,8,manufacturer);
                forCopy.write(number,12,whereUsed);
                forCopy.write(number,16,quantity);
                forCopy.write(number,19,quantity);
                for (auto it = mappedSheets.begin(); it != mappedSheets.end(); ++it) {
                    if(number > it.value()){
                        sheetNumber = it.key()+1;
                        newStrNumber = (number-21)%27;
                    }
                    else if(number == it.value()){
                        sheetNumber = it.key();
                        newStrNumber = 27;
                    }
                }
//                forCopy.write(number,22,sheetNumber);
            }
        }
    }
    forCopy.save();

    QXlsx::Document xlsxX (outDir+"/"+outFileName+".xlsx");
    QString kategoryName ="";
    int numberSheets1 = 0;
    if(((xlsxX.dimension().lastRow()-21)%27)>0.5){
          numberSheets1 = qCeil(((xlsxX.read(xlsxX.dimension().lastRow(),8).toInt()-21)/27))+2;
      }
    else {
        numberSheets1 = qCeil(((xlsxX.read(xlsxX.dimension().lastRow(),8).toInt()-21)/27)+0.5)+2;
    }
    for (int i = 3; i <= numberSheets1; i++) {
        createSheetXlsxVP(outDir+"/"+"Ведомость покупных "+outFileName+".xlsx",QString::number(i));
    }

//    qDebug()<<"start merging";
    for (int sheet = 1; sheet <= numberSheets1 ; sheet++) {
        if(sheet == 1){
            forCopy.mergeCells("AE1:AE21");
            forCopy.write("AE1","1");
        }
        else if(sheet >= 1){
            forCopy.mergeCells("V"+QString::number(22+(sheet-2)*27)+":V"+QString::number((sheet)*27-6));
//            qDebug()<<"V"+QString::number(22+(sheet-2)*27)+":V"+QString::number((sheet)*27-6);
            forCopy.write("V"+QString::number(22+(sheet-2)*27),QString::number(sheet));
        }
    }
    forCopy.save();

    QStringList sheetNames = forCopy.sheetNames();
    for (const QString& name : sheetNames) {
        if (name != sheetName) {
            forCopy.deleteSheet(name);
        }
    }
    forCopy.save();

    qDebug()<<"work is done!";*/

    QFile fileCat("oboz.csv");
    if (!fileCat.open(QIODevice::ReadOnly | QIODevice::Text)){}
//        return;
    QTextStream in(&fileCat);
    bool found = false;
    QStringList categories;
    while (!in.atEnd()) {
        QString line = in.readLine();
        if (line.contains("//Категории")) {
            found = true;
            continue;
        }
        if (found) {
            categories.append(line);
        }
    }
//    qDebug()<<categories;
    fileCat.close();

    //разделление извлеченных категорий по спискам для дальнейшего использования
    QStringList categoryDesignator;
    QStringList categoryNames;
    QStringList categoryName;
    for(auto i : categories){
        QStringList str = i.split(":");
        categoryDesignator.append(str[0]);
        categoryNames.append(str[1]);
        categoryName.append(str[2]);
    }

    QStringList fileNames;

    qDebug() << ui->groupVPcheckBox->checkState();

    if(ui->groupVPcheckBox->checkState() == Qt::Checked){
        fileNames.append(fileFinals);
    }else if (ui->groupVPcheckBox->checkState() == Qt::Unchecked){
        for(auto it = fileMap.begin(); it != fileMap.end(); ++it){
            fileNames.append(it.value());
        }
    }

    QMap<QString, QList<vpline>> result = readAndProcessXMLFiles(fileNames);

    //извлечени двустрочных строк с поставщиками, именами или ТУ
    QList<vpline> temp;
    QMap<QString,QString> oboz2str, post2str;
    for (auto it = result.begin(); it != result.end(); ++it){
        const QList<vpline>& elements = it.value();
        temp.append(elements);
    }
    for(int i = 0; i < temp.size() - 2; i++){
        const vpline& elementC = temp[i];
        const vpline& elementN = temp[i + 1];
        const vpline& elementN1 = temp[i + 2];
        bool expr1 = elementN.name.isEmpty() && !elementN.post.isEmpty();
        bool expr2 = elementN.name.isEmpty() && !elementN.post.isEmpty() && elementN1.name.isEmpty() && !elementN1.post.isEmpty();
        bool expr1oboz = elementN.name.isEmpty() && !elementN.oboz.isEmpty();
        bool expr2oboz = elementN.name.isEmpty() && !elementN.oboz.isEmpty() && elementN1.name.isEmpty() && !elementN1.oboz.isEmpty();
        if(!elementC.name.isEmpty() && (expr1)){
            QString tmp = elementC.post + "" + elementN.post;
            if(post2str.contains(elementC.name)){
                QString currentValue = post2str[elementC.name]; // Получаем текущее значение для ключа
                if (tmp.length() > currentValue.length()) { // Сравниваем длину нового значения с текущим
                    post2str[elementC.name] = tmp; // Если новое значение длиннее, перезаписываем значение для ключа
                }
            }
            else{
                post2str.insert(elementC.name, tmp);
            }
//            qDebug() <<  tmp;
        }
        if(!elementC.name.isEmpty() && (expr2)){
            QString tmp = elementC.post + "" + elementN.post + "" + elementN1.post;
            if(post2str.contains(elementC.name)){
                QString currentValue = post2str[elementC.name]; // Получаем текущее значение для ключа
                if (tmp.length() > currentValue.length()) { // Сравниваем длину нового значения с текущим
                    post2str[elementC.name] = tmp; // Если новое значение длиннее, перезаписываем значение для ключа
                }
            }
            else{
                post2str.insert(elementC.name, tmp);
            }
//            qDebug() << "post3str" << tmp;
        }
        if(!elementC.name.isEmpty() && (expr1oboz)){
            QString tmp = elementC.oboz + " " + elementN.oboz;
            if(oboz2str.contains(elementC.name)){
                QString currentValue = oboz2str[elementC.name]; // Получаем текущее значение для ключа
                if (tmp.length() > currentValue.length()) { // Сравниваем длину нового значения с текущим
                    oboz2str[elementC.name] = tmp; // Если новое значение длиннее, перезаписываем значение для ключа
                }
            }
            else{
                oboz2str.insert(elementC.name, tmp);
            }
//            qDebug() <<  tmp;
        }
        if(!elementC.name.isEmpty() && (expr2oboz)){
            QString tmp = elementC.oboz + " " + elementN.oboz + ":" + elementN1.oboz;
            if(oboz2str.contains(elementC.name)){
                QString currentValue = oboz2str[elementC.name]; // Получаем текущее значение для ключа
                if (tmp.length() > currentValue.length()) { // Сравниваем длину нового значения с текущим
                    oboz2str[elementC.name] = tmp; // Если новое значение длиннее, перезаписываем значение для ключа
                }
            }
            else{
                oboz2str.insert(elementC.name, tmp);
            }
//            qDebug() <<  tmp;
        }
    }

    // Проход по всем категориям в result
    for (auto it = result.begin(); it != result.end(); ++it) {
        // Сортировка элементов в текущей категории по имени, а затем по параметру vhodit
        std::sort(it.value().begin(), it.value().end(), [](const vpline& a, const vpline& b) {
            if (a.name != b.name) {
                return a.name < b.name;
            } else {
                return a.vhodit < b.vhodit;
            }
        });

        // Удаление строк с пустыми значениями name, oboz и post
        it.value().erase(std::remove_if(it.value().begin(), it.value().end(), [](const vpline& elem) {
            return elem.name.isEmpty() || elem.oboz.isEmpty() || elem.post.isEmpty();
        }), it.value().end());

    }

    //Создание предварительного списка строк компонентов, отсортированного согласно ГОСТ
    QList<vpline> Npivplines;
    int summ = 0;
    for (int i = 0; i < categories.size(); i++) {
        // Вывод категорий и их содержимого
        for (auto it = result.constBegin(); it != result.constEnd(); ++it) {
            if(it.key() == categoryNames[i] ||it.key() == categoryName[i] ){
//                qDebug() << "Категория:" << it.key();
//                qDebug() << "Содержимое:";
                addVpline(Npivplines, "", it.key(), "ElementGroup");

                const QList<vpline>& elements = it.value();
                for (auto it = elements.begin(); it != elements.end(); ++it) {
                    const vpline& element = *it;
                    vpline newElement;
                    QString obozMatch, postMatch;
                    if(oboz2str.contains(element.name)){
                        obozMatch = oboz2str[element.name];
                    }
                    if(post2str.contains(element.name)){
//                        if(bigPostVP.contains(element.post)){
//                            postMatch = bigPostVP[element.post];
//                        }
//                        else{
                            postMatch = post2str[element.name];
//                        }
//                        qDebug() << "запись в общий списек" << element.name << obozMatch << postMatch;
                    }

                    if((!obozMatch.isEmpty()) || (!postMatch.isEmpty())){
                        newElement = element;
                        if((!obozMatch.isEmpty()) && (postMatch.isEmpty())){
                            newElement = element;
                            newElement.oboz = obozMatch;
//                            qDebug() << "newOboz" << newElement.name << newElement.oboz << obozMatch;
                        }
                        if((!postMatch.isEmpty()) && (obozMatch.isEmpty())){
//                            if(bigPostVP.contains(postMatch)){
//                                newElement = element;
//                                newElement.post = bigPostVP[postMatch];
////                                qDebug() << "newElement.post" << newElement.post;
//                            }
//                            else{
                                newElement = element;
                                newElement.post = postMatch;
//                            }

//                            qDebug() << "newOboz" << newElement.name << newElement.post << postMatch;
                        }
                        if ((!obozMatch.isEmpty()) && (!postMatch.isEmpty())){
                            newElement = element;
                            newElement.oboz = obozMatch;
                            newElement.post = postMatch;
                        }
//                        qDebug() << newElement.oboz << newElement.post << newElement.name;
                        Npivplines.append(newElement);
                    }
//                    else if(bigPostVP.contains(element.post)){
//                        newElement = element;
//                        newElement.post = bigPostVP[element.post];
//                        Npivplines.append(newElement);
//                    }
                    else{
                        Npivplines.append(element);
                    }
                }
                addVpline(Npivplines, "", "", "");
            }
        }
    }

    QMap<QString, int> postLenght;
    //подсчет суммы повторяющихся элементов
    pivplinesFin.append(Npivplines.at(0));
    for(int i = 1; i < Npivplines.size() - 1; i++){
        const vpline& elementC = Npivplines[i];
        const vpline& elementP = Npivplines[i - 1];
        const vpline& elementN = Npivplines[i + 1];

        if(elementC.name == elementN.name){
            pivplinesFin.append(Npivplines.at(i));
            int count = elementC.num.toInt();
            summ += count;
        }
        if((elementC.name != elementN.name) && ((elementC.name == elementP.name))){
            pivplinesFin.append(Npivplines.at(i));
            int count = elementC.num.toInt();
            summ += count;
            addVpline(pivplinesFin, QString::number(summ), "", "");
            summ = 0;
        }
        if(elementC.name.isEmpty() || elementC.type == "ElementGroup"){
            pivplinesFin.append(Npivplines.at(i));
        }

//        qDebug() << "  Номер:" << i
//                 << "  Обозначение:" << element.oboz
//                 << "  Имя:" << element.name
//                 << "  Комментарий:" << element.comment
//                 << "  Тип:" << element.type
//                 << "  Куда входит:" << element.vhodit;
    }

    Npivplines.clear();

    //не помню зачем, но зачем-то есть...
    for(int i = 0; i < pivplinesFin.size(); i++){
        const vpline& element = pivplinesFin[i];
//        qDebug() << "  Номер:" << element.num
//                 << "  Обозначение:" << element.oboz
//                 << "  Имя:" << element.name
//                 << "  Комментарий:" << element.comment
//                 << "  Тип:" << element.type
//                 << "  Куда входит:" << element.vhodit;
        if(element.post.length() > 25){
            postLenght.insert(element.post, element.post.size());
        }

        if(element.post.length() > 25){
            vpline newVpline;
            QStringList postS = element.post.split(":");
            for (int i = 0; i < postS.size(); i++) {
                if(i > 0){
                    newVpline.num = "";
                    newVpline.oboz = "";
                    newVpline.name = "";
                    newVpline.comment = "";
                    newVpline.underline = false;
                    newVpline.type = "";
                    newVpline.merge = false;
                    newVpline.vhodit = "";
                    newVpline.post = postS[i];
                    Npivplines.append(newVpline);
                }
                else{
                    newVpline = element;
                    newVpline.post = postS[i];
                    Npivplines.append(newVpline);
                }
            }
        }
        else{
            Npivplines.append(element);
        }
    }

    //разделение полученного списка на группы, содержащие определенное
    //количество строк для более удобной и правильной записи в файл
    pivplinesFin.clear();

    QMap<int, QList<vpline>> map;
    int componentCounter = 0;
    QString currentComponentName;
    QList<vpline> currentList, currentGroup;
    bool groupNameWritten = false;

    for(int i = 0; i < Npivplines.size(); i++) {
        const vpline& elementC = Npivplines[i];
//        const vpline& elementN = Npivplines[i+1];

        // Если текущий элемент - это ElementGroup, устанавливаем текущее имя компонента
        if(elementC.type == "ElementGroup") {
            currentList.append(elementC);
            currentComponentName = elementC.name;
            groupNameWritten = false;
//            if(!(elementC.name.isEmpty()) && !(elementC.post.isEmpty()))
//            componentCounter++;
        } else {
            // Если имя следующего элемента не совпадает с текущим и не пустое,
            // то это новый компонент
            if(/*!(currentComponentName == elementC.name) &&*/ !(elementC.name.isEmpty()) && !(elementC.post.isEmpty())) {
                // Добавляем текущий список элементов в QMap
                if(groupNameWritten){
                    map.insert(componentCounter, currentList);
                    componentCounter++;
                    currentList.clear();
                    // Создаем новый список элементов для нового компонента

                    currentList.append(elementC);

                    // Устанавливаем текущее имя компонента
                    currentComponentName = elementC.name;
                }else{
                    currentList.append(elementC);
                    groupNameWritten = true;
                }


            } else {
                // Продолжаем добавлять элементы к текущему списку
                if(elementC.name.isEmpty() && elementC.post.isEmpty() && elementC.num.isEmpty()){
                    currentList.append(elementC);
                    map.insert(componentCounter, currentList);
                    componentCounter++;
                    currentList.clear();
                    // Создаем новый список элементов для нового компонента

//                    currentList.append(elementC);

                    // Устанавливаем текущее имя компонента
                    currentComponentName = elementC.name;
                }else{
                    currentList.append(elementC);
                }
            }
        }
    }

    // Добавляем последний список элементов в QMap
    map.insert(componentCounter, currentList);

    //разделение получившегося словаря на страницы

    // Максимальное количество элементов на первой странице
    const int firstPageMax = 22;
    // Максимальное количество элементов на последующих страницах
    const int otherPagesMax = 28;

    Page currentPageVP;
    int currentPageCount = 0; // Счетчик текущего количества заполненных строк на странице
    int currentCountOfPages = 1;// счетчик номера страницы
    QList<vpline> componentLinesN;

    for (auto it = map.begin(); it != map.end(); ++it) {
//        int componentCounter = it.key();
        const QList<vpline>& componentLines = it.value();
        if(!componentLinesN.isEmpty()){
            for (int i = 0; i < componentLinesN.size(); ++i){
                const vpline& line = componentLinesN.at(i);
                currentPageVP.pivplines.append(line);
            }
            currentPageCount += componentLinesN.size();
            componentLinesN.clear();
        }
        // Разделение компонентов на страницы
        currentPageCount += componentLines.size();
        int currentPageCountMax = (currentCountOfPages == 1 ? firstPageMax : otherPagesMax);
        if(currentPageCount > currentPageCountMax){
            componentLinesN.append(componentLines);
            containerVPs.pages.append(currentPageVP);
            currentPageVP = Page();
//            for (int i = 0; i < componentLines.size(); ++i){
//                const vpline& line = componentLines.at(i);
//                currentPageVP.pivplines.append(line);
//            }
            currentCountOfPages++; // Увеличиваем счетчик текущей страницы
            currentPageCount = 0;
        }else{
            for (int i = 0; i < componentLines.size(); ++i){
                const vpline& line = componentLines.at(i);
                currentPageVP.pivplines.append(line);
            }
        }
    }

    // Добавляем последнюю страницу, если она не пуста
    if (!currentPageVP.pivplines.isEmpty()) {
        containerVPs.pages.append(currentPageVP);
    }

    //Создаем групповую ВП
    createVPs("testo");
    //Обрабатываем полученную ВП с целью упрощения дальнейшей печати
    excelPost("Ведомость покупных "+ui->obozVPs_2->text()+"ВП.xlsx", "A3");

    containersClear();
}

QString checkMatch(const QString& data, const QStringList& list) {
    QString out;
    foreach(const QString &str, list) {
        if (str.contains(data, Qt::CaseInsensitive)) {
            out = str;            
        }
    }
    return out;
}

void MainWindow::on_pushButton_3_clicked()
{
    /*qDebug()<<"перенос данных...";
    QXlsx::Document xlsx (outDir+"/"+outFileName+".xlsx");
    QString kategoryName ="";
    QMap<int,int> mappedSheets;
    mappedSheets.insert(1,21);
    for(int i = 2; i <= 20; ++i){
        mappedSheets.insert(i, mappedSheets.value(i-1)+27);
    }
    int numberSheets = 0;
    if(((xlsx.dimension().lastRow()-21)%27)>0.5){
          numberSheets = qCeil(((xlsx.dimension().lastRow()-21)/27))+2;
      }
    else {
        numberSheets = qCeil(((xlsx.dimension().lastRow()-21)/27)+0.5)+2;
    }
    for (int i = 3; i <= numberSheets; i++) {
        createSheetXlsxVP(outDir+"/"+"Ведомость покупных "+outFileName+".xlsx",QString::number(i));
    }
    QXlsx::Document xlsxW (outDir+"/"+"Ведомость покупных "+outFileName+".xlsx");
    for (int row = 1; row <= xlsx.dimension().rowCount(); ++row) {
        QString tmpStr2 = xlsx.read(row, 2).toString();
        QString tmpStr1 = xlsx.read(row, 1).toString();
        QString tmpStr7 = xlsx.read(row, 7).toString();
        if(!tmpStr2.isEmpty()){            
            if(!tmpStr1.isEmpty()){
                QString number, name, TU, manufacturer, whereUsed, quantity;
                number = xlsx.read(row, 1).toString();
                name = xlsx.read(row, 2).toString();
                TU = xlsx.read(row, 4).toString();
                manufacturer = xlsx.read(row, 5).toString();
                whereUsed = xlsx.read(row, 6).toString();
                quantity = xlsx.read(row, 7).toString();
                int sheetNumber, newStrNumber;
                int strNumber = xlsx.read(row, 8).toInt();
                if(strNumber > 0 && strNumber < 22){
                    sheetNumber = 1;
                    newStrNumber = strNumber;
//                    qDebug()<<strNumber<<sheetNumber<<newStrNumber<<kategoryName;
                    xlsxW.selectSheet(QString::number(sheetNumber));
//                    xlsxW.write(newStrNumber+3,4,number);
                    xlsxW.write(newStrNumber+3,5,name);
                    xlsxW.write(newStrNumber+3,7,TU);
                    xlsxW.write(newStrNumber+3,8,manufacturer);
                    xlsxW.write(newStrNumber+3,14,whereUsed);
                    xlsxW.write(newStrNumber+3,19,quantity);
                    xlsxW.write(newStrNumber+3,25,quantity);
                    xlsxW.save();
                }
                else {
                    for (auto it = mappedSheets.begin(); it != mappedSheets.end(); ++it) {
                        if(strNumber > it.value()){
                            sheetNumber = it.key()+1;
                            newStrNumber = (strNumber-21)%27;
                        }
                        else if(strNumber == it.value()){
                            sheetNumber = it.key();
                            newStrNumber = 27;
                        }
                    }
                    xlsxW.selectSheet(QString::number(sheetNumber));
//                    xlsxW.write(newStrNumber+3,4,number);
                    xlsxW.write(newStrNumber+3,5,name);
                    xlsxW.write(newStrNumber+3,7,TU);
                    xlsxW.write(newStrNumber+3,8,manufacturer);
                    xlsxW.write(newStrNumber+3,12,whereUsed);
                    xlsxW.write(newStrNumber+3,16,quantity);
                    xlsxW.write(newStrNumber+3,19,quantity);
                    xlsxW.save();
                }
            }            
            else {
                kategoryName = tmpStr2;
                int sheetNumber, newStrNumber;
                int strNumber = xlsx.read(row, 8).toInt();
                if(strNumber > 0 && strNumber < 22){
                    sheetNumber = 1;
                    newStrNumber = strNumber;
//                    qDebug()<<strNumber<<sheetNumber<<newStrNumber<<kategoryName;
                }
                else {
                    for (auto it = mappedSheets.begin(); it != mappedSheets.end(); ++it) {
                        if(strNumber > it.value()){
                            sheetNumber = it.key()+1;
                            newStrNumber = (strNumber-21)%27;
                        }
                        else if(strNumber == it.value()){
                            sheetNumber = it.key();
                            newStrNumber = 27;
                        }

                    }
                }
                xlsxW.selectSheet(QString::number(sheetNumber));
                xlsxW.write(newStrNumber+3,5,kategoryName);
                xlsxW.save();
            }
        }
//    qDebug()<<mappedSheets;
        if(!(tmpStr7.isEmpty())&&tmpStr2.isEmpty()){
            int sheetNumber, newStrNumber;
            int strNumber = xlsx.read(row, 8).toInt();
            if(strNumber > 0 && strNumber < 22){
                sheetNumber = 1;
                newStrNumber = strNumber;
//                qDebug()<<"ggg"<<strNumber<<sheetNumber;
                xlsxW.selectSheet(QString::number(sheetNumber));
                xlsxW.write(newStrNumber+3,19,tmpStr7);
                xlsxW.save();
            }
            else {
                for (auto it = mappedSheets.begin(); it != mappedSheets.end(); ++it) {
                    if(strNumber > it.value()){
                        sheetNumber = it.key()+1;
                        newStrNumber = (strNumber-21)%27;
                    }
                    else if(strNumber == it.value()){
                        sheetNumber = it.key();
                        newStrNumber = 27;
                    }

                }
                xlsxW.selectSheet(QString::number(sheetNumber));
                xlsxW.write(newStrNumber+3,16,tmpStr7);
                xlsxW.save();
            }

        }
    }
    qDebug()<<"work is done!";*/

    QStringList fileNames = QFileDialog::getOpenFileNames(nullptr, "Выберите XML файлы", "", "XML Files (*.xml)");
    for (int i = 0; i < fileNames.size(); i++) {
        qDebug() << fileNames[i];
    }

    if(fileNames.size() < 2){
        fileMap.insert(projectObozn(fileNames[0]),fileNames[0]);
    }
    else{
        for (int i = 0; i < fileNames.size(); i++){
            fileMap.insert(projectObozn(fileNames[i]),fileNames[i]);
        }
    }
    qDebug() << fileMap.size();
    if (!fileMap.isEmpty()) {
        auto it = fileMap.begin(); // Получаем итератор на первый элемент
        QString firstKey = it.key(); // Получаем ключ первого элемента
        QString firstValue = it.value(); // Получаем значение первого элемента

        qDebug() << "Первый элемент QMap: " << firstKey << "=" << firstValue;
    } else {
        qDebug() << "QMap пуст";
    }

    ui->boasList->clear();    

    /*int cnt = 0;
    QString currentPost, currentBigPost;
    for(int i = 0; i < Npivplines.size(); i++){
        const vpline& elementC = Npivplines[i];
        vpline newElement;

//        const vpline& elementN = Npivplines[i + 1];
        if(!elementC.post.isEmpty()){
            if(elementC.post.length() > 25){
                if(cnt == 0){
                    currentBigPost = elementC.post;
                    currentPost = elementC.post;
                    cnt++;
//                    qDebug() << cnt << currentBigPost << i << elementC.post;

                }
                else{
//                    qDebug() << cnt << "currentBigPost "+currentBigPost << i << elementC.post;
                    if(elementC.post == currentBigPost){
                        newElement = elementC;
                        newElement.post = "-\\\\-";
                        Npivplines[i] = newElement;
//                        qDebug() << "plused" << newElement.post;
                        cnt++;

                    }
                    if(elementC.post != currentBigPost){
//                        qDebug() << cnt << "currentIIPost" << i;
                        currentBigPost = elementC.post;
                        currentPost = elementC.post;
                        cnt = 1;
                    }
                }
            }
            else if (elementC.post != "-\\-"){
//                qDebug() << cnt << "currentII" << i;
                currentPost = elementC.post;
                currentBigPost.clear();
                cnt = 0;
            }
        }
    }*/

//    for (auto it = postLenght.constBegin(); it != postLenght.constEnd(); ++it){
//        qDebug() << it.key() << it.value();
//    }

//    for (auto it = map.constBegin(); it != map.constEnd(); ++it){
//        qDebug() << "Ключ:" << it.key();

//        // Выводим каждый элемент списка для текущего ключа
//        const QList<vpline>& currentList = it.value();
//        for (const vpline& element : currentList) {
//            qDebug() << "    " << element.name << element.num << element.post; // Предполагается, что у vpline определен оператор вывода <<
//        }
//    }
    // Разделение списка pivplines на страницы
//    for (int i = 0; i < pivplinesFin.size(); ++i) {
//        if ((i == firstPageMax && currentPageVP.pivplines.size() == firstPageMax) ||
//            (i > firstPageMax && (i - firstPageMax) % otherPagesMax == 0)) {
//            containerVPs.pages.append(currentPageVP);
//            currentPageVP = Page();
//        }
//        currentPageVP.pivplines.append(pivplinesFin.at(i));
//    }
//    if (!currentPageVP.pivplines.isEmpty()) {
//        containerVPs.pages.append(currentPageVP);
//    }
}

void searchFile(const QString &folderPath)
{
    QDir folder(folderPath);

    // Получаем список файлов и папок в текущей папке
    QStringList entries = folder.entryList(QDir::Dirs | QDir::NoDotAndDotDot | QDir::Files);

    // Проверяем каждый элемент списка
    foreach (const QString &entry, entries) {
        // Полный путь к текущему элементу
        QString entryPath = folder.filePath(entry);

        // Если текущий элемент - файл, проверяем его имя
        if (QFileInfo(entryPath).isFile() && entry == "excelPageMargin.exe") {
            qDebug() << "Файл excelPageMargin.exe найден в папке" << folderPath;
            qDebug() << "Полный путь к файлу:" << entryPath;
        }

        // Если текущий элемент - папка, рекурсивно вызываем функцию для нее
        if (QFileInfo(entryPath).isDir()) {
            searchFile(entryPath);
        }
    }
}

void MainWindow::on_choseXml_clicked()
{
    inDirXml = QFileDialog::getOpenFileName(0,"Выберите файл проекта BOAS","","*.xml");

    fileMap.insert(projectObozn(inDirXml),inDirXml);

    // Получаем компоновку (layout) из boasListGB
    QLayout *layout = ui->boasListGB->layout();

    // Проверяем, существует ли компоновка
    if (layout) {
        // Перебираем все дочерние виджеты в компоновке
        while (QLayoutItem *item = layout->takeAt(0)) {
            // Удаляем виджет из компоновки и освобождаем память
            delete item->widget();
            delete item;
        }
        delete layout;
    }

    // Удаляем layout из QGroupBox
//    ui->boasListGB->setLayout(nullptr);

    // Создаем вертикальную компоновку
    QGridLayout *gridLayout = new QGridLayout();

    QList<QCheckBox*> checkBoxList;
    QList<QLineEdit*> lineEditList;

    int projectsCount = fileMap.size();

    if(projectsCount < 2){
        auto it = fileMap.begin(); // Получаем итератор на первый элемент
        QString firstKey = it.key(); // Получаем ключ первого элемента
        QCheckBox *checkBox = new QCheckBox(firstKey, this); // Используем имя группы "boasListGB"
        QLineEdit *lineEdit = new QLineEdit(nullptr,this);
        lineEdit->setObjectName(firstKey);
        checkBox->setChecked(true);
        fileFinals.append(fileMap[checkBox->text()]);
        fileFinals.removeDuplicates();
        checkBoxList.append(checkBox);
        lineEditList.append(lineEdit);
    }
    else{
        for (auto it = fileMap.constBegin(); it != fileMap.constEnd(); ++it){
            QString firstKey = it.key(); // Получаем ключ первого элемента
            QCheckBox *checkBox = new QCheckBox(firstKey, this); // Используем имя группы "boasListGB"
            QLineEdit *lineEdit = new QLineEdit(nullptr,this);
            lineEdit->setObjectName(firstKey);
            checkBox->setChecked(true);
            fileFinals.append(fileMap[checkBox->text()]);
            fileFinals.removeDuplicates();
            checkBoxList.append(checkBox);
            lineEditList.append(lineEdit);
        }
    }

    // Создание и добавление кнопок для каждого графика на форму и соединение сигнала переключения с обработчиком
    for (int i = 0; i < checkBoxList.size(); i++) {
        connect(checkBoxList[i], &QCheckBox::stateChanged, this, [=](int checked) {
            if (checked == Qt::Checked) {
                fileFinals.append(fileMap[checkBoxList[i]->text()]);
                fileFinals.removeDuplicates();
            } else {
                int index = fileFinals.indexOf(fileMap[checkBoxList[i]->text()]);
                if (index != -1) {
                    fileFinals.removeAt(index);
                }
            }
        });
    }

    for (int i = 0; i < checkBoxList.size(); i++) {
        gridLayout->addWidget(checkBoxList[i], i, 0);
        gridLayout->addWidget(lineEditList[i], i, 1);
    }

    // Устанавливаем созданную вертикальную компоновку для QGroupBox
    ui->boasListGB->setLayout(gridLayout);

    parseXML(inDirXml);
    setupStampData();
//    createPE(projectOboz);
//    createSP(projectOboz);
//    createVP(projectOboz);
    qDebug() << projectOboz;
    containersClear();
}

void MainWindow::on_boasList_textChanged()
{
    QString newText = ui->boasList->toPlainText();
    QStringList changedTextList;
    QVector<int> indexes, indexes2; // Создаем вектор для хранения индексов символов '/'
//    ui->boasList->setAcceptDrops(false);
    if(!newText.isEmpty()){
        int index = newText.indexOf("file:///");
        if (index != -1) {
            indexes.append(index); // Добавляем индекс в вектор

            while (index != -1) {
                index = newText.indexOf("file:///", index + 1); // Ищем следующее вхождение символа '/' начиная с позиции index + 1
                if (index != -1) {
                    indexes.append(index); // Добавляем индекс в вектор
                }
            }
        }

        int index2 = newText.indexOf("xml");
        if (index2 != -1) {
            indexes2.append(index2); // Добавляем индекс в вектор

            while (index2 != -1) {
                index2 = newText.indexOf("xml", index2 + 1); // Ищем следующее вхождение символа '/' начиная с позиции index + 1
                if (index2 != -1) {
                    indexes2.append(index2); // Добавляем индекс в вектор
                }
            }
        }

        if (indexes.size() < 2){
            int start = indexes.at(0) + 8;
            int end = indexes2.at(0) + 3;
            QString subString = newText.mid(start, end - start);
            changedTextList.append(subString);
            qDebug() << "Подстрока" << 1 << ":" << subString;
        }
        else{
            for (int i = 0; i < indexes.size(); ++i) {
                if (i < indexes2.size()) {
                    int start = indexes.at(i) + 8;
                    int end = indexes2.at(i) + 3;
                    if (start >= 0 && end >= start && end <= newText.length()) {
                        QString subString = newText.mid(start, end - start);
                        changedTextList.append(subString);
                        qDebug() << "Подстрока" << i + 1 << ":" << subString;
                    }
                }
            }
        }

        qDebug() << changedTextList.size();

        if(changedTextList.size() < 2){
            fileMap.insert(projectObozn(changedTextList[0]),changedTextList[0]);
            parseXML(changedTextList[0]);
            setupStampData();
        }
        else{
            for (int i = 0; i < changedTextList.size(); i++){
                fileMap.insert(projectObozn(changedTextList[i]),changedTextList[i]);
                parseXML(changedTextList[i]);
                setupStampData();
            }
        }
        qDebug() << fileMap.size();
        if (!fileMap.isEmpty()) {
            auto it = fileMap.begin(); // Получаем итератор на первый элемент
            QString firstKey = it.key(); // Получаем ключ первого элемента
            QString firstValue = it.value(); // Получаем значение первого элемента

            qDebug() << "Первый элемент QMap: " << firstKey << "=" << firstValue;
        } else {
            qDebug() << "QMap пуст";
        }

        ui->boasList->clear();//Перетащите сюда файл(-ы)
    }

    // Получаем компоновку (layout) из boasListGB
    QLayout *layout = ui->boasListGB->layout();

    // Проверяем, существует ли компоновка
    if (layout) {
        // Перебираем все дочерние виджеты в компоновке
        while (QLayoutItem *item = layout->takeAt(0)) {
            // Удаляем виджет из компоновки и освобождаем память
            delete item->widget();
            delete item;
        }
        delete layout;
    }

    // Удаляем layout из QGroupBox
//    ui->boasListGB->setLayout(nullptr);

    // Создаем вертикальную компоновку
    QGridLayout *gridLayout = new QGridLayout();

    QList<QCheckBox*> checkBoxList;
    QList<QLineEdit*> lineEditList;

    int projectsCount = fileMap.size();

    if(projectsCount < 2){
        auto it = fileMap.begin(); // Получаем итератор на первый элемент
        QString firstKey = it.key(); // Получаем ключ первого элемента
        QCheckBox *checkBox = new QCheckBox(firstKey, this); // Используем имя группы "boasListGB"
        QLineEdit *lineEdit = new QLineEdit(nullptr,this);
        lineEdit->setObjectName(firstKey);
        checkBox->setChecked(true);
        fileFinals.append(fileMap[checkBox->text()]);
        fileFinals.removeDuplicates();
        checkBoxList.append(checkBox);
        lineEditList.append(lineEdit);
    }
    else{
        for (auto it = fileMap.constBegin(); it != fileMap.constEnd(); ++it){
            QString firstKey = it.key(); // Получаем ключ первого элемента
            QCheckBox *checkBox = new QCheckBox(firstKey, this); // Используем имя группы "boasListGB"
            QLineEdit *lineEdit = new QLineEdit(nullptr,this);
            lineEdit->setObjectName(firstKey);
            checkBox->setChecked(true);
            fileFinals.append(fileMap[checkBox->text()]);
            fileFinals.removeDuplicates();
            checkBoxList.append(checkBox);
            lineEditList.append(lineEdit);
        }
    }

    // Создание и добавление кнопок для каждого графика на форму и соединение сигнала переключения с обработчиком
    for (int i = 0; i < checkBoxList.size(); i++) {
        connect(checkBoxList[i], &QCheckBox::stateChanged, this, [=](int checked) {
            if (checked == Qt::Checked) {
                fileFinals.append(fileMap[checkBoxList[i]->text()]);
                fileFinals.removeDuplicates();
                qDebug() << fileFinals;
            } else {
                int index = fileFinals.indexOf(fileMap[checkBoxList[i]->text()]);
                if (index != -1) {
                    fileFinals.removeAt(index);
                    qDebug() << fileFinals;
                }
            }
        });
    }

    for (int i = 0; i < checkBoxList.size(); i++) {
        gridLayout->addWidget(checkBoxList[i], i, 0);
        gridLayout->addWidget(lineEditList[i], i, 1);
    }

    // Устанавливаем созданную вертикальную компоновку для QGroupBox
    ui->boasListGB->setLayout(gridLayout);

//    // Установка менеджера размещения на группу кнопок
//    ui->boasListGB->setLayout(layout);

}

void MainWindow::on_clearFilesList_clicked()
{
//    fileMap.clear();
    if(ui->clearFilesList->text() == "Очистить список"){
        ui->clearFilesList->setText("Подтвердить");        
    }
    else{
        // Получаем компоновку (layout) из boasListGB
        QLayout *layout = ui->boasListGB->layout();

        // Проверяем, существует ли компоновка
        if (layout) {
            // Перебираем все дочерние виджеты в компоновке
            while (QLayoutItem *item = layout->takeAt(0)) {
                // Удаляем виджет из компоновки и освобождаем память
                delete item->widget();
                delete item;
            }
            delete layout;
        }
        ui->clearFilesList->setText("Очистить список");

    }
}

void MainWindow::on_choseGroupDir_clicked()
{
    outDir = QFileDialog::getExistingDirectory(0," Выберите папку, в которую хотите сохранить ВП","");
    ui->lineEdit->setText(outDir);
}
