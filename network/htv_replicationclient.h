#ifndef ABB_REPLICATIONCLIENT_H
#define ABB_REPLICATIONCLIENT_H

#include <QObject>
#include <QAbstractSocket>
#include "abb_replicationserver.h"
#include "abb_replicationconnectionmanager.h"
class C_ReplicationServer;
class QHostAddress;
class C_ReplicationConnection;
class C_Message;

class C_ReplicationClient : public QObject
{
    Q_OBJECT
public:
	explicit C_ReplicationClient( QObject *parent = 0);
	void M_BroadcastMessage(C_Message &message);
	bool M_SendMessage(const QHostAddress &toHost, C_Message &message);
	bool M_HasConnection(const QHostAddress &senderIp) const;

signals:
	void S_NewMessage(QHostAddress &host, C_Message *message);
	void S_NewFile(QHostAddress &host, C_Message *message);
	void S_NewParticipant(const QString &nick);
	void S_ParticipantLeft(const QString &nick);

private slots:
	void slt_NewConnection(C_ReplicationConnection *connection);
	void slt_NewFileConnection(C_ReplicationConnection *connection);
	void slt_ConnectionError(QAbstractSocket::SocketError socketError);
	void slt_Disconnected();
	void slt_FileConnectionDisconnected();
	void slt_ReadyForUse(C_ReplicationConnection *);
	void slt_FileConnectionReadyForUse(C_ReplicationConnection *);
	void slt_NewMessage(C_Message *message);
	void slt_NewFile(C_Message* message);
private:
	void M_RemoveConnection(C_ReplicationConnection *connection);

	C_ReplicationConnectionManager			m_Manager;
	C_ReplicationServer							m_Server;
	C_ReplicationServer							m_FileServer;
	QHash<QHostAddress, C_ReplicationConnection*> m_Hosts;
	QMultiHash<QHostAddress, C_ReplicationConnection*> m_FileConnections;
};

#endif // ABB_REPLICATIONCLIENT_H
