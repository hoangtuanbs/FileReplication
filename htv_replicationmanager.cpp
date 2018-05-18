#include "htv_replicationmanager.h"
#include "core/htv_config.h"


C_ReplicationManager::C_ReplicationManager(QObject *parent) :
	 QObject(parent),
	m_Client(this)
{
	m_ReplicationConfig = C_ReplicationManagerConfig::getInstance();
	C_ReplicationEntity *entity;
	static ulong flag = FILE_NOTIFY_CHANGE_LAST_WRITE | FILE_NOTIFY_CHANGE_CREATION | FILE_NOTIFY_CHANGE_FILE_NAME;
	C_Host *host = m_ReplicationConfig->M_GetLocalHost();
	foreach (entity, host->M_GetReplicationEntities())
	{
		m_Monitor.M_AddDirectory(entity->M_GetReplication()->M_GetReplicationId(), entity->M_GetEntity(), flag, true);
	}
	connect(&m_Monitor, SIGNAL(M_OnEvent(C_FileEvent*)), this, SLOT(slt_OnFileEvent(C_FileEvent*)));
	connect(&m_BroadcastTimer, SIGNAL(timeout()), this, SLOT(slt_BroadcastFileFolderRequest()));
	connect(&m_Client, SIGNAL(S_NewMessage(QHostAddress&,C_Message*)), this, SLOT(slt_NewMessage(QHostAddress&,C_Message*)));
	connect(&m_Client, SIGNAL(S_NewFile(QHostAddress&,C_Message*)), this, SLOT(slt_NewFile(QHostAddress&,C_Message*)));
	//m_BroadcastTimer.setInterval(1000);
	//m_BroadcastTimer.start();

}

void C_ReplicationManager::slt_OnFileEvent(C_FileEvent *event)
{
	qDebug()<<"There is event on file " << event->M_GetFullPath();
	C_FileSimpleMessage filemessage(event->M_GetDirectory()->M_GetReplicationId(),
											  m_ReplicationConfig->M_GetRelativePath(event->M_GetDirectory()->M_GetReplicationId(), event->M_GetFullPath()),
											  event->M_GetFullPath());
	filemessage.M_ReadDataToBuffer();
	m_Client.M_BroadcastMessage(filemessage);
}

void C_ReplicationManager::slt_NewConnection(C_ReplicationConnection *connection)
{
	qDebug() << "C_Replicationmanager: New connection established";
	//connection->M_SendMessage();
}
void C_ReplicationManager::slt_BroadcastFileFolderRequest()
{
	C_Host *host = m_ReplicationConfig->M_GetLocalHost();
	C_ReplicationEntity *entity;
	foreach (entity, host->M_GetReplicationEntities())
	{
		C_FileListRequest message(entity->M_GetReplication()->M_GetReplicationId());
		m_Client.M_BroadcastMessage(message);
	}
}
void C_ReplicationManager::slt_NewMessage (QHostAddress &fromHost, C_Message *message)
{

	switch(message->M_GetMessageType())
	{
		case C_Message::t_FileSimpleMessage:
		{
			C_FileSimpleMessage *simplefile = (C_FileSimpleMessage*) message;
			qDebug() << QDateTime::currentDateTimeUtc()<<"C_Replicationmanager: New file received from host " << fromHost.toString() << simplefile->M_GetFileName();
			QString path = m_ReplicationConfig->M_GetFullPath(simplefile->M_GetReplicationId(), simplefile->M_GetRelativePath());
			m_Monitor.M_SilenceFile(path);
			QFile file(path);
			if (!file.open(QIODevice::WriteOnly)) return;
			QDataStream out(&file);
			out.writeRawData(simplefile->M_GetByteArray().data(), simplefile->M_GetByteArray().size()) ;
			file.flush();
			file.close();
			m_Monitor.M_RemoveSilencedFile(path);
		}
		break;
		case C_Message::t_FileAction:
		{
			C_FileAction *fileaction = (C_FileAction*) message;
			qDebug() << QDateTime::currentDateTimeUtc()<<"C_Replicationmanager: New action message from host " << fromHost.toString()<<": " <<fileaction->M_GetPath();
		}
		break;
		case C_Message::t_FileListRequest:
		{
			C_FileListRequest *filelistrequest = (C_FileListRequest *) message;
			qDebug() << QDateTime::currentDateTimeUtc()<<"C_Replicationmanager: New file lists request from host " << fromHost.toString()<<" replicationid "<<filelistrequest->M_GetReplicationId();
		}
		break;
	default:
		break;
	}
}

void C_ReplicationManager::slt_NewFile(QHostAddress &fromHost, C_Message *message)
{
	switch(message->M_GetMessageType())
	{
		case C_Message::t_FileSimpleMessage:
		{
			C_FileSimpleMessage *simplefile = (C_FileSimpleMessage*) message;
			qDebug() << QDateTime::currentDateTimeUtc()<<"C_Replicationmanager: New file received from host " << fromHost.toString() << simplefile->M_GetFileName();
			QString path = m_ReplicationConfig->M_GetFullPath(simplefile->M_GetReplicationId(), simplefile->M_GetRelativePath());
			QFile file(path);
			QDataStream out(&file);
			out << simplefile->M_GetByteArray();
			file.flush();
			file.close();
		}
		break;
		case C_Message::t_FileListRequest:
		{
			C_FileListRequest *filelistrequest = (C_FileListRequest *) message;
			qDebug() << QDateTime::currentDateTimeUtc()<<"C_Replicationmanager: New file lists request from host " << fromHost.toString()<<" replicationid "<<filelistrequest->M_GetReplicationId();
		}
		break;
	default:
		break;
	}
}

/*C_ReplicationTask::C_ReplicationTask(const QString &fileName, const t_TaskType& task, const quint64 &taskId):
	m_Task(task),
	m_FileName(fileName)

{
	static QMutex	lock;
	lock.tryLock();
	static quint64 taskid = 0;
	m_TaskId = (taskId) ? (taskId) : (++taskid);
	lock.unlock();
}*/
/*
C_TaskFactory::C_TaskFactory(const quint16 maxThreadCount):
	m_MaxThreadCount(maxThreadCount)
{
	m_Tasks.clear();
}

C_TaskFactory &C_TaskFactory::M_GetInstance()
{
	static C_TaskFactory instance;
	return instance;
}*/

