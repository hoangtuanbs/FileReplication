#ifndef HTV_TASK_H
#define HTV_TASK_H
#include <QObject>

#include <QTimer>
#include <QDateTime>
#include <QString>
#include <QMutex>
#include <QMultiMap>
#include <QLinkedList>
#include "htv_workerthread.h"
class C_Task
{
	public: enum t_TaskStatus
	{
		t_TaskStatusWaiting,
		t_TaskStatusExecuting,
		t_TaskStatusFinished,
		t_TaskStatusQueueNext
	};
	public: C_Task(const QString &name, const QDateTime &executeTime, const quint32 &repeatInterval = 100);
	public: virtual bool				M_Execute()	= 0;
	public: virtual bool				M_IsExpire();
	public:	const t_TaskStatus	&M_GetStatus() { return m_Status;}
	public:	void						M_SetStatus (t_TaskStatus status) { m_Status = status; }
	public:	QMutex					&M_GetLock() { return m_Lock; }
	public: const QString			&M_GetName() { return m_Name; }
	public:	void						M_QueueNext();

	private: QDateTime				m_ExecuteTime;
	private: t_TaskStatus			m_Status;
	private: QMutex					m_Lock;
	private:	quint32					m_RepeatInterval;
	private: QString					m_Name;
};

class C_TaskFactory : public C_WorkerThread
{
public: C_TaskFactory(const quint16 scanIntervalMs = 10, const quint16 maxThreadCount = 5);
public: void	M_AddTask(C_Task* );
public: QLinkedList<C_Task*>	M_GetTasks(const QString &taskname);

protected:	virtual void M_ThreadRoutine();
private: C_Task*		M_GetAvailableTask();
private:	QMutex		m_Lock;
private:	QMutex		m_ThreadLock;
private: QMutex		m_TaskLock;

//private: QMultiMap<QString, C_Task*> m_Tasks;
private: QLinkedList<C_Task*>	m_Tasks;
};
#endif // htv_TASK_H
