#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QImage>

#include <QPixmap>
#include <QImageReader>
#include <QFileDialog>
#include <bits/stdc++.h>
#include <QDebug>
#include <QProcess>
#include <QTimer>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QPainter>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->btn_find->setDisabled(true);
    ui->lb_picture->setScaledContents(1);
    ui->lb_pictrurenew->setScaledContents(1);
    connect(ui->btn_choose, &QPushButton::clicked, this, &MainWindow::setPicture);
    connect(ui->btn_find, SIGNAL(clicked(bool)), this, SLOT(findObject()));
    connect(&tim, &QTimer::timeout, this, &MainWindow::onElapsed);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow:: setPicture()
{
    ui->lb_pictrurenew->clear();
    ui->tb_text->clear();
    ui->btn_find->setDisabled(false);
    QFileDialog dialog(this);
    dialog.setNameFilter(tr("Images (*.jpg)"));
    dialog.setViewMode(QFileDialog::Detail);
    filename = QFileDialog::getOpenFileName(this, tr("Open File"),
                                                    "C:/",
                                                    tr("Images (*.jpg)"));
    if (filename == "")
        return;

    QImageReader reader(filename);
    reader.setAutoTransform(true);
    QImage newImage = reader.read();

    if(image != nullptr)
        delete image;

    if (newImage.isNull())
        return;

    image = new QImage(newImage);
    image->scaled(ui->lb_picture->width(), ui->lb_picture->height(), Qt::KeepAspectRatio, Qt::FastTransformation);

    update_image();
}

void MainWindow::parseJson(int exitCode)
{
    QByteArray str = pythonProcess->readAll();
    qDebug()<<Q_FUNC_INFO<<str<<exitCode;
    auto doc = QJsonDocument::fromJson(str);
    auto rootObject = doc.object();
    QVariantMap map = rootObject.toVariantMap();

    float w = map["image_width"].toInt();
    float h = map["image_height"].toInt();
    auto stroke = QString::fromUtf16(u"Ширина картинки: ");
    stroke += map["image_width"].toString() + "\n" + QString::fromUtf16(u"Длина картинки: ") + map["image_height"].toString() + "\n" + QString::fromUtf16(u"Объект(ы), найденный(ые) на картинке: ");

    for (auto i : map["classes"].toStringList())
    {
        stroke += (i + " ");
    }
    stroke += "\n" + QString::fromUtf16(u"Точность определения объекта(ов): ");
    QList <QString> names = map["classes"].toStringList();

    for (QVariant i : map["scores"].toList())
    {
        stroke += (i.toString() + " ");
    }
    stroke += "\n" + QString::fromUtf16(u"Координаты прямоугольника(ов), в котором(ых) находятся объект(ы): ");
    QList <QVariant> rates = map["scores"].toList();

    auto arr = map["bboxes"].toList();
    img = new QImage(*image);
    for (int i = 0; i < arr.size(); i++)
    {
        QList <QVariant> list = arr[i].toList();

        stroke += names[i] + ": (";
        int y1 = (int)(list[0].toDouble() * h);
        int x1 = (int)(list[1].toDouble() * w);
        int y2 = (int)(list[2].toDouble() * h);
        int x2 = (int)(list[3].toDouble() * w);

        stroke += (QString::number(x1) + ", " + QString::number(y1) + ", " + QString::number(x2) + ", " + QString::number(y2));
        stroke += ")";
        if (list.size() == 4)
          update_image_r(y1, x1, y2, x2, names[i], rates[i].toDouble());
        stroke += "\n";
    }

    ui->tb_text->setText(stroke);
    ui->btn_choose->setDisabled(false);
}

void MainWindow::update_image() {
    if (!image) return;

    ui->lb_picture->setScaledContents(true);
    image->scaled(ui->lb_picture->width(), ui->lb_picture->height(), Qt::KeepAspectRatio, Qt::FastTransformation);
    ui->lb_picture->setPixmap(QPixmap::fromImage(*image));

    ui->btn_choose->setDisabled(true);
}

void MainWindow::onElapsed()
{
    ui->tb_text->setText("Предположительно " + transport + ", номер рейса: " + number);
}

void MainWindow::update_image_r(int y1, int x1, int y2, int x2, QString name, double rate)
{
    QString str = name + " " + QString::number(rate);
    QPainter painter;
    painter.begin(image);
    painter.setPen(QPen(Qt::red, 3, Qt::DashLine));
    painter.drawRect(x1, y1, x2 - x1, y2 - y1);
    QFont font = painter.font();
    font.setPointSize(18);
    painter.setFont(font);
    if (y1 > 18)
        painter.drawText(x1 + 2, y1, str);
    else
        painter.drawText(x1 + 2, y1 + 30, str);
    painter.end();
    image->scaled(ui->lb_pictrurenew->width(), ui->lb_pictrurenew->height(), Qt::KeepAspectRatio, Qt::FastTransformation);
    ui->lb_pictrurenew->setPixmap(QPixmap::fromImage(*image, 0));
}

void MainWindow:: findObject()
{
    if (!image) return;

    ui->btn_find->setDisabled(true);

    pythonProcess =  new QProcess(this);
    connect(pythonProcess, static_cast<void (QProcess::*)(int)>(&QProcess::finished), this, &MainWindow::parseJson);
    connect(pythonProcess,&QProcess::started,[this](){
        qDebug()<<"Process started";
    });
    connect(pythonProcess,&QProcess::errorOccurred,[this](auto error){
        qDebug()<<"Process error"<<error<<pythonProcess->errorString();
    });
    pythonProcess->setWorkingDirectory("C:\\Users\\zabelin\\Documents\\Projects\\python\\SSD-Tensorflow");
    pythonProcess->setProcessChannelMode(QProcess::MergedChannels);

    QString pythonScript = "C:\\Users\\zabelin\\AppData\\Local\\Programs\\Python\\Python35\\python.exe";
    pythonProcess->setProgram(pythonScript);
    QStringList arguments;
    arguments <<"demo\\tst_voc_model.py"<< "-im"<<filename;
    pythonProcess->setArguments(arguments);
    pythonProcess->start();
    pythonProcess->waitForFinished();
}
