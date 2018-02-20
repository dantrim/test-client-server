#include "mainwindow.h"
#include <sstream>
#include <iostream>
#include <thread>
#include <chrono>
using namespace std;
#include "ui_mainwindow.h"

int receive_cnt = 0;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    m_socket_receiver(0),
    event_count(0),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);


    m_socket_receiver = new QUdpSocket(this);
    bool bind = m_socket_receiver->bind(QHostAddress::LocalHost, 1235);
    //bool bind = m_socket_receiver->bind(QHostAddress("127.0.0.1"), 1240);
    //bool bind = m_socket_receiver->bind(QHostAddress("192.168.0.2"),6008);
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
   // cout << "incoming data = " << incomingData.toHex().toStdString() << endl;

    // write back
    //QByteArray array;
    //QDataStream out(&array, QIODevice::WriteOnly);
    //out << 0xcc << 0xff << 0xaa << 0xbb;
    //m_socket_receiver->writeDatagram(array, QHostAddress("127.0.0.1"), 1234);

}

void MainWindow::readData()
{
    //qDebug() << "Receiving data packet";
    QHostAddress fromIP;
    quint16 fromPort;
    QByteArray incomingDatagram;
    
    while(m_socket_receiver->hasPendingDatagrams()) {
        incomingDatagram.resize(m_socket_receiver->pendingDatagramSize());
        m_socket_receiver->readDatagram(incomingDatagram.data(),
                                            incomingDatagram.size(),
                                            &fromIP, &fromPort);
        parseData(incomingDatagram);
        qDebug() << "data from IP: " << fromIP.toString() ;
        cout << "    > port : " << (unsigned)fromPort << endl;
        cout << "data size = " << incomingDatagram.size() << " : " << incomingDatagram.toHex().toStdString() << endl;
        uint32_t* datain = (uint32_t*)incomingDatagram.data();
        QByteArray array;
        QDataStream out(&array, QIODevice::WriteOnly);
        //out << incomingDatagram[0];// << 0xbb << 0xaa << 0xcc;

        // control command [fpga header + RESET]
        bool do_echo = true;
        if(incomingDatagram.size() == 12) {
            do_echo = false;
            out << (uint8_t)0x7e;
            out << (uint8_t)0xff;
            out << (uint8_t)0x63;
            out << (uint8_t)0xe5;
            out << (uint8_t)0x5e;
            out << (uint8_t)0x7e;
        }
        else {
            cout << "sending echo with stripped stuff" << endl;
            size_t incoming_size = incomingDatagram.size();
            int n_bytes_reply = ((incoming_size - 8)); // remove FPGA header and add SOF/EOF
            uint8_t reply_data [n_bytes_reply];
            std::copy(incomingDatagram.data() + 8, incomingDatagram.data() + incoming_size, reply_data);

            //for(int i = 0; i < n_bytes_reply; i++) out << (uint8_t)reply_data[i];


            n_bytes_reply = 8;
            uint8_t  framed_reply [n_bytes_reply];
            std::copy(incomingDatagram.data() + 8, incomingDatagram.data() + 16, framed_reply);
            uint8_t framed_tid_corr [n_bytes_reply];
            std::copy(incomingDatagram.data() + 8, incomingDatagram.data() + 16, framed_tid_corr);
            //framed_tid_corr[1] = framed_reply[2];
            
            cout << " ==> ";
            for(int i = 0; i < n_bytes_reply; i++) cout << std::hex << " " << (unsigned)framed_tid_corr[i];
            cout << endl;
            out << (uint8_t)0x7e;
            for(int i = 0; i < n_bytes_reply; i++) out << (uint8_t)framed_tid_corr[i];
            out << (uint8_t)0x7e;
        }
//        // I2C write command [fpga header + (addr+control) + (payload header) + (FCS)]
//        else if(incomingDatagram.size() == (8 + 2 + 4 + 2)) {
//            out << (uint8_t)0x7e;
//            out << (uint8_t)0xff;
//            out << (uint8_t)0x0d;
//            out << (uint8_t)0xd1;
//            out << (uint8_t)0x00;
//            out << (uint8_t)0x6f;
//            out << (uint8_t)0x7f;
//            out << (uint8_t)0x7e;
//        }
    
//        out << (uint32_t)datain[0];
        //std::this_thread::sleep_for(std::chrono::milliseconds(100));
        quint16 port_to_send = 1234;
        uint8_t data = 0xff;
        if((unsigned)incomingDatagram.data()[0] == 0xcc) port_to_send = 1234;
        else if((unsigned)incomingDatagram.data()[0] == 0xdd) port_to_send = 1244;
        //data = 0x01;
        //out << (unsigned)incomingDatagram.data()[0];
        //out << (unsigned)incomingDatagram.data()[1];
        //out << (unsigned)incomingDatagram.data()[2];
        //out << (unsigned)incomingDatagram.data()[3];
        receive_cnt++;
        port_to_send = 1234;
        
        m_socket_receiver->writeDatagram(array, QHostAddress("127.0.0.1"), port_to_send);
        //m_socket_receiver->writeDatagram(incomingDatagram.data(), QHostAddress("127.0.0.1"), port_to_send);

        //QString input = QString(incomingDatagram);
        //ui->serverText->append(input);
    } // while
    event_count++;
}

MainWindow::~MainWindow()
{
    delete ui;
}
