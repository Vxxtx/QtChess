#include "ClientConnection.h"

ClientConnection::ClientConnection(const QString& InUsername, QString& isendstr)
    : sendstr(isendstr)
{
    Username = InUsername;
}

void ClientConnection::Init()
{
    WSADATA WsaData;

    struct addrinfo* AiResult = NULL, * Ptr = NULL, Hints;
    const char* Sendbuf = "0";

    char recvbuf[DEFAULT_BUFLEN];
    int recvbuflen = DEFAULT_BUFLEN;
    
    int Result;

    Result = WSAStartup(MAKEWORD(2, 2), &WsaData);

    if (Result != 0) {
        SendMsg(QString("WSAStartup failed with error %1").arg(Result));
        return;
    }

    ZeroMemory(&Hints, sizeof(Hints));
    Hints.ai_family = AF_UNSPEC;
    Hints.ai_socktype = SOCK_STREAM;
    Hints.ai_protocol = IPPROTO_TCP;

    Result = getaddrinfo("127.0.0.1", DEFAULT_PORT, &Hints, &AiResult);

    if (Result != 0) {
        SendMsg(QString("getaddrinfo failed with error %1").arg(Result));
        WSACleanup();
        return;
    }

    for (Ptr = AiResult; Ptr != NULL; Ptr = Ptr->ai_next) {
        ConnectSocket = socket(Ptr->ai_family, Ptr->ai_socktype, Ptr->ai_protocol);
        
        if (ConnectSocket == INVALID_SOCKET) {
            SendMsg(QString("socket failed with error: %1").arg(WSAGetLastError()));
            WSACleanup();
            return;
        }

        Result = ::connect(ConnectSocket, Ptr->ai_addr, (int)Ptr->ai_addrlen);

        if (Result == SOCKET_ERROR) {
            closesocket(ConnectSocket);
            ConnectSocket = INVALID_SOCKET;
            continue;
        }
        break;
    }

    freeaddrinfo(AiResult);

    if (ConnectSocket == INVALID_SOCKET) {
        SendMsg(QString("Unable to connect to server."));
        WSACleanup();
        return;
    }

    Result = send(ConnectSocket, qPrintable(Username), 32, 0);

    if (Result == SOCKET_ERROR) {
        SendMsg(QString("Send failed with err: %1").arg(WSAGetLastError()));
        closesocket(ConnectSocket);
        WSACleanup();
        return;
    }
    
    /*Result = shutdown(ConnectSocket, SD_SEND);

    if (Result == SOCKET_ERROR) {
        SendMsg(QString("Shutdown failed with err: %1").arg(WSAGetLastError()));
        closesocket(ConnectSocket);
        WSACleanup();
        return false;
    }*/

    const char* zero = "1";
    
    while (true) {

        if (sendstr != "") {
            QString ToSend = QString("msg;%1;%2").arg(sendstr.length()).arg(sendstr);
            Result = send(ConnectSocket, qPrintable(ToSend), ToSend.size(), 0);
            sendstr = "";
        }

        Result = recv(ConnectSocket, recvbuf, recvbuflen, 0);

        if (Result > 0) {
            if (QString(recvbuf[0]) == "1") {
                send(ConnectSocket, zero, sizeof(zero), 0);
                continue;
            }
            
            QString Reply = QString::fromUtf8(recvbuf);
            
            if (Reply.contains("msg;")) {
                QString SplitMsg = Reply.split("msg;")[1];
                QStringList SplitMsgData = SplitMsg.split(";");
                QString FinalMsg = QString("%1").arg(SplitMsgData[1].mid(0, SplitMsgData[0].toInt()));

                qDebug() << "1" << SplitMsg;
                SendMsg(FinalMsg);
            }

        }
    }
    
    //closesocket(ConnectSocket);
    //WSACleanup();
}

void ClientConnection::SendMsg(const QString& Message)
{
    emit MessageReceived(Message);
}

void ClientConnection::SendMsgFromClient(const QString& Message)
{

    

    char recvbuf[DEFAULT_BUFLEN];
    int recvbuflen = DEFAULT_BUFLEN;

   if (ConnectSocket == INVALID_SOCKET) {
        SendMsg(QString("Unable to connect to server."));
        WSACleanup();
        return;
    }

    QString MsgFinal = QString("msg;%1").arg(Message);

    int Result = send(ConnectSocket, qPrintable(MsgFinal), (int)MsgFinal.length(), 0);

    if (Result == SOCKET_ERROR) {
        SendMsg(QString("Send failed with err: %1").arg(WSAGetLastError()));
        closesocket(ConnectSocket);
        WSACleanup();
        return;
    }

    /*while (true) {
        Result = recv(ConnectSocket, recvbuf, recvbuflen, 0);

        if (Result > 0) {
            SendMsg(QString("Bytes received: %1").arg(Result));
            break;
        }
        else if (Result == 0) {
            SendMsg(QString("Connection closed"));
        }
        else {
            SendMsg(QString("recv failed with err: %1").arg(WSAGetLastError()));
        }
    }
    SendMsg(QString("Sent message"));*/
}