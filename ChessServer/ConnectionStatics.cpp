#include "ConnectionStatics.h"

QString ConnectionStatics::PrepareString(const QString& Header, const QString& Str)
{
	// Header first, length second, actual string third
	return QString("%1;%2;%3").arg(Header).arg(Str.length()).arg(Str);
}

MsgData ConnectionStatics::ResolveString(const QString& Str)
{
	MsgData OutMsgData;
	QStringList Split = Str.split(";");

	OutMsgData.Header = Split[0];
	OutMsgData.Length = Split[1].toInt();
	OutMsgData.String = Split[2].mid(0, Split[1].toInt());

	return OutMsgData;
}