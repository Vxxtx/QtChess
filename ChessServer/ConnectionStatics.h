#pragma once

#include <qstring.h>
#include <qstringlist.h>

enum class EMsgType {
	msg, pj
};

struct MsgData {
	QString Header;
	QString String;
	int Length;
};

class ConnectionStatics {

public:
	// Prepares string with header and string's length
	static QString PrepareString(const QString& Header, const QString& Str);
	static MsgData ResolveString(const QString& Str);
};