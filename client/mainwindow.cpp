#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QByteArray>
#include <unistd.h>
#include <QString>
#include <QDataStream>
#include <iostream>



#include <iostream>
//boost

#include <boost/dynamic_bitset.hpp>


int count;
int trigger_count = 0;
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    m_socket_sender(0),
    n_pressed(0),
    trigger_count(0),
    stop_sending(false),
    ui(new Ui::MainWindow)
{

    QString q_1 = "bbbb";
    QString q_2 = "aaaa";
    QString q_final;
    q_final.append(q_2);
    q_final.append(q_1);
    std::cout << "Q_FINAL: " << q_final.toStdString() << std::endl;
    std::cout << "Q_FINAL.right(4) " << q_final.right(4).toStdString() << std::endl;
    QString test = "0123456789abcdefghijklmnop";
    std::cout << "test.left(8): " << test.left(8).toStdString() << std::endl;
    std::cout << "test.mid(14,2): " << test.mid(14,2).toStdString() << std::endl;

    ui->setupUi(this);

    m_socket_sender = new QUdpSocket(this);
    bool bind = m_socket_sender->bind(QHostAddress::LocalHost, 2225);
    if(!bind) {
        qDebug() << "unable to bind socket SENDER";
        m_socket_sender->close();
        m_socket_sender->disconnectFromHost();
    }

    //connect(ui->sendMINI2_button, SIGNAL(clicked()), this, SLOT(sendDataMINI2()));
    //connect(ui->sendMMFE8_button, SIGNAL(clicked()), this, SLOT(sendDataMMFE8()));
    connect(ui->sendMINI2_button, SIGNAL(clicked()), this, SLOT(stopMMFE8()));

    connect(ui->sendMMFE8_button, SIGNAL(clicked()), this, SLOT(startMMFE8()));
    connect(&sendTimer, SIGNAL(timeout()), this, SLOT(sendDataMMFE8()));
    count = 0;

    //n_to_send = 1000;
    n_to_send = 20000;

    connect(ui->send_dummy, SIGNAL(clicked()), this, SLOT(send_dummy_ddaq())); 

}
void MainWindow::startMMFE8()
{
    sendTimer.start(1);
    stop_sending = false;
}

void MainWindow::stopMMFE8()
{
    sendTimer.stop();
    stop_sending = true;
}

void MainWindow::sendDataMINI2()
{
    //for(int i = 0; i < 3; i++) {
    for(int i = 0; i < n_to_send; i++) {
        usleep(500000);
        QByteArray data;
        data.clear(); 
        QDataStream out(&data, QIODevice::WriteOnly);
        out.device()->seek(0);
        uint8_t _d0 = 40;
        uint8_t _d1 = 171;
        uint8_t _d2 = 204;
        uint8_t _d3 = 54;
        uint8_t _d4 = 91;

        uint32_t _frame = i;
        boost::dynamic_bitset<> frame(32, _frame);


        uint32_t _headerId = 0x564d32;
        boost::dynamic_bitset<> headerId(32, _headerId);
        uint32_t _chipId = 4;
        boost::dynamic_bitset<> chipId(32, _chipId);
        boost::dynamic_bitset<> header(32, 0);
        header = header | chipId;
        header = (header << 24)  | headerId;
        //header = header | headerId;
        //header = (header << 8) | chipId;

        uint32_t _trigCounter = i;
        boost::dynamic_bitset<> trigCounter(32, _trigCounter);
        uint32_t _timeStamp = 18;
        boost::dynamic_bitset<> timeStamp(32, _timeStamp);
        boost::dynamic_bitset<> headerInfo(32, 0);
        headerInfo = headerInfo | timeStamp;
        headerInfo = (headerInfo << 16) | trigCounter;

        boost::dynamic_bitset<> d0(32, _d0);
        boost::dynamic_bitset<> d1(32, _d1);
        boost::dynamic_bitset<> d2(32, _d2);
        boost::dynamic_bitset<> d3(32, _d3);
        boost::dynamic_bitset<> d4(32, _d4);

        std::cout << "frame       : " << frame << std::endl;
        std::cout << "header      : " << header << std::endl;
        std::cout << "header info : " << headerInfo << std::endl;

        std::cout << "d0: " << d0 << std::endl;
        std::cout << "d1: " << d1 << std::endl;
        std::cout << "d2: " << d2 << std::endl;
        std::cout << "d3: " << d3 << std::endl;
        std::cout << "d4: " << d4 << std::endl;

        boost::dynamic_bitset<> first32(32, 0);
        first32 = first32 | d3;
        first32 = (first32 << 8) | d2;
        first32 = (first32 << 8) | d1;
        first32 = (first32 << 8) | d0;

        boost::dynamic_bitset<> second32(32, 0);
        second32 = second32 | d4;
        

        out << (quint32)frame.to_ulong();
        out << (quint32)header.to_ulong();
        out << (quint32)headerInfo.to_ulong();
        out << (quint32)first32.to_ulong();
        out << (quint32)second32.to_ulong();
        
        std::cout << "sending [" << i << "] " << data.toHex().toStdString() << std::endl;
        m_socket_sender->writeDatagram(data, QHostAddress::LocalHost, 1236);
        count++;



        // send dummy art

        uint32_t _headerId_art = 0x564132;
        boost::dynamic_bitset<> headerId_art(32, _headerId_art);
        boost::dynamic_bitset<> channel_bits(32, 0);
        boost::dynamic_bitset<> art_channel_or(32, 0x38000000); // art channel = 56
        channel_bits |= art_channel_or;

        boost::dynamic_bitset<> timestamp_art(32, 0x5e); // timestamp = 94;
        boost::dynamic_bitset<> art1_data_and_flag(32, 0x8d); // art1 data: flag = 1, channel = 13
        boost::dynamic_bitset<> art2_data_and_flag(32, 0x31); // art2 data: flag =  0, channel = 49

        boost::dynamic_bitset<> art_data_word(32, 0);

        art_data_word |= art1_data_and_flag;
        boost::dynamic_bitset<> blah((art_data_word << 8));
        blah |= art2_data_and_flag;
        blah = (blah << 16);
        art_data_word |= ((art_data_word << 8) | art2_data_and_flag);
        art_data_word |= ((art_data_word << 8) | timestamp_art);

        blah = (blah | timestamp_art);
        boost::dynamic_bitset<> final(blah); 


        data.clear();
        out.device()->seek(0);
        out << (quint32)0;
        out << (quint32)headerId_art.to_ulong();
        out << (quint32)channel_bits.to_ulong();
        out << (quint32)0;
        out << (quint32)final.to_ulong();

        // send the art
        m_socket_sender->writeDatagram(data, QHostAddress::LocalHost, 1236);
        std::cout << "sending ART [" << i << "] " << data.toHex().toStdString() << std::endl;
        
        // send the fafafafa
        data.clear();
        out.device()->seek(0);
        out << (quint32)0xfafafafa;
        m_socket_sender->writeDatagram(data, QHostAddress::LocalHost, 1236);
        




    }
}

void MainWindow::sendDataMMFE8()
{
    if(stop_sending) return;
    //trigger_count += n_to_send + ( (n_pressed==0) ? 0 : n_to_send);
    //for(int i = 0; i <= n_to_send; i++) {
    int i = 0;
    //while(true) {
    
    //qDebug() << "sendData   sender state: " << m_socket_sender->state();
        //usleep(500);
        //sleep(0.5);
        QByteArray data;

        data.clear();
        QDataStream out(&data, QIODevice::WriteOnly);
    //if(trigger_count%2==0) {
        out.device()->seek(0);
///*
        int count_to_send = trigger_count;
        out << (quint32)count_to_send; // event counter
        out << (quint32)0x1555A3B; // vmmid = 59, gcid = 21850, precision = 1
        out << (quint32)0x1555A3B; // vmmid = 59, gcid = 21850, precision = 1
        //usleep(20000);
    while(i<5) {
        //if(i%2==0) continue;
        //trigger_count = i;
        //if(i%2==0 && !(i==0)) trigger_count++;
        //if(i%2==0) trigger_count = i;
        //if(i>200) {
        //    if(i%2==0) trigger_count++;
        //}
        //else {
        //    trigger_count = i;
        //}
        //if(i==0) trigger_count = i;
     //   if(i==4231) trigger_count = i+4;
        //if(i==4232) trigger_count = 4234;
        out << (quint32)0xc06c0f; // pdo = 15, gray = 27, tdo = 3
        //out << (quint32)0xbe; // flag = 0, threshold = 1, chan = 47
     //   out << (quint32)0xaa;
     //   out << (quint32)0xbb;
     //   out << (quint32)0xc06c0f; // pdo = 15, gray = 27, tdo = 3
     //   out << (quint32)0xbe; // flag = 0, threshold = 1, chan = 47
     //   out << (quint32)0xc06c0f; // pdo = 15, gray = 27, tdo = 3
     //   out << (quint32)0xbe; // flag = 0, threshold = 1, chan = 47
        //trigger_count = i;

        
        count++;
        i++;
    }
        out << (quint32)0xFFFFFFFF;
        trigger_count++;
        //if(rand()%10==0) trigger_count -= 10;
        std::cout << "sending [" << i << "] " << data.toHex().toStdString() << std::endl;
        m_socket_sender->writeDatagram(data, QHostAddress::LocalHost, 2226);
    n_pressed++;
    //}
    //else {
    

//    */
        // send an empty
    //if(rand()%2==0) {

    /*
        if(rand()%2==0) {
        //if(true) {
        //if(true) {
            data.clear();
            out.device()->seek(0);
            //for(int i = 0; i < 5; i++) {
            out << (quint32)trigger_count; // event counter
            out << (quint32)0x1555A3B; // vmmid = 59, gcid = 21850, precision = 1
            out << (quint32)0xFFFFFFFF;
            std::cout << "sending [" << i << "] " << data.toHex().toStdString() << std::endl;
            m_socket_sender->writeDatagram(data, QHostAddress::LocalHost, 2226);
            trigger_count++;
            //}
        }
    //}
    */
}

void MainWindow::send_dummy_ddaq()
{
    QUdpSocket* socket = new QUdpSocket(this);
    bool bind = socket->bind(QHostAddress::LocalHost, 2222);

    for(int i = 0; i < 20; i++) {
        usleep(500);
    QByteArray array;
    QDataStream out(&array, QIODevice::WriteOnly);
    int port = std::stoi(ui->port_to_send_to->text().toStdString());
    std::cout << "sending to port : " << port << std::endl;
    if(i%2==0) port = 1244;
    if(port==1234) {
        out << 0x01;
        out << 0x01;
        out << 0x01;
        out << 0x01;
    }
    else {
        out << 0x02;
        out << 0x02;
        out << 0x02;
        out << 0x02;
    }
    socket->writeDatagram(array, QHostAddress("127.0.0.1"), port);
    }

}


MainWindow::~MainWindow()
{
    delete ui;
}
