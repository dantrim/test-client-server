#include "mainwindow.h"
#include <sstream>
#include <iostream>
using namespace std;
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    m_socket_receiver(0),
    event_count(0),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    m_socket_receiver = new QUdpSocket(this);
    bool bind = m_socket_receiver->bind(QHostAddress::LocalHost, 2224);
    if(!bind) {
        qDebug() << "unable to bind socket SENDER";
        m_socket_receiver->close();
        m_socket_receiver->disconnectFromHost();
    }

    connect(m_socket_receiver, SIGNAL(readyRead()), this, SLOT(readData()));


}
void MainWindow::parseData(QString incomingData)
{
    stringstream sx;
    sx << "------------- incoming event " << event_count << " ---";
    QString line = QString::fromStdString(sx.str());
    ui->serverText->append(line);
    ui->serverText->append(incomingData);

}

void MainWindow::readData()
{
    //qDebug() << "Receiving data packet";
    QHostAddress fromIP;
    QByteArray incomingDatagram;
    
    while(m_socket_receiver->hasPendingDatagrams()) {
        incomingDatagram.resize(m_socket_receiver->pendingDatagramSize());
        m_socket_receiver->readDatagram(incomingDatagram.data(),
                                            incomingDatagram.size(),
                                            &fromIP);
        parseData(QString(incomingDatagram));

        //QString input = QString(incomingDatagram);
        //ui->serverText->append(input);
    } // while
    event_count++;
}

MainWindow::~MainWindow()
{
    delete ui;
}
