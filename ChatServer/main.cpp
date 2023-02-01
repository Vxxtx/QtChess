#include <QtCore/QCoreApplication>
#include <qthread.h>
#include "ListenerConnection.h"
#include <iostream>

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    QThread* ConnectionThread = new QThread();
    ListenerConnection* Connection = new ListenerConnection();

   
    Connection->moveToThread(ConnectionThread);

    QObject::connect(Connection,
        &ListenerConnection::MessageReceived, [=](const QString& Message) {
            std::cout << Message.toStdString();
        });

    ConnectionThread->start();
    Connection->Init();
    return a.exec();
}


void MessageReceived(const QString& Message) {
    std::cout << Message.toStdString();
}