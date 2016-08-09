#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "generalheader.h"
#include <QUdpSocket>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    QPushButton *sendDataButton;

private:
    Ui::MainWindow *ui;
    QUdpSocket *m_socket_sender;

public slots :
    void sendData();

};

#endif // MAINWINDOW_H
