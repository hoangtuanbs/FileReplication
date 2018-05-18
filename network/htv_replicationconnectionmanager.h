#ifndef HTV_REPLICATIONCONNECTIONMANAGER_H
#define HTV_REPLICATIONCONNECTIONMANAGER_H

#include <QObject>
#include <QHostAddress>

class C_ReplicationHost
{
	public: enum t_HostStatus
	{
		g_HostStatusInitializing,
		g_HostStatusSynchronizing,
		g_HostStatusReplicating,
		g_HostStatusIdle,
		g_HostStatusDisconnected,
		g_HostStatusError
	};
	public:	C_ReplicationHost();
	public:	const QHostAddress	&M_GetHostAddress();
	public:	const QString			&M_GetHostName();
	public:	const t_HostStatus	&M_GetHostStatus();

	private:	QHostAddress	m_HostAddress;
	private:	quint16			m_EventPort;
	private: quint16			m_ServerPort;
	private:	QString			m_HostName;
	private:	t_HostStatus	m_HostStatus;
};

class C_ReplicationConnectionManager : public QObject
{
    Q_OBJECT
public: explicit	C_ReplicationConnectionManager(QObject *parent = 0);
public: void		M_SetServerPort(quint16 port);
signals:

public slots:
private:	quint16	m_Port;
};

#endif // htv_REPLICATIONCONNECTIONMANAGER_H
