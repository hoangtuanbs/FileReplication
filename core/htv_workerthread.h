#ifndef ABB_WORKERTHREAD_H
#define ABB_WORKERTHREAD_H

#include <QObject>
#include <QTimer>
#include <QMutex>

class C_WorkerThread : public QObject
{
	Q_OBJECT
	public: 	C_WorkerThread(int maxThreadCount, int ms, QObject *parent = 0);
	~C_WorkerThread() {}
	protected: virtual void M_ThreadRoutine() = 0;
	public: void				M_Start();
	public: void				M_Stop();
	private slots: void		slt_Timeout();

	protected: bool		m_Running;
	protected: QTimer		m_Timer;
	protected: quint16	m_ThreadCount;
	protected: quint16	m_MaxThreadCount;
	protected: QMutex		m_ThreadCountLock;
};
#endif // ABB_WORKERTHREAD_H
