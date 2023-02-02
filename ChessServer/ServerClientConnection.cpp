#include "ServerClientConnection.h"
#include "ListenerConnection.h"
#include <qdebug.h>

ServerClientConnection::ServerClientConnection()
{
}

void ServerClientConnection::Init(SOCKET InSocket, const QString& InUsername, ListenerConnection* InListener, int InPlayerID)
{
	Socket = InSocket;
	Username = InUsername;
	Listener = InListener;
	PlayerID = InPlayerID;
}

void ServerClientConnection::Start()
{
	int Result;

	int SendResult;
	char recvbuf[512];
	int recvbuflen = 512;

	const char* one = "1";

	QString JoinStr = ConnectionStatics::PrepareString("msg", QString("%1 joined").arg(Username));
	Listener->SendMulticastMessage(JoinStr);

	QString PidStr = ConnectionStatics::PrepareString("pid", QString::number(PlayerID));
	send(Socket, qPrintable(PidStr), PidStr.size(), 0);

	while (true) {
		Result = recv(Socket, recvbuf, recvbuflen, 0);
		
		if (Result > 0) {
			if (recvbuf[0] == '1') {
				send(Socket, one, sizeof(one), 0);
				continue;
			}

			QString ReceivedString = QString(recvbuf);
			MsgData Data = ConnectionStatics::ResolveString(ReceivedString);

			if (ReceivedString.contains("msg;")) {
				QString Msg = ConnectionStatics::PrepareString("msg", QString("%1: %2").arg(Username).arg(Data.String));
				
				Listener->SendMulticastMessage(Msg);
			}

			if (ReceivedString.contains("pm;")) {
				Listener->SendMulticastMessage(ReceivedString);
			}
		}
	}
}

void ServerClientConnection::SendMsg(const QString& Msg)
{
	emit MessageReceived(Msg);
}
