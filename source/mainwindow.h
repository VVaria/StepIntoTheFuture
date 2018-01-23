#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QWidget>
#include <QMainWindow>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QImage>
#include <QLabel>
#include <bits/stdc++.h>
#include <QString>
#include <QTimer>
#include <QProcess>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    QImage *image = nullptr;
    QImage *img = nullptr;
    QProcess *pythonProcess = nullptr;
    QString transport;
    QString number;
    QTimer tim;
    QString filename;

private slots:
    void findObject();
    void setPicture();
    void parseJson(int exitCode);
    void update_image();
    void onElapsed();
    void update_image_r(int y1, int x1, int y2, int x2, QString name, double rate);
};

#endif // MAINWINDOW_H
