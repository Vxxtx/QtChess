#pragma once

#define WIN32_LEAN_AND_MEAN

#include <winsock2.h>
#include <ws2tcpip.h>

#include <qobject.h>

#pragma comment (lib, "Ws2_32.lib")
#pragma comment (lib, "Mswsock.lib")
#pragma comment (lib, "AdvApi32.lib")

class ListenerConnection;

class ServerClientConnection : public QObject
{
	Q_OBJECT

public:
	ServerClientConnection();

	void Init(SOCKET InSocket, const QString& InUsername, ListenerConnection* InListener);
	void Start();

signals:
	void MessageReceived(const QString& Message);

private:
	SOCKET Socket {INVALID_SOCKET};
	QString Username;
	ListenerConnection* Listener {nullptr};

	void SendMsg(const QString& Msg);

public:
	inline SOCKET GetSocket() { return Socket; };
};

