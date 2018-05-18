#include "htv_workerthread.h"

C_WorkerThread::C_WorkerThread(int maxThreadCount, int ms, QObject *parent) : QObject(parent)
{
	m_MaxThreadCount=(maxThreadCount);
	m_ThreadCount=(0);
	m_Timer.setInterval(ms);
	QObject::connect(&m_Timer, SIGNAL(timeout()), this, SLOT(slt_Timeout()));
}

void C_WorkerThread::M_Start()
{
	m_Running = true;
	m_Timer.start();
}

void C_WorkerThread::M_Stop()
{
	m_Running = false;
	m_Timer.stop();
}

void C_WorkerThread::slt_Timeout()
{
	{
		QMutexLocker locker(&m_ThreadCountLock);
		m_ThreadCount++;
	}
	if (m_ThreadCount < m_MaxThreadCount)
	{
		M_ThreadRoutine();
	}
	{
		QMutexLocker locker(&m_ThreadCountLock);
		m_ThreadCount--;
	}
}
