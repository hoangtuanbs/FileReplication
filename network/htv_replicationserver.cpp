#include "abb_replicationserver.h"
#include "abb_replicationconnection.h"
#include <QtNetwork>

C_ReplicationServer::C_ReplicationServer(quint16 port, QObject *parent) :
	 QTcpServer(parent)
{
	listen(QHostAddress::Any, port);
}

void C_ReplicationServer::incomingConnection(int socketDescriptor)
{
	C_ReplicationConnection *connection = new C_ReplicationConnection(this);
	connection->setSocketDescriptor(socketDescriptor);
	QObject::connect(connection, SIGNAL(disconnected()), connection, SLOT(deleteLater()));
	emit S_NewConnection(connection);
}
