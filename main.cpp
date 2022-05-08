#include "mainwindow.h"
#include "receiver.h"

#include <QApplication>

#include <iostream>
#include <thread>

int main(int argc, char* argv[])
{
    QApplication a(argc, argv);

    ReceiverOptions opt;
    opt.addr = "127.0.0.1";
    opt.port = 23002;
    if (argc == 3) {
        opt.addr = argv[1];
        opt.port = atoi(argv[2]);
    }
    else {
        std::cout << "Pass server ip address and port as program arguments\n"
                  << "Default arguments: addr = 127.0.0.1 port = 23002" << std::endl;
    }

    Receiver rcv(opt);
    std::thread recvThread(&Receiver::receiveData, &rcv);

    MainWindow w;

    QObject::connect(&rcv, &Receiver::bitmapChanged, &w, &MainWindow::repaintBitmap);

    w.show();

    a.exec();

    recvThread.join();

    return 0;
}
