#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QImage>
#include <QPixmap>
#include <QImageReader>
#include <QFileDialog>
#include <bits/stdc++.h>
#include <QDebug>
#include <QTimer>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->btn_find->setDisabled(true);
    connect(ui->btn_choose, SIGNAL(clicked(bool)), this, SLOT(setPicture()));
    connect(ui->btn_find, SIGNAL(clicked(bool)), this, SLOT(findObject()));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow:: setPicture()
{
    QFileDialog dialog(this);
    dialog.setNameFilter(tr("Images (*.png *.bmp *.jpg)"));
    dialog.setViewMode(QFileDialog::Detail);
    QString filename = QFileDialog::getOpenFileName(this, tr("Open File"),
                                                    "C:/",
                                                    tr("Images (*.png *.xpm *.jpg)"));
    if (filename == "") image=nullptr;

    QImageReader reader(filename);
    reader.setAutoTransform(true);
    QImage newImage = reader.read();
    if (newImage.isNull()) image = nullptr;
    image = new QImage(newImage);
    ui->btn_find->setDisabled(false);
    update_image();
}

void MainWindow::update_image() {
    if (!image) return;
    ui->lb_picture->setPixmap(QPixmap::fromImage(*image));
}

void MainWindow:: findObject()
{
    QTimer *tim;
    tim = new QTimer();
    tim->start();
    while(tim->interval() != 10)
    {
        tim++;
    }
    ui->tb_text->setText("Предположительно " + transport + ", номер рейса: " + number);


}
