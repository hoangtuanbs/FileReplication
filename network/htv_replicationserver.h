#ifndef HTV_REPLICATIONSERVER_H
#define HTV_REPLICATIONSERVER_H

#include <QObject>
#include <QTcpServer>

class C_ReplicationConnection;

class C_ReplicationServer : public QTcpServer
{
	Q_OBJECT
public: explicit C_ReplicationServer(quint16 port, QObject *parent = 0);

signals:
	void S_NewConnection(C_ReplicationConnection* );
public slots:
protected:
	void incomingConnection(int socketDescriptor);
};

#endif // htv_REPLICATIONSERVER_H
