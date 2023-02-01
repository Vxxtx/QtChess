#include "ServerClientConnection.h"
#include "ListenerConnection.h"
#include <qdebug.h>

ServerClientConnection::ServerClientConnection()
{
}

void ServerClientConnection::Init(SOCKET InSocket, const QString& InUsername, ListenerConnection* InListener)
{
	Socket = InSocket;
	Username = InUsername;
	Listener = InListener;
}

void ServerClientConnection::Start()
{
	int Result;

	int SendResult;
	char recvbuf[512];
	int recvbuflen = 512;

	const char* one = "1";

	QString JoinStr = QString("%1;%2 joined").arg(7 + Username.length()).arg(Username);
	Listener->SendMulticastMessage(JoinStr);

	while (true) {
		Result = recv(Socket, recvbuf, recvbuflen, 0);
		
		if (Result > 0) {
			if (recvbuf[0] == '1') {
				send(Socket, one, sizeof(one), 0);
				continue;
			}

			QString FinalString = QString(recvbuf);
			
			if (FinalString.contains("msg;")) {
				std::vector<ServerClientConnection*> Clients;
				Listener->GetClients(Clients);

				QStringList SplitStr = FinalString.split(";");
				QString Msg = QString("%1: %2").arg(Username).arg(SplitStr[2].mid(0, SplitStr[1].toInt()));
				FinalString = QString("%1;%2").arg(Msg.length()).arg(Msg);

				Listener->SendMulticastMessage(FinalString);
			}
		}
	}
}

void ServerClientConnection::SendMsg(const QString& Msg)
{
	emit MessageReceived(Msg);
}
