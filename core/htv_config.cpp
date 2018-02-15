#include "htv_config.h"
#include <QHostInfo>

static const quint16 m_kDefaultReplicationPort   = 1705;
static const quint16 m_kDefaultServerPort        = 1704;
C_Config::C_Config()
{
}
quint16 C_Config::M_GetReplicationPort()
{
    return m_kDefaultReplicationPort;
}

quint16 C_Config::M_GetServerPort()
{
    return m_kDefaultServerPort;
}
C_Host::C_Host (QString hostName, QString hostAddress) : m_HostAddress(hostAddress), m_HostName(hostName)
{

}
QString	C_ReplicationManagerConfig::M_GetRelativePath(const QString &replicationId, const QString &fullPath)
{
	C_Host *host = M_GetLocalHost();
	QList<C_ReplicationEntity*> entities = host->M_GetReplicationEntities();
	C_ReplicationEntity* entity;
	foreach (entity, entities)
	{
		if (entity->M_GetReplication()->M_GetReplicationId().compare(replicationId) == 0)
		{
			return fullPath.mid(entity->M_GetEntity().size()+1);
		}
	}
	return "";
}
QString	C_ReplicationManagerConfig::M_GetFullPath(const QString &replicationId, const QString &relativePath)
{
	C_Host *host = M_GetLocalHost();
	QList<C_ReplicationEntity*> entities = host->M_GetReplicationEntities();
	C_ReplicationEntity* entity;
	foreach (entity, entities)
	{
		if (entity->M_GetReplication()->M_GetReplicationId().compare(replicationId) == 0)
		{
			return entity->M_GetEntity() + "\\" + relativePath;
		}
	}
	return "";
}

C_Replication *C_ReplicationManagerConfig::M_GetReplicationEntityFromHost(const QString& hostname, const QString& path)
{
   C_Host *host = this->M_GetHost(hostname);
   C_ReplicationEntity *entity;
   foreach (entity, host->M_GetReplicationEntities())
   {
      if (path.indexOf(entity->M_GetEntity(), 0, Qt::CaseInsensitive) == 0)
      {
         return entity->M_GetReplication();
      };
   };
	return 0;
}
C_ReplicationManagerConfig::C_ReplicationManagerConfig()
{
	//QHostInfo hostinfo;
	m_CurrentHost = QHostInfo::localHostName();
	QString hostnamea = "HTV-PC";
	QString hostnameb = "TuanMini-PC";

	C_Host   *hosta = new C_Host(hostnamea, "192.168.1.234");
	C_Host   *hostb = new C_Host(hostnameb, "192.168.1.201");
	m_Hosts[hostnamea] = hosta;
	m_Hosts[hostnameb] = hostb;

	C_ReplicationEntity  *repa = new C_ReplicationEntity(hosta, "D:\\Replication");
	C_ReplicationEntity  *repb = new C_ReplicationEntity(hostb, "D:\\Replication");

   QList<C_ReplicationEntity*> replications;
   replications.append(repa);
   replications.append(repb);

	C_Replication *replication = new C_Replication("Replication", replications);
   m_Replications.append(replication);
}

C_ReplicationEntity::C_ReplicationEntity(C_Host* host, QString entity) : m_Entity(entity)
{
   m_Host = host;
   m_Host->M_AddReplicationEntity(this);
}

C_Replication::C_Replication (QString replicationId, QList<C_ReplicationEntity*> &replications)
{
   m_ReplicationId = replicationId;
   C_ReplicationEntity* entity;
   foreach (entity, replications)
   {
      m_Replications.append(entity);
      entity->M_GetHost()->M_AddReplication(this);
      entity->M_SetReplication(this);
   }
}
