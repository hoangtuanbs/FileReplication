#include "abb_replicationclient.h"
#include "abb_replicationconnection.h"
#include "core/htv_config.h"

C_ReplicationClient::C_ReplicationClient(QObject *parent) :
	 QObject(parent),
	m_Server(C_Config::M_GetServerPort(), this),
	m_FileServer(C_Config::M_GetReplicationPort(), this)
{
	connect(&m_Server, SIGNAL(S_NewConnection(C_ReplicationConnection*)), this, SLOT(slt_NewConnection(C_ReplicationConnection*)));
	C_ReplicationManagerConfig *replicationConfig = C_ReplicationManagerConfig::getInstance();
	QList<C_Host*> otherhosts = replicationConfig->M_GetReplicationHosts();
	C_Host *host;
	foreach (host, otherhosts)
	{
		C_ReplicationConnection *connection = new C_ReplicationConnection(this);
		connection->connectToHost(host->M_GetHostAddress(), C_Config::M_GetServerPort());
		slt_NewConnection(connection);

		//C_ReplicationConnection *fileconnection = new C_ReplicationConnection(this);
		//fileconnection->connectToHost(host->M_GetHostAddress(), C_Config::M_GetReplicationPort());
		//slt_NewFileConnection(fileconnection);
	}
}
void C_ReplicationClient::slt_NewConnection(C_ReplicationConnection *connection)
{
	if (!M_HasConnection(connection->peerAddress()))
	{
		connect(connection, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(slt_ConnectionError(QAbstractSocket::SocketError)));
		connect(connection, SIGNAL(disconnected()), this, SLOT(slt_Disconnected()));
		connect(connection, SIGNAL(S_ReadyForUse(C_ReplicationConnection*)), this, SLOT(slt_ReadyForUse(C_ReplicationConnection*)));
	} else
	{
		connection->deleteLater();
	}
}

void C_ReplicationClient::slt_NewFileConnection(C_ReplicationConnection *connection)
{
	connect(connection, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(slt_ConnectionError(QAbstractSocket::SocketError)));
	connect(connection, SIGNAL(disconnected()), this, SLOT(slt_Disconnected()));
	connect(connection, SIGNAL(S_ReadyForUse(C_ReplicationConnection*)), this, SLOT(slt_ReadyForUse(C_ReplicationConnection*)));
}

void C_ReplicationClient::slt_ConnectionError(QAbstractSocket::SocketError socketError)
{
	qDebug()<<QDateTime::currentDateTimeUtc()<<" (C_ReplicationClient): There is errors with the connection";
}

void C_ReplicationClient::slt_Disconnected()
{
	if (C_ReplicationConnection* connection = qobject_cast<C_ReplicationConnection*>(sender()))
	{
		qDebug()<<QDateTime::currentDateTimeUtc()<<" (C_ReplicationClient): Connection to " << connection->peerAddress() <<" disconnected";
		//connection->connectToHost(connection->peerAddress(), C_Config::M_GetServerPort());
		M_RemoveConnection(connection);
		/*C_ReplicationConnection *newconnection = new C_ReplicationConnection(this);
		newconnection->connectToHost(connection->peerAddress(), C_Config::M_GetServerPort());
		slt_NewConnection(newconnection);*/
	}
}
void C_ReplicationClient::slt_FileConnectionDisconnected()
{
	if (C_ReplicationConnection* connection = qobject_cast<C_ReplicationConnection*>(sender()))
	{
		qDebug()<<QDateTime::currentDateTimeUtc()<<" (C_ReplicationClient): Connection to " << connection->peerAddress() <<" disconnected";
		connection->connectToHost(connection->peerAddress(), C_Config::M_GetReplicationPort());
	}
}

void C_ReplicationClient::slt_ReadyForUse(C_ReplicationConnection *connection)
{
	qDebug()<<QDateTime::currentDateTimeUtc()<<" (C_ReplicationClient): Connection ready to use " << connection->peerAddress();
	connect(connection, SIGNAL(S_NewMessage(C_Message*)), this, SLOT(slt_NewMessage(C_Message*)));
	m_Hosts.insert(connection->peerAddress(), connection);
}

void C_ReplicationClient::slt_FileConnectionReadyForUse(C_ReplicationConnection *connection)
{
	qDebug()<<QDateTime::currentDateTimeUtc()<<" (C_ReplicationClient): File connection ready to use " << connection->peerAddress();
	connect(connection, SIGNAL(S_NewFile(C_Message*)), this, SLOT(slt_NewFile(C_Message*)));
	m_FileConnections.insert(connection->peerAddress(), connection);
}

void C_ReplicationClient::M_BroadcastMessage(C_Message &message)
{
	QList<C_ReplicationConnection*> connections = m_Hosts.values();
	qDebug()<<QDateTime::currentDateTimeUtc()<<" (C_ReplicationClient): Broadcasting message to " << connections.size()<<" hosts.";
	foreach (C_ReplicationConnection *connection, connections)
	{
		connection->M_SendMessage(&message);
	}
}

bool C_ReplicationClient::M_SendMessage(const QHostAddress &toHost, C_Message &message)
{
	QHash<QHostAddress, C_ReplicationConnection*>::iterator iter = m_Hosts.find(toHost);
	if (iter == m_Hosts.end())
		 return false;
	C_ReplicationConnection* connection = iter.value();
	if (connection != 0)
	{
		qDebug()<<QDateTime::currentDateTimeUtc()<<" (C_ReplicationClient): Sending message to " << connection->peerAddress();
		connection->M_SendMessage(&message);
		return true;
	}
	return false;
}

bool C_ReplicationClient::M_HasConnection(const QHostAddress &senderIp) const
{
	return m_Hosts.contains(senderIp);
}

void C_ReplicationClient::M_RemoveConnection(C_ReplicationConnection *connection)
{
	if (M_HasConnection(connection->peerAddress()))
	{
		m_Hosts.remove(connection->peerAddress());
	}
	connection->deleteLater();
}

void C_ReplicationClient::slt_NewMessage(C_Message *message)
{
	if (C_ReplicationConnection* connection = qobject_cast<C_ReplicationConnection*>(sender()))
	{
		QHostAddress address = connection->peerAddress();
		emit S_NewMessage(address, message);
	}
}
void C_ReplicationClient::slt_NewFile(C_Message* message)
{
	if (C_ReplicationConnection* connection = qobject_cast<C_ReplicationConnection*>(sender()))
	{
		QHostAddress address = connection->peerAddress();
		emit S_NewFile(address, message);
	}
}
