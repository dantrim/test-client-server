#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "generalheader.h"
#include <QUdpSocket>
#include <QTimer>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    int n_to_send;

    QTimer sendTimer;
    int trigger_count;


private:
    Ui::MainWindow *ui;
    QUdpSocket *m_socket_sender;
    int n_pressed;
    bool stop_sending;

public slots :
    void sendDataMMFE8();
    void sendDataMINI2();
    void startMMFE8();
    void stopMMFE8();
    void send_dummy_ddaq();

};

#endif // MAINWINDOW_H
