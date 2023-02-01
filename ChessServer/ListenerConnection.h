#pragma once

#include <qobject.h>
#include "ServerClientConnection.h"
#include <vector>

class ListenerConnection : public QObject
{
	Q_OBJECT

public:
	ListenerConnection();

	bool Init();

	void SendMulticastMessage(const QString& Message);

signals:
	void MessageReceived(const QString& Message);

private:
	void SendMsg(const QString& Message);

	void JoinRequestReceived();

	std::vector<ServerClientConnection*> Connections;

public:
	inline void GetClients(std::vector<ServerClientConnection*>& OutClients) {
		OutClients = Connections;
	};
};

