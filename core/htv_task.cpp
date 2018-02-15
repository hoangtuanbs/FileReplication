#include "abb_task.h"
#include <QTimer>
#include <QDateTime>
#include <QMutex>
#include <QLinkedList>
#include <QMultiMap>
#include "abb_workerthread.h"
C_Task::C_Task(const QString &name,const QDateTime &executeTime, const quint32 &repeatInterval) :
	m_ExecuteTime(executeTime),
	m_Status(t_TaskStatusWaiting),
	m_RepeatInterval(repeatInterval),
	m_Name(name)
{
}

bool C_Task::M_IsExpire()
{
	return QDateTime() > m_ExecuteTime;
}
void	C_Task::M_QueueNext()
{
	m_ExecuteTime.addMSecs(m_RepeatInterval);
}

C_TaskFactory::C_TaskFactory(const quint16 scanIntervalMs, const quint16 maxThreadCount) :
	C_WorkerThread( maxThreadCount, scanIntervalMs)
{
	m_MaxThreadCount = maxThreadCount;
}

void C_TaskFactory::M_ThreadRoutine()
{
	C_Task* task = 0;
	do
	{	// executing tasks
		task = M_GetAvailableTask();
		if (task)
		{
			bool ret = task->M_Execute();
			if (ret)
			{
				task->M_SetStatus(C_Task::t_TaskStatusFinished);
			} else
			{
				task->M_SetStatus(C_Task::t_TaskStatusQueueNext);
			}
		}
	} while (task);

	QLinkedList<C_Task*>::iterator eraser = m_Tasks.end();

	// erasing completed tasks
	QMutexLocker lock(&m_TaskLock);
	for (QLinkedList<C_Task*>::iterator iter = m_Tasks.begin(); iter != m_Tasks.end(); iter++)
	{
		if (eraser != m_Tasks.end())
		{
			C_Task * task = *eraser;
			m_Tasks.erase(eraser);
			delete task;
			eraser = m_Tasks.end();
		}
		if ((*iter)->M_GetStatus() == C_Task::t_TaskStatusFinished)
		{
			eraser = iter;
		} else if ((*iter)->M_GetStatus() == C_Task::t_TaskStatusQueueNext)
		{
			(*iter)->M_QueueNext();
		}
	}
	if (eraser != m_Tasks.end())
	{
		C_Task * task = *eraser;
		m_Tasks.erase(eraser);
		delete task;
	}
}


C_Task* C_TaskFactory::M_GetAvailableTask()
{
	QMutexLocker locker(&m_TaskLock);
	for (QLinkedList<C_Task*>::iterator iter = m_Tasks.begin(); iter != m_Tasks.end(); iter++)
	{
		C_Task* ret = *iter;
		if (ret->M_GetStatus() == C_Task::t_TaskStatusWaiting && ret->M_IsExpire())
		{
			ret->M_SetStatus(C_Task::t_TaskStatusExecuting);
			return ret;
		}
	}
	return 0;
}
void	C_TaskFactory::M_AddTask(C_Task* task)
{
	QMutexLocker locker(&m_TaskLock);
	if (task->M_GetStatus() == C_Task::t_TaskStatusWaiting)
	m_Tasks.append(task);
}

QLinkedList<C_Task*>	C_TaskFactory::M_GetTasks(const QString &taskname)
{
	QLinkedList<C_Task*> ret;
	QMutexLocker locker(&m_TaskLock);
	for (QLinkedList<C_Task*>::iterator iter = m_Tasks.begin(); iter != m_Tasks.end(); iter++)
	{
		C_Task* task = *iter;
		if (task->M_GetName().compare(taskname, Qt::CaseInsensitive) == 0)
		{
			ret.append(task);
		}
	}
	return ret;
}
