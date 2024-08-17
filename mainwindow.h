#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QDebug>
#include <QRegularExpression>
#include <QRegularExpressionMatch>
#include <QFileInfo>
#include <QDialog>
#include <QFileDialog>
#include <QXmlStreamReader>
#include <QtMath>
#include <QProcess>
#include <QCheckBox>
//#include <QLinearMapping>

//for tables
#include "xlsxdocument.h"
#include "xlsxchartsheet.h"
#include "xlsxcellrange.h"
#include "xlsxchart.h"
#include "xlsxrichstring.h"
#include "xlsxworkbook.h"

//#include <QOpenGLWidget>
//#include <QOpenGLFunctions>
//#include <QOpenGLTexture>
//#include <QOpenGLShaderProgram>
//#include <QOpenGLBuffer>
//#include <QOpenGLContext>
//#include <QOpenGLVertexArrayObject>
//#include <QOpenGLExtraFunctions>
//#include <QPropertyAnimation>
//#include "oglwidget.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

signals:
    void sendOffsetX(qreal x);
    void sendOffsetY(qreal y);

private slots:

//    struct vpline {
//        QString num;
//        QString oboz;
//        QString name;
//        QString comment;
//        bool underline;
//        QString type;
//        bool merge;
//        QString vhodit;
//        QString post;
//    }pivpline;


    void createSheetXlsxVP(QString filemane, QString sheetName);
    void copyPE2listStyle(QMap <int,double>& columnWidthPE, QMap <int,double>& rowHeightPE, QMap<QString, QString>& cellsWithTextPE, QList<QXlsx::CellRange>& merged_cellsPE, QMap<QString, QXlsx::Format>& cellFormatsPE);       
    void copySP2listStyle(QMap <int,double>& columnWidthPE, QMap <int,double>& rowHeightPE, QMap<QString, QString>& cellsWithTextPE, QList<QXlsx::CellRange>& merged_cellsPE, QMap<QString, QXlsx::Format>& cellFormatsPE);    
    void createSheetXlsxPE(QString filemane, QString sheetName);
    void createSheetXlsxSP(QString filemane, QString sheetName);    
    void containersClear();
    void createPE(QString filemane);
    void createPE(QString filemane, QString distPath);
    void createSP(QString filemane);
    void createSP(QString filemane, QString distPath);
    void createVP(QString filemane);
    void createVP(QString filemane, QString distPath);
    void createVPs(QString filemane);
    void createVPs(QString filemane, QString distPath);
    void excelPost(QString filemane, QString format);
    void setupStampData();
    void reloadStamp();
//    QMap<QString, QList<vpline>> readAndProcessXMLFiles(const QStringList& xmlFiles);
    void parseXML(const QString& filePath);
    void parseStamp(const QString& filePath);
    void parseElementLines(const QString& filePath);
//    void mouseMoveEvent(QMouseEvent *event);
//    bool eventFilter(QObject *obj, QEvent *event);
//    void enterEvent(QEvent *event);

    void on_pushButton_clicked();

    void on_pushButton_2_clicked();

    void on_pushButton_3_clicked();

    void on_choseXml_clicked();

    void on_boasList_textChanged();

    void on_clearFilesList_clicked();

    void on_choseGroupDir_clicked();

private:
    Ui::MainWindow *ui;
//    OGLWidget *oGL;
};
#endif // MAINWINDOW_H
