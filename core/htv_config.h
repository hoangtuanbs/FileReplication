#ifndef HTV_CONFIG_H
#define HTV_CONFIG_H
#include <QObject>
#include <QtGui>
class C_Host;
class C_Replication;
class C_ReplicationEntity;

class C_Host
{
public: enum t_HostStatus
	{
		g_HostStatusInitilizing,
		g_HostStatusConnecting,
		g_HostStatusDisconnected,
		g_HostStatusConnected
	};

public:
   C_Host (QString hostName, QString hostAddress);
   void M_AddReplication(C_Replication* replication) { m_Replications.append(replication); }
   void M_AddReplicationEntity(C_ReplicationEntity * entity) { m_ReplicationEntities.append(entity); }
   const QList<C_Replication*> &M_GetReplications() { return m_Replications; }
   const QList<C_ReplicationEntity*> &M_GetReplicationEntities() { return m_ReplicationEntities;}

	const QString			&M_GetHostName() { return m_HostName;}
	const QString			&M_GetHostAddress() { return m_HostAddress;}
	const t_HostStatus	&M_GetStatus()	{ return m_Status;}
	void						M_SetStatus(const t_HostStatus& status) { m_Status = status;}

	/*const QList<C_Replication*> &M_GetReplications() { return m_Replications;}
	const QList<C_ReplicationEntity*> M_GetReplicationEntities() {return*/
private:
	QString		m_HostAddress;
	QString		m_HostName;
	QList<C_Replication*>			m_Replications;
	QList<C_ReplicationEntity*>	m_ReplicationEntities;
	t_HostStatus						m_Status;
   friend class C_Replication;
   friend class C_ReplicationEntity;
};

class C_ReplicationEntity
{
public:
   C_ReplicationEntity(C_Host* host, QString entity);
   C_Host *M_GetHost() { return m_Host;}
   const QString &M_GetEntity() { return m_Entity;}
   void M_SetReplication(C_Replication * replication) { m_Replication = replication;}
   C_Replication *M_GetReplication() { return m_Replication; }
private:
   C_Host * m_Host;
   QString m_Entity;
   C_Replication * m_Replication;
   friend class C_Host;
};

class C_Replication
{
public:
   C_Replication (QString replicationId, QList<C_ReplicationEntity*> &replications);
	QString &M_GetReplicationId() { return m_ReplicationId;}
	const QList<C_ReplicationEntity*> &M_GetReplicationsEntities() { return m_Replications;}
private:
   QString m_ReplicationId;
   QList<C_ReplicationEntity*> m_Replications;
   friend class C_Host;
   friend class C_ReplicationEntity;
};

class C_ReplicationManagerConfig
{
public:
	static C_ReplicationManagerConfig*		getInstance()
	{
		static C_ReplicationManagerConfig    instance;
		return &instance;
	}
public:
	C_Host*			M_GetLocalHost()
	{
		return M_GetHost(m_CurrentHost);
	}
	QList<C_Host*> M_GetReplicationHosts()
	{
		QList<C_Host*> ret;
		ret.clear();
		for (QMap<QString, C_Host*>::iterator iter = m_Hosts.begin(); iter != m_Hosts.end(); iter++)
		{
			C_Host* host = (iter.value());
			if (m_CurrentHost.compare(host->M_GetHostName()) != 0)
			{
				ret.append(host);
			}
		}
		return ret;
	}
	C_Host*			M_GetHost (const QString &hostname)
   {
      C_Host* ret = 0;
      QMap<QString, C_Host*>::const_iterator iter = m_Hosts.find(hostname);
      if (iter != m_Hosts.end())
      {
         ret = *iter;
      }
      return ret;
   }
   C_Replication *M_GetReplicationEntityFromHost(const QString& host, const QString& path);
	QString			M_GetRelativePath(const QString &replicationId, const QString &fullPath);
	QString			M_GetFullPath(const QString &replicationId, const QString &relativePath);
private:
	C_ReplicationManagerConfig();
	C_ReplicationManagerConfig(C_ReplicationManagerConfig const&);	// Don't Implement
	void operator=(C_ReplicationManagerConfig const&);					// Don't implement

	QList<C_Replication*>	m_Replications;
	QMap<QString, C_Host*>	m_Hosts;
	QString						m_CurrentHost;
};

class C_Config
{
public:
    C_Config();
    static quint16 M_GetReplicationPort();
    static quint16 M_GetServerPort();
    static QString M_GetPathFromReplicationId(quint16 replicationid)
    {
			return "D:\\Replication";
    }
};

#endif // HTV_CONFIG_H
