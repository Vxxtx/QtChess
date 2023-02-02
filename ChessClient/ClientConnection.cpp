#include "ClientConnection.h"
#include "ConnectionStatics.h"

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

    const char* one = "1";
    
    while (true) {
        if (sendstr != "") {
            Result = send(ConnectSocket, qPrintable(sendstr), sendstr.size(), 0);
            sendstr = "";
        }

        Result = recv(ConnectSocket, recvbuf, recvbuflen, 0);

        if (Result > 0) {
            if (QString(recvbuf[0]) == "1") {
                send(ConnectSocket, one, sizeof(one), 0);
                continue;
            }
            
            // Remember to send a reply back so the server's socket doesn't get stuck in recv!!!
            
            QString Reply = QString::fromUtf8(recvbuf);
            MsgData Data = ConnectionStatics::ResolveString(Reply);

            if (Reply.contains("msg;")) {
                SendMsg(Data.String);
                send(ConnectSocket, one, sizeof(one), 0);
            }

            if (Reply.contains("pid")) {
                OnPlayerIDGot(PlayerID = Data.String.toInt());
                send(ConnectSocket, one, sizeof(one), 0);
            }

            if (Reply.contains("pm")) {
                QStringList Params = Data.String.split(":");
                emit OnPiecePositionUpdate(Params[0].toInt(), Params[1].toInt(), Params[2].toInt());
                send(ConnectSocket, one, sizeof(one), 0);
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
   if (ConnectSocket == INVALID_SOCKET) {
        SendMsg(QString("Could not reach server."));
        WSACleanup();
        return;
    }

    QString Msg = ConnectionStatics::PrepareString("msg", Message);
    send(ConnectSocket, qPrintable(Msg), Msg.size(), 0);
}

void ClientConnection::SendChessPieceData(int PieceIdx, int X, int Y)
{
    if (ConnectSocket == INVALID_SOCKET) {
        SendMsg(QString("Could not reach server."));
        WSACleanup();
        return;
    }

    QString Msg = ConnectionStatics::PrepareString("pm", QString("%1:%2:%3").arg(PieceIdx).arg(X).arg(Y));
    send(ConnectSocket, qPrintable(Msg), Msg.size(), 0);
}
