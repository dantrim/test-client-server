#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QByteArray>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    sendDataButton(0),
    m_socket_sender(0),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    m_socket_sender = new QUdpSocket(this);
    bool bind = m_socket_sender->bind(QHostAddress::LocalHost, 2223);
    if(!bind) {
        qDebug() << "unable to bind socket SENDER";
        m_socket_sender->close();
        m_socket_sender->disconnectFromHost();
    }

    sendDataButton = new QPushButton("Send Data", this);
    sendDataButton->move(10,10);
    sendDataButton->show();

    connect(sendDataButton, SIGNAL(clicked()), this, SLOT(sendData()));
}

void MainWindow::sendData()
{
    qDebug() << "sendData   sender state: " << m_socket_sender->state();
    QByteArray data = "test data send";
    m_socket_sender->writeDatagram(data, QHostAddress::LocalHost, 2224);


}

MainWindow::~MainWindow()
{
    delete ui;
}
