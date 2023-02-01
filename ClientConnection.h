#pragma once

#include <qobject.h>
#include <qcoreapplication.h>

#define WIN32_LEAN_AND_MEAN

#include <winsock2.h>
#include <ws2tcpip.h>

#pragma comment (lib, "Ws2_32.lib")
#pragma comment (lib, "Mswsock.lib")
#pragma comment (lib, "AdvApi32.lib")

#define DEFAULT_BUFLEN 512
#define DEFAULT_PORT "27015"

class ClientConnection : public QObject
{
	Q_OBJECT

public:
	ClientConnection(const QString& InUsername, QString& isendstr);

	QString& sendstr;

public slots:
	void Init();
	void SendMsgFromClient(const QString& Message);

signals:
	void MessageReceived(const QString& Message);

private:
	void SendMsg(const QString& Message);

	SOCKET ConnectSocket = INVALID_SOCKET;
	QString Username;
};