#include "abb_replicationconnectionmanager.h"

C_ReplicationConnectionManager::C_ReplicationConnectionManager(QObject *parent) :
    QObject(parent)
{
}

C_ReplicationHost::C_ReplicationHost()
{

}

const QHostAddress	&C_ReplicationHost::M_GetHostAddress()
{
	return m_HostAddress;
}

const QString			&C_ReplicationHost::M_GetHostName()
{
	return m_HostName;
}

const C_ReplicationHost::t_HostStatus	&C_ReplicationHost::M_GetHostStatus()
{
	return m_HostStatus;
}

void	C_ReplicationConnectionManager::M_SetServerPort(quint16 port)
{
	m_Port = port;
}
