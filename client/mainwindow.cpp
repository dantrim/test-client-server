#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QByteArray>
#include <unistd.h>
#include <QDataStream>
#include <iostream>


int count;
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
    count = 0;
}

void MainWindow::sendData()
{
    for(int i = 0; i < 500; i++) {
    //qDebug() << "sendData   sender state: " << m_socket_sender->state();
        //usleep(500000);
        usleep(5000);
        //sleep(0.5);
        QByteArray data;
        data.clear();
        //QByteArray data = "datagram sending count: ";
        //data.append(QString::number(count));

        QDataStream out(&data, QIODevice::WriteOnly);
        out.device()->seek(0);
       // if(i%2==0) {
       //     out << (quint32)1;
       //     out << (quint32)1;
       //     out << (quint32)2;
       //     out << (quint32)2;
       //     out << (quint32)3;
       //     out << (quint32)3;
       //     out << (quint32)4;
       //     out << (quint32)4;
       // }
       // else {
            out << (quint32)i; // event counter
            out << (quint32)0x1555A3B; // vmmid = 59, gcid = 21850, precision = 1
            out << (quint32)0xc06c0f; // pdo = 15, gray = 27, tdo = 3
            out << (quint32)0xbe; // flag = 0, threshold = 1, chan = 47


           // out << (quint32)1;
           // out << (quint32)1;
           // out << (quint32)2;
           // out << (quint32)2;
           // out << (quint32)3;
           // out << (quint32)3;
           // out << (quint32)4;
           // out << (quint32)4;
           // out << (quint32)5;
           // out << (quint32)5;
           // out << (quint32)6;
           // out << (quint32)6;
            out << (quint32)0xFFFFFFFF;
            //out << (quint32)0;

//sending [0] : 000001a1
//sending [4] : 01555a3b
//sending [8] : 00c06c0f
//sending [12] : 000000be
       // }
        //out << (quint32)0x9FD75;
        //out << (quint32)0xBF0F0F0F;
        
      //  for(int i = 0; i < 16; ) {
      //      std::cout << "sending [" << i << "] : " << data.mid(i,4).toHex().toStdString() << std::endl;
      //      i+=4;
      //  }
        
        m_socket_sender->writeDatagram(data, QHostAddress::LocalHost, 1236);
        count++;
    }


}

MainWindow::~MainWindow()
{
    delete ui;
}
