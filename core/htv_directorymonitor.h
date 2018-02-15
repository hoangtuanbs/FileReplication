#ifndef HTV_DIRECTORYMONITOR_H
#define HTV_DIRECTORYMONITOR_H

#include <QtGui>
#include <windows.h>
/*===========================================================================

CSCommon_DirectoryMonitor.h
CSCommon - Directory Monitoring
(c) HTV

Description:

   Routines to watch directories changes.

History:

   02-JAN-2012/Tuan Vu
      Original

===========================================================================*/

#include <list>
#include <map>
#include <stack>
#include <QFile>
class C_FileEvent;
class C_DirectoryMonitor;
class C_ReplicationDirectory;
class QFileSystemWatcher;
/**********************************************************************************************//**
 * @class	C_IDirectoryEventListener
 *
 * @brief	Directory event listener.
 *
 * @author	Fituvu
 * @date	1/13/2012
 **************************************************************************************************/


class C_File : public QFileInfo
{
public: static const quint16 m_k_sMaxFileNameLength = 1024;
public: static const quint16 m_k_sSafeFileNameLength = 1000;
private:
   C_ReplicationDirectory *m_ReplicationDirectory;
public:
   C_File(const QString &filename, C_ReplicationDirectory *replicationDirectory = 0):
      QFileInfo(filename)
   {
      m_ReplicationDirectory = replicationDirectory;
   }
   C_File(QFileInfo &file, C_ReplicationDirectory *replicationDirectory = 0):
      QFileInfo(file)
   {
      m_ReplicationDirectory =replicationDirectory;
   }
   static C_File *M_S_GetFileFromPath(const QString & path)
   {
      QFileInfo fileInfo(path);
      if (fileInfo.isFile())
      {
         return new C_File(fileInfo);
      } else
      {
         return 0;
      }

   }
   C_ReplicationDirectory *M_GetReplicationDirectory() { return m_ReplicationDirectory; }
};

/**********************************************************************************************//**
 * @class	C_ReplicationDirectory
 *
 * @brief	Directory.
 *
 * @author	Fituvu
 * @date	1/13/2012
 **************************************************************************************************/

class C_ReplicationDirectory : QObject
{
	Q_OBJECT
   private: C_DirectoryMonitor   *m_Parent;
   private: QString              m_ReplicationId;
   private: QString              m_DirectoryName;
   private: HANDLE               m_Handle;
   private: OVERLAPPED           m_Overlapped;
   private: quint8               *m_Buffer;
   private: BOOL                 m_Subdirectory;
   private: DWORD                m_Status;
   private: DWORD                m_Flag;

   public: explicit C_ReplicationDirectory(QString directoryName, DWORD flag, BOOL subdirectory = FALSE, C_DirectoryMonitor* parent = 0);
   public: explicit C_ReplicationDirectory(QString replicationId, QString directoryName, DWORD flag, BOOL subdirectory = FALSE, C_DirectoryMonitor* parent = 0);
   public: ~C_ReplicationDirectory();

   public: HANDLE                &M_GetHandle();
   public: LPOVERLAPPED          M_GetOverlapped ();
   public: const QString         &M_GetDirectoryPath();
   public: const QString         &M_GetReplicationId();
   protected: void               M_ParseBuffer(char* buffer);
   protected: bool               M_ReadDirectory();
   public: bool                  M_StopAllOperations();
   public: static void CALLBACK  M_DirectoryChangesCallback(DWORD errorCode, DWORD nBytes, LPOVERLAPPED pOverlapped);

signals:
   void M_OnEvent (C_FileEvent *);
   friend class C_DirectoryMonitor;
};

/**********************************************************************************************//**
 * @class	C_FileEvent
 *
 * @brief	File event.
 *
 * @author	Fituvu
 * @date	1/13/2012
 **************************************************************************************************/

class C_FileEvent : public QObject
{
	Q_OBJECT
   public: enum t_FileEventCode {
      t_FileEventRenamed,
      t_FileEventDeleted,
      t_FileEventModified,
      t_FileEventAdded
   };
   protected:	QDateTime               m_TimeStamp;
   protected:	QString                 m_FileName;
   protected:	t_FileEventCode         m_Action;
   protected:  C_ReplicationDirectory  *m_Directory;
   protected:  QString                 m_FullPath;

   public: explicit C_FileEvent(C_ReplicationDirectory *directory, PFILE_NOTIFY_INFORMATION &eventInfo) ;

   public: virtual const QString	&M_GetFileName();
   public: virtual const QString	&M_GetFullPath();
   public: C_ReplicationDirectory				*M_GetDirectory();
   public: virtual const t_FileEventCode		&M_GetAction();
   public: virtual ~C_FileEvent(){ }
   public: static C_FileEvent						*M_GetEvent(PFILE_NOTIFY_INFORMATION & pBuffer);
   public: void										M_SetFileName( const QString &filename);
   public: void										M_SetAction(const t_FileEventCode &code);
   public: const QDateTime 						&M_GetTime() { return m_TimeStamp;}
   public: bool										M_IsFireable();
};

