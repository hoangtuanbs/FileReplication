#ifndef ABB_REPLICATIONMANAGER_H
#define ABB_REPLICATIONMANAGER_H

#include <QObject>
#include <QtGui>
#include <core/htv_config.h>
#include "core/abb_directorymonitor.h"
#include "core/abb_replicationfile.h"
#include "network/abb_replicationserver.h"
#include "network/abb_replicationconnection.h"
#include "network/abb_replicationclient.h"
#include <QMutex>
#include <QMutexLocker>
#include <QTimer>

/*
class C_ReplicationTask : public QObject
{
	Q_OBJECT
public: enum t_TaskType
{
	t_Deletion,
	t_Rename,
	t_UpdateRemote,
	t_RequestData
};

public: C_ReplicationTask (const QString &fileName, const t_TaskType& task, const quint64 &taskid = 0);
public: const QString		&M_GetFileName() const { return m_FileName; }
public: const t_TaskType	&M_GetTask() const { return m_Task; }
public: const quint64		&M_GetTaskId() const { return m_TaskId; }

public: void	M_Execute();

private: t_TaskType		m_Task;
private: QString			m_FileName;
private: quint64			m_TaskId;

};*/

/******************************************************************************************************
  *
  */
/*
class C_TaskFactory : public QObject
{
	Q_OBJECT
public: C_TaskFactory& M_GetInstance();
public: void M_Insert(C_ReplicationTask* task)
{
		m_Tasks[task->M_GetTaskId()] = task;
}
public: C_ReplicationTask* M_GetTask(const quint64 &taskId)
{
	QMap<quint64, C_ReplicationTask*>::iterator iter = m_Tasks.find(taskId);
	if (iter == m_Tasks.end())
	{
		return 0;
	} else
	{
		return iter.value();
	}
}

private: C_TaskFactory();
private: C_TaskFactory(C_TaskFactory& );
private: void operator=(C_TaskFactory const&);
private:	QMap<quint64, C_ReplicationTask*>	m_Tasks;

};*/

class C_ReplicationManager : public QObject
{
    Q_OBJECT
public:
    explicit C_ReplicationManager(QObject *parent = 0);
signals:
public slots:
private slots:
	void slt_OnFileEvent(C_FileEvent* event);
	void slt_NewConnection(C_ReplicationConnection *connection);
	void slt_BroadcastFileFolderRequest();
	void slt_NewMessage (QHostAddress &fromHost, C_Message *message);
	void slt_NewFile(QHostAddress &fromHost, C_Message *message);
private:
	C_DirectoryMonitor			m_Monitor;
   C_ReplicationManagerConfig *m_ReplicationConfig;
	C_ReplicationClient			m_Client;

	QTimer							m_BroadcastTimer;
};

#endif // ABB_REPLICATIONMANAGER_H
