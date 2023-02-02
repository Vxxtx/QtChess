#include "ListenerConnection.h"

#undef UNICODE

#define WIN32_LEAN_ANDM_MEAN


#include <WinSock2.h>
#include <WS2tcpip.h>
#include <stdlib.h>
#include <stdio.h>
#include <qthread.h>

#pragma comment (lib, "Ws2_32.lib")

#define DEFAULT_BUFLEN 512
#define DEFAULT_PORT "27015"

ListenerConnection::ListenerConnection()
{
}

bool ListenerConnection::Init()
{
	SendMsg(QString("Initializing server..\n"));

	WSADATA WsaData;
	int Result;

	SOCKET ListenSocket = INVALID_SOCKET;
	SOCKET ClientSocket = INVALID_SOCKET;

	struct addrinfo* AiResult;
	struct addrinfo Hints;

	int SendResult;
	char recvbuf[DEFAULT_BUFLEN];
	int recvbuflen = DEFAULT_BUFLEN;

	Result = WSAStartup(MAKEWORD(2, 2), &WsaData);

	if (Result != 0) {
		SendMsg(QString("WSAStartup failed with error %1\n").arg(Result));
		return false;
	}

	ZeroMemory(&Hints, sizeof(Hints));
	Hints.ai_family = AF_INET;
	Hints.ai_socktype = SOCK_STREAM;
	Hints.ai_protocol = IPPROTO_TCP;
	Hints.ai_flags = AI_PASSIVE;

	Result = getaddrinfo(NULL, DEFAULT_PORT, &Hints, &AiResult);

	if (Result != 0) {
		SendMsg(QString("getaddrinfo failed with error %1\n").arg(Result));

		WSACleanup();
		return false;
	}

	ListenSocket = socket(AiResult->ai_family, AiResult->ai_socktype,
		AiResult->ai_protocol);

	if (ListenSocket == INVALID_SOCKET) {
		SendMsg(QString("socket failed with error %1\n").arg(WSAGetLastError()));

		freeaddrinfo(AiResult);
		WSACleanup();
		return false;
	}

	Result = bind(ListenSocket, AiResult->ai_addr, (int)AiResult->ai_addrlen);

	if (Result == SOCKET_ERROR) {
		SendMsg(QString("bind failed with error %1\n").arg(WSAGetLastError()));

		freeaddrinfo(AiResult);
		closesocket(ListenSocket);
		WSACleanup();
		return false;
	}

	freeaddrinfo(AiResult);

	Result = listen(ListenSocket, SOMAXCONN);

	if (Result == SOCKET_ERROR) {
		SendMsg(QString("listen failed with error %1\n").arg(WSAGetLastError()));

		closesocket(ListenSocket);
		WSACleanup();
		return false;
	}

	SendMsg(QString("Waiting for clients.\n"));

	ClientSocket = accept(ListenSocket, NULL, NULL);

	if (ClientSocket == INVALID_SOCKET) {
		SendMsg(QString("accept failed with error %1\n").arg(WSAGetLastError()));

		closesocket(ListenSocket);
		WSACleanup();
		return false;
	}

	//closesocket(ListenSocket);

	const char* one = "1";

	while (true) {
		Result = recv(ClientSocket, recvbuf, recvbuflen, 0);

		if (Result > 0) {
			QString Username = QString::fromUtf8(recvbuf);
			SendMsg(QString("%1 joined\n").arg(Username));

			SendResult = send(ClientSocket, one, sizeof(one), 0);

			if (SendResult == SOCKET_ERROR) {
				SendMsg(QString("send failed with error: %1\n").arg(WSAGetLastError()));
				closesocket(ClientSocket);
				WSACleanup();
				return false;
			}

			QThread* NewThread = new QThread();
			ServerClientConnection* NewConnection = new ServerClientConnection();
			NewConnection->Init(ClientSocket, Username, this, Connections.size());
			NewConnection->moveToThread(NewThread);

			connect(NewThread, &QThread::started, NewConnection, &ServerClientConnection::Start);
			connect(NewConnection, &ServerClientConnection::MessageReceived, this, &ListenerConnection::MessageReceived);

			Connections.push_back(NewConnection);
			NewThread->start();
			break;
		}
	}

	closesocket(ListenSocket);
	Init();

	/*Result = shutdown(ClientSocket, SD_SEND);

	if (Result == SOCKET_ERROR) {
		SendMsg(QString("shutdown failed, error: %1\n").arg(WSAGetLastError()));
		closesocket(ClientSocket);
		WSACleanup();
		return false;
	}

	closesocket(ClientSocket);
	WSACleanup();*/
	return true;
}

void ListenerConnection::SendMulticastMessage(const QString& Message)
{
	for (int i = 0; i < Connections.size(); i++) {
		int SendResult = send(Connections[i]->GetSocket(), qPrintable(Message), Message.size(), 0);

		if (SendResult == SOCKET_ERROR) {
			SendMsg(QString("send failed with error: %1\n").arg(WSAGetLastError()));
			return;
		}
	}
}

void ListenerConnection::SendMsg(const QString& Message)
{
	emit MessageReceived(Message);
}