/**********************************************************************************************//**
 * @class	C_FileRenamedEvent
 *
 * @brief	File renamed event.
 *
 * @author	Fituvu
 * @date	1/13/2012
 **************************************************************************************************/

class C_FileRenamedEvent : public C_FileEvent
{
   private: QString	m_OldFileName;
   public: explicit C_FileRenamedEvent(C_ReplicationDirectory *directory, PFILE_NOTIFY_INFORMATION &eventInfo);
	public: const	QString &M_GetOldFileName();
	public:			QString &M_GetOldFullPath();
};

/**********************************************************************************************//**
 * @class	C_DirectoryMonitor
 *
 * @brief	Directory monitor.
 *
 * @author	Fituvu
 * @date	1/13/2012
 **************************************************************************************************/
typedef std::list<C_ReplicationDirectory*>	t_DirectoriesList;
typedef C_FileEvent                          *t_FileEventPtr;
class C_FileEventDispatcher : public QThread
{
	Q_OBJECT
	typedef std::pair<QString, C_FileEvent*>				t_EventPair;
	typedef QMap<QString, C_FileEvent*> t_EventsMap;
	typedef t_EventsMap::iterator											t_EventsMapIterator;
	typedef std::list<C_FileEvent*>		t_EventsList;

	private: bool					m_Running;
	protected: t_EventsMap		m_Events;
	protected: t_EventsList		m_EventsList;
	private: QMutex            m_ThreadConcurrencyLock;
	private: HANDLE            m_Thread;
	public: explicit C_FileEventDispatcher();
	public: virtual ~C_FileEventDispatcher();

	public: bool		M_Add(const t_FileEventPtr &event);
	public: void		M_DispatchEvents();
	public: void		M_Stop();
	public: HANDLE		&M_GetHandle() { return m_Thread;}
	protected: void						M_StartInternal();
	protected: void						M_StopInternal();
	protected: static DWORD WINAPI	M_ThreadRoutine (void *ptr);
	protected: static void CALLBACK	M_StopCallback(ULONG_PTR ptr);
signals:
	void M_OnEvent(C_FileEvent*);
};
class C_DirectoryMonitor : public QObject
{
	Q_OBJECT

   public:

   // event worker thread
   // sleep until it has something to do
   // from APC
   private: class C_FileEventCollector: QThread
   {
		private: bool		m_Running;
		private: HANDLE	m_Thread;
      public: explicit C_FileEventCollector();
      public: virtual ~C_FileEventCollector();
      // Put the thread in alertable state
      protected: void	M_StartCallback();
      public: void		M_SetState(bool run);
      public: void		M_Start();
      public: void		M_Stop();
		public: HANDLE &M_GetHandle() { return m_Thread;}
      protected: static void CALLBACK M_StopCallback(ULONG_PTR ptr);
		private: static DWORD WINAPI M_ThreadRoutine (void *arg);
      friend class C_DirectoryMonitor;
   };


   /**********************************************************************************************//**
    * @struct	t_EventCallback
    *
    * @brief	Event callback.
    *
    * @author	Fituvu
    * @date	1/13/2012
    **************************************************************************************************/

   typedef struct t_EventCallback
   {
      C_FileEventDispatcher	*m_Thread;
      t_FileEventPtr				m_Event;
   } *t_EventCallbackPtr;

   private: C_FileEventCollector			m_EventCollector;
   private: C_FileEventDispatcher		m_EventDispatcher;
   private: t_DirectoriesList				m_Directories;
   private: QMap<QString, ulong>       m_DirectoryMap;
   private: QMap<QString, quint8>      m_EventSilencer;

   public: explicit C_DirectoryMonitor();
   public: virtual ~C_DirectoryMonitor(void);

   public: const t_DirectoriesList	&M_GetDirectoriesList();
   public: bool							M_AddDirectory(C_ReplicationDirectory *directory);
   public: bool							M_AddDirectory(const QString &replicationId, const QString &directoryName, ulong watchFlag, bool subdirectory = FALSE);
   public: bool							M_AddDirectory(const QString &directoryName, ulong watchFlag, bool subdirectory = FALSE);
   public: bool							M_RemoveDirectory(const QString &directoryName);
   public: bool							M_SilenceFile(const QString &fileName);
   public: bool							M_IsFileSilenced(const QString &fileName);
   public: bool							M_RemoveSilencedFile(const QString &fileName);

   // ReadPreviousChangesW requires the caller thread to be in an alertable waiting state
   protected: static void CALLBACK	M_AddDirectoryCallback(ULONG_PTR arg);

protected slots:
	void	slt_OnEvent(C_FileEvent *event);
	void	slt_InternalEvent(C_FileEvent *event);
signals:
	void M_OnEvent(C_FileEvent *);
};


#endif // HTV_DIRECTORYMONITOR_H
