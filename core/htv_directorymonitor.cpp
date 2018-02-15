#include "abb_directorymonitor.h"
#include <windows.h>
const quint32     g_StandardBufferSize = 1048576; // 1 MB buffer
static const int	m_k_sBufferSize = 4096;		// buffer size
static quint32		m_sDelayedInterval = 1000; // 30s delayed before dispatching modified events
static quint32    m_k_sMapCleaningInterval = 60; // every 60second, clean the map buffer to save space and memory
static const int	m_k_sEventDispatchingInterval = 100; // 100ms
#include <exception>
/**********************************************************************************************//**
 * @fn	C_Directory::~C_Directory()
 *
 * @brief	Destructor, simply stop monitoring operation.
 *
 * @author	Fituvu
 * @date	1/13/2012
 **************************************************************************************************/

C_ReplicationDirectory::~C_ReplicationDirectory()
{
   M_StopAllOperations();
}

/**********************************************************************************************//**
 * @fn	HANDLE &C_Directory::M_GetHandle()
 *
 * @brief	Gets the handle.
 *
 * @author	Fituvu
 * @date	1/13/2012
 *
 * @return	The handle.
 **************************************************************************************************/

HANDLE &C_ReplicationDirectory::M_GetHandle()
{
   return m_Handle;
}

/**********************************************************************************************//**
 * @fn	LPOVERLAPPED C_Directory::M_GetOverlapped ()
 *
 * @brief	Gets the overlapped struct as defined by Microsoft.
 *
 * @author	Fituvu
 * @date	1/13/2012
 *
 * @return	The overlapped.
 **************************************************************************************************/

LPOVERLAPPED	C_ReplicationDirectory::M_GetOverlapped ()
{
   return &m_Overlapped;
}

/**********************************************************************************************//**
 * @fn	const QString &C_Directory::M_GetDirectoryName()
 *
 * @brief	Gets the directory name.
 *
 * @author	Fituvu
 * @date	1/13/2012
 *
 * @return	The directory name.
 **************************************************************************************************/

const QString		&C_ReplicationDirectory::M_GetDirectoryPath()
{
   return m_DirectoryName;
}

/**********************************************************************************************//**
 * @fn	const QString &C_Directory::M_GetReplicationId()
 *
 * @brief	Gets the replication id.
 *
 * @author	Fituvu
 * @date	1/13/2012
 *
 * @return	The replication id.
 **************************************************************************************************/

const QString		&C_ReplicationDirectory::M_GetReplicationId()
{
   return m_ReplicationId;
}



/**********************************************************************************************//**
 * @fn	bool C_DirectoryMonitor::M_RemoveDirectory(QString directoryName)
 *
 * @brief	Removes the directory described by directoryName.
 *
 * @author	Fituvu
 * @date	1/13/2012
 *
 * @param	directoryName	Pathname of the directory.
 *
 * @return	true if it succeeds, false if it fails.
 **************************************************************************************************/

bool C_DirectoryMonitor::M_RemoveDirectory(const QString &directoryName)
{
   for (t_DirectoriesList::iterator iter = m_Directories.begin(); iter != m_Directories.end(); iter++)
   {
      C_ReplicationDirectory *directory = (*iter);
      if (directoryName.compare(directory->M_GetDirectoryPath()) == 0)
      {
         bool ret = directory->M_StopAllOperations();
         if (ret)
         {
				this->disconnect(directory);
            m_Directories.erase(iter);
            delete directory;
            return true;
         }
         return false;
      }
   }
   return false;
}

/**********************************************************************************************//**
 * @fn	bool C_DirectoryMonitor::M_AddDirectory(QString directoryName, ulong watchFlag,
 * 		bool subdirectory)
 *
 * @brief	Adds a directory.
 * 			The function will queue the add request, therefore, there will be reasonable delay
 * 			processing the request.
 * 			Monitoring of the directory will start immediately after it has been added.
 *
 * @author	Fituvu
 * @date	1/13/2012
 *
 * @param	directoryName	Pathname of the directory.
 * @param	watchFlag	 	The watch flag as defined by Microsoft, see ReadDirectoryChangesW.
 * @param	subdirectory 	true to enable subdirectory look up.
 *
 * @return	true if it succeeds, false if it fails.
 **************************************************************************************************/

bool C_DirectoryMonitor::M_AddDirectory(const QString &directoryName, ulong watchFlag, bool subdirectory)
{
   static QChar slash = '\\';
   static QChar spacewchar = ' ';
   QString directoryname(directoryName);
   while(directoryname.at(directoryname.size()-1) == slash || directoryname.at(directoryname.size()-1) == spacewchar)
   {
      directoryname.remove(directoryname.size()-1,1);
   }
   if (m_DirectoryMap[directoryname])
   {
      M_RemoveDirectory(directoryname);
      m_DirectoryMap[directoryname] = 0;
   }
   C_ReplicationDirectory *directory = new C_ReplicationDirectory(directoryname, watchFlag, subdirectory, this);
	connect(directory, SIGNAL(M_OnEvent(C_FileEvent*)), this, SLOT(slt_OnEvent(C_FileEvent*)));
	QueueUserAPC(M_AddDirectoryCallback, m_EventCollector.M_GetHandle(), (ulong)directory); // ReadDirectoryChanges requires calling thread in waiting alertable state
   return true;
}

/**********************************************************************************************//**
 * @fn	bool C_DirectoryMonitor::M_AddDirectory(const QString &replicationId, const QString &directoryName, ulong watchFlag, bool subdirectory)
 *
 * @brief	Adds a directory.
 * 			The function will queue the add request, therefore, there will be reasonable delay
 * 			processing the request.
 * 			Monitoring of the directory will start immediately after it has been added.
 *
 * @author	Fituvu
 * @date	1/13/2012
 *
 * @param	directoryName	Pathname of the directory.
 * @param	watchFlag	 	The watch flag as defined by Microsoft, see ReadDirectoryChangesW.
 * @param	subdirectory 	true to enable subdirectory look up.
 *
 * @return	true if it succeeds, false if it fails.
 **************************************************************************************************/

bool C_DirectoryMonitor::M_AddDirectory(const QString &replicationId, const QString &directoryName, ulong watchFlag, bool subdirectory)
{
   static QChar slash = '\\';
   static QChar spacewchar = ' ';
   QString directoryname(directoryName);
   while(directoryname.at(directoryname.size()-1) == slash || directoryname.at(directoryname.size()-1) == spacewchar)
   {
      directoryname.remove(directoryname.size()-1,1);
   }
   if (m_DirectoryMap[directoryname])
   {
      M_RemoveDirectory(directoryname);
      m_DirectoryMap[directoryname] = 0;
   }
   C_ReplicationDirectory *directory = new C_ReplicationDirectory(replicationId, directoryname, watchFlag, subdirectory, this);
	connect(directory, SIGNAL(M_OnEvent(C_FileEvent*)), this, SLOT(slt_OnEvent(C_FileEvent*)));
	QueueUserAPC(M_AddDirectoryCallback, m_EventCollector.M_GetHandle(), (ulong)directory); // ReadDirectoryChanges requires calling thread in waiting alertable state
   return true;
}

/**********************************************************************************************//**
 * @fn	void CALLBACK C_DirectoryMonitor::M_AddDirectoryCallback(ULONG_PTR arg)
 *
 * @brief	Adds a directory callback.
 *
 * @author	Fituvu
 * @date	1/13/2012
 *
 * @exception	C_Exception	Thrown when a directory cant be read.
 *
 * @param	arg	The argument.
 *
 **************************************************************************************************/

void CALLBACK C_DirectoryMonitor::M_AddDirectoryCallback(ULONG_PTR arg)
{

   C_ReplicationDirectory *directory = (C_ReplicationDirectory*) arg;
   if (directory->M_ReadDirectory())
   {
      directory->m_Parent->m_Directories.push_back(directory);
      directory->m_Parent->m_DirectoryMap[directory->M_GetDirectoryPath()] = (ulong) directory;
   } else
   {
      delete directory;
      throw std::exception();
   }
}

/**********************************************************************************************//**
 * @fn	C_DirectoryMonitor::C_DirectoryMonitor(C_IDirectoryEventListener *listener)
 *
 * @brief	Constructor.
 *
 * @author	Fituvu
 * @date	1/13/2012
 *
 * @param [in]	listener	If non-null, the listener.
 **************************************************************************************************/

C_DirectoryMonitor::C_DirectoryMonitor()
{
	connect(&m_EventDispatcher, SIGNAL(M_OnEvent(C_FileEvent*)), this, SLOT(slt_InternalEvent(C_FileEvent*)));
}

/**********************************************************************************************//**
 * @fn	C_ReplicationDirectory::C_ReplicationDirectory(QString directoryName, DWORD flag, BOOL subdirectory,
 * 		C_DirectoryMonitor* parent)
 *
 * @brief	Constructor.
 *
 * @author	Fituvu
 * @date	1/13/2012
 *
 * @exception	C_Exception	Thrown when the directory cant be handled.
 *
 * @param	directoryName  	Pathname of the directory.
 * @param	flag					The flag.
 * @param	subdirectory		true to enable subdirectory look-up.
 * @param [in,out]	parent	If non-null, the parent.
 **************************************************************************************************/

C_ReplicationDirectory::C_ReplicationDirectory(QString directoryName, DWORD flag, BOOL subdirectory, C_DirectoryMonitor* parent) : m_Parent(parent),  m_ReplicationId(""), m_DirectoryName(directoryName)
{
   m_Buffer = new quint8[m_k_sBufferSize];
   m_Handle = 0;
   m_Flag = flag;
   m_Subdirectory = subdirectory;
   ZeroMemory(&m_Overlapped, sizeof(m_Overlapped));	// clear it
   m_Overlapped.hEvent = this;	// to get this object returned from the callback

   m_Handle = CreateFile(directoryName.toStdWString().c_str(), // create a handle to the given directory
      FILE_LIST_DIRECTORY,
      FILE_SHARE_READ | FILE_SHARE_WRITE,
      NULL,
      OPEN_EXISTING,
      FILE_FLAG_OVERLAPPED | FILE_FLAG_BACKUP_SEMANTICS ,
      NULL);
   if (m_Handle == INVALID_HANDLE_VALUE)
   {
	  // ulong errorcode = GetLastError();
      throw std::exception();
   }
}

/**********************************************************************************************//**
 * @fn	C_ReplicationDirectory::C_ReplicationDirectory(QString directoryName, DWORD flag, BOOL subdirectory,
 * 		C_DirectoryMonitor* parent)
 *
 * @brief	Constructor.
 *
 * @author	Fituvu
 * @date	1/13/2012
 *
 * @exception	C_Exception	Thrown when the directory cant be handled.
 *
 * @param	directoryName  	Pathname of the directory.
 * @param	flag					The flag.
 * @param	subdirectory		true to enable subdirectory look-up.
 * @param [in,out]	parent	If non-null, the parent.
 **************************************************************************************************/

C_ReplicationDirectory::C_ReplicationDirectory(QString replicationId, QString directoryName, DWORD flag, BOOL subdirectory, C_DirectoryMonitor* parent) : m_Parent(parent), m_ReplicationId(replicationId), m_DirectoryName(directoryName)
{
   m_Buffer = new quint8[m_k_sBufferSize];
   m_Handle = 0;
   m_Flag = flag;
   m_Subdirectory = subdirectory;
   ZeroMemory(&m_Overlapped, sizeof(m_Overlapped));	// clear it
   m_Overlapped.hEvent = this;	// to get this object returned from the callback

   m_Handle = CreateFile(directoryName.toStdWString().c_str(), // create a handle to the given directory
      FILE_LIST_DIRECTORY,
      FILE_SHARE_READ | FILE_SHARE_WRITE,
      NULL,
      OPEN_EXISTING,
      FILE_FLAG_OVERLAPPED | FILE_FLAG_BACKUP_SEMANTICS ,
      NULL);
   if (m_Handle == INVALID_HANDLE_VALUE)
   {
      throw std::exception();
   }
}

/**********************************************************************************************//**
 * @fn	void C_ReplicationDirectory::M_ParseBuffer(char* buffer)
 *
 * @brief	Parse buffer.
 *
 * @author	Fituvu
 * @date	1/13/2012
 *
 * @param [in,out]	buffer	If non-null, the buffer.
 **************************************************************************************************/

void C_ReplicationDirectory::M_ParseBuffer(char* buffer)
{
   PFILE_NOTIFY_INFORMATION pBuffer = (PFILE_NOTIFY_INFORMATION) buffer;
   while (pBuffer)
   {
      if (pBuffer->Action == FILE_ACTION_RENAMED_OLD_NAME)
      {
         C_FileRenamedEvent *event = new C_FileRenamedEvent (this, pBuffer);
         emit M_OnEvent(event);
      } else {
         C_FileEvent *event = new C_FileEvent (this, pBuffer);
         emit M_OnEvent(event);
      }
   }
}

/**********************************************************************************************//**
 * @fn	bool C_ReplicationDirectory::M_ReadDirectory()
 *
 * @brief	Reads the directory.
 *
 * @author	Fituvu
 * @date	1/13/2012
 *
 * @return	true if it succeeds, false if it fails.
 **************************************************************************************************/

bool C_ReplicationDirectory::M_ReadDirectory()
{
   ulong nbytes = 0;
   if (!ReadDirectoryChangesW(m_Handle, m_Buffer, m_k_sBufferSize, m_Subdirectory, m_Flag, &nbytes, &m_Overlapped, &M_DirectoryChangesCallback)) return FALSE;
   return !nbytes;
}

/**********************************************************************************************//**
 * @fn	bool C_ReplicationDirectory::M_StopAllOperations()
 *
 * @brief	Gets the stop all operations.
 *
 * @author	Fituvu
 * @date	1/13/2012
 *
 * @return	true if it succeeds, false if it fails.
 **************************************************************************************************/

bool C_ReplicationDirectory::M_StopAllOperations()
{
   if (m_Handle)
   {
      if (!CancelIo(m_Handle)) return false;
      if (!CloseHandle(m_Handle)) return false;
      m_Handle = NULL;
      return true;
   }
   return false;
}

/**********************************************************************************************//**
 * @fn	void CALLBACK C_Directory::M_DirectoryChangesCallback(DWORD errorCode, DWORD nBytes,
 * 		LPOVERLAPPED pOverlapped)
 *
 * @brief	Callback, called when the m directory changes.
 *
 * @author	Fituvu
 * @date	1/13/2012
 *
 * @param	errorCode  	The error code.
 * @param	nBytes	  	The bytes.
 * @param	pOverlapped	The overlapped.
 *
 * @return	.
 **************************************************************************************************/

void CALLBACK C_ReplicationDirectory::M_DirectoryChangesCallback(DWORD errorCode, DWORD nBytes, LPOVERLAPPED pOverlapped)
{
   C_ReplicationDirectory *pDirectory = (C_ReplicationDirectory*) pOverlapped->hEvent;
   if (errorCode)
   {
      return;
   }
   if (!nBytes) // if there is no byte read, put the directory in read state again
   {
      pDirectory->M_ReadDirectory();
      return;
   } else
   {
      char *buffer = new char[nBytes];
      memcpy(buffer, pDirectory->m_Buffer, nBytes);
      pDirectory->M_ReadDirectory();
      pDirectory->M_ParseBuffer(buffer);
   }
}

/**********************************************************************************************//**
 * @fn	void C_DirectoryMonitor::C_FileEventDispatcher::M_Stop()
 *
 * @brief	Stops this object.
 *
 * @author	Fituvu
 * @date	1/13/2012
 **************************************************************************************************/

void	C_FileEventDispatcher::M_Stop()
{
	if (m_Running)
		QueueUserAPC(M_StopCallback, this->M_GetHandle(), (ulong)this);
}

/**********************************************************************************************//**
 * @fn	C_DirectoryMonitor::~C_DirectoryMonitor(void)
 *
 * @brief	Destructor.
 *
 * @author	Fituvu
 * @date	1/13/2012
 **************************************************************************************************/

C_DirectoryMonitor::~C_DirectoryMonitor(void)
{
   m_EventCollector.M_Stop();
   m_EventDispatcher.M_Stop();
   SleepEx(m_k_sEventDispatchingInterval, true);
   //TODO: Add cleaning code here
   for (t_DirectoriesList::iterator iter = m_Directories.begin(); iter != m_Directories.end(); iter++)
   {
      C_ReplicationDirectory *directory = (*iter);
		this->disconnect(directory);
      delete directory;
   }
   m_Directories.clear();
}

/**********************************************************************************************//**
 * @fn	void C_DirectoryMonitor::stl_OnEvent(C_FileEvent *event)
 *
 * @brief	Executes the event action.
 *
 * @author	Fituvu
 * @date	1/13/2012
 *
 * @param [in,out]	event	If non-null, the event.
 **************************************************************************************************/

void C_DirectoryMonitor::slt_OnEvent(C_FileEvent *event)
{
	// discard event exceed lenth
   QMap<QString, quint8>::const_iterator iter = m_EventSilencer.find(event->M_GetFullPath());
   if (iter != m_EventSilencer.end() || event->M_GetFullPath().size() > C_File::m_k_sSafeFileNameLength)
   {
      delete event;
   } else
   {
      if (event->M_GetFileName().size()>7)
      {
         QString tail = event->M_GetFileName().mid(event->M_GetFileName().size()-7);
         if (tail.compare(".ccdtmp") == 0 || tail.compare(".ccitmp") == 0)
         {
            delete event;
            return;
         }
      }
      m_EventDispatcher.M_Add(event);
   }
}
void C_DirectoryMonitor::slt_InternalEvent(C_FileEvent *event)
{
	emit M_OnEvent(event);
}

/**********************************************************************************************//**
 * @fn	void C_DirectoryMonitor::C_FileEventCollector::M_StartCallback()
 *
 * @brief	Callback, called when the m start.
 *
 * @author	Fituvu
 * @date	1/13/2012
 **************************************************************************************************/

void C_DirectoryMonitor::C_FileEventCollector::M_StartCallback()
{
   m_Running = true;
   while (m_Running)
   {
      SleepEx(INFINITE, TRUE);
   }
}

/**********************************************************************************************//**
 * @fn	void C_DirectoryMonitor::C_FileEventCollector::M_StopCallback(ULONG_PTR ptr)
 *
 * @brief	Callback, called when the m stop.
 *
 * @author	Fituvu
 * @date	1/13/2012
 *
 * @param	ptr	The pointer.
 **************************************************************************************************/

void C_DirectoryMonitor::C_FileEventCollector::M_StopCallback(ULONG_PTR ptr)
{
   C_FileEventCollector *thread = (C_FileEventCollector *) ptr;
   thread->M_SetState(false);
}

/**********************************************************************************************//**
 * @fn	int C_DirectoryMonitor::C_FileEventCollector::M_ThreadRoutine (void *arg)
 *
 * @brief	Thread routine.
 *
 * @author	Fituvu
 * @date	1/13/2012
 *
 * @param [in,out]	arg	If non-null, the argument.
 *
 * @return	.
 **************************************************************************************************/

DWORD WINAPI C_DirectoryMonitor::C_FileEventCollector::M_ThreadRoutine (void *arg)
{
   C_FileEventCollector * thread = (C_FileEventCollector*) arg;
   thread->M_StartCallback();
   return 0;
}

/**********************************************************************************************//**
 * @fn	C_FileEventDispatcher::C_FileEventDispatcher(C_IDirectoryEventListener *listener)
 *
 * @brief	Constructor.
 *
 * @author	Fituvu
 * @date	1/13/2012
 *
 * @param [in,out]	listener	If non-null, the listener.
 **************************************************************************************************/

C_FileEventDispatcher::C_FileEventDispatcher() : m_Running (false)
{
   m_Events.clear();
   m_EventsList.clear();
	m_Thread = CreateThread(NULL,	// cannot be inherited
					0,		// default stack size
					M_ThreadRoutine,
					this,
					0,		// start now
					NULL);
}

/**********************************************************************************************//**
 * @fn	C_DirectoryMonitor::C_FileEventDispatcher::~C_FileEventDispatcher()
 *
 * @brief	Destructor.
 *
 * @author	Fituvu
 * @date	1/13/2012
 **************************************************************************************************/

C_FileEventDispatcher::~C_FileEventDispatcher()
{
   if (m_Running) M_Stop();
   QMutexLocker lock(&m_ThreadConcurrencyLock);
   M_DispatchEvents();
   for (t_EventsMap::iterator iter = m_Events.begin(); iter != m_Events.end(); iter++ )
   {

      t_FileEventPtr event = (t_FileEventPtr) iter.value();
      if (event)
      {
         delete event;
      }
   }
   m_Events.clear();
}

/**********************************************************************************************//**
 * @fn	bool C_FileEventDispatcher::M_Add(const t_FileEventPtr &event)
 *
 * @brief	Adds event..
 *
 * @author	Fituvu
 * @date	1/13/2012
 *
 * @param	event	The event.
 *
 * @return	true if it succeeds, false if it fails.
 **************************************************************************************************/

bool C_FileEventDispatcher::M_Add(const t_FileEventPtr &event)
{
   QMutexLocker lock(&m_ThreadConcurrencyLock);
   m_EventsList.push_back(event);
   return true;
}

/**********************************************************************************************//**
 * @fn	void C_FileEventDispatcher::M_DispatchEvents()
 *
 * @brief	Dispatch events.
 *
 * @author	Fituvu
 * @date	1/13/2012
 **************************************************************************************************/

void C_FileEventDispatcher::M_DispatchEvents()
{
   QMutexLocker lock(&m_ThreadConcurrencyLock);

   for (t_EventsList::iterator iter = m_EventsList.begin(); iter != m_EventsList.end(); iter++)
   {
      const QString filepath = ((*iter)->M_GetAction() == C_FileEvent::t_FileEventRenamed) ? ((*iter)->M_GetDirectory()->M_GetDirectoryPath() + ((C_FileRenamedEvent*) (*iter))->M_GetOldFileName()) : (*iter)->M_GetFullPath();
      t_FileEventPtr oldevent = m_Events[filepath];
      if (!oldevent)
      {
         m_Events[filepath] = (*iter);
      } else
      {
         //t_FileEventPtr oldevent = (t_FileEventPtr) eventaddress;
			t_FileEventPtr newevent = *(iter);
         switch (oldevent->M_GetAction())
         {
         case C_FileEvent::t_FileEventAdded:
            switch (newevent->M_GetAction())
            {
               // if add then delete then do nothing
               case C_FileEvent::t_FileEventDeleted:
                  delete oldevent;
                  delete newevent;
                  m_Events[filepath] = 0;
                  break;
               // if add then rename then only add with new name
               case C_FileEvent::t_FileEventRenamed:
                  oldevent->M_SetFileName(newevent->M_GetFileName());
                  delete newevent;
                  m_Events[filepath] = oldevent;
                  break;
               // if add then modify then add
               case C_FileEvent::t_FileEventModified:
                  delete newevent;
                  break;
            }
            break;
         case C_FileEvent::t_FileEventDeleted:
            switch (newevent->M_GetAction())
            {
               // if delete then add then broadcast or can queue modify event
               case C_FileEvent::t_FileEventAdded:
                  newevent->M_SetAction(C_FileEvent::t_FileEventModified);
                  m_Events[filepath] = newevent;
                  delete oldevent;
                  break;
               case C_FileEvent::t_FileEventModified:
                  delete newevent;
                  break;
            }
            break;
         case C_FileEvent::t_FileEventRenamed:
            switch (newevent->M_GetAction())
            {
               // if delete then add then broadcast or can queue modify event
               case C_FileEvent::t_FileEventDeleted:
                  newevent->M_SetFileName(((C_FileRenamedEvent *)(oldevent))->M_GetOldFileName());
                  //m_Listener->M_OnEvent(newevent);
                  delete oldevent;
                  m_Events[filepath] = newevent;
                  break;
               case C_FileEvent::t_FileEventRenamed:
                  oldevent->M_SetFileName(newevent->M_GetFileName());
                  m_Events[filepath] = oldevent;
                  delete newevent;
                  break;
               case C_FileEvent::t_FileEventModified:
                  emit M_OnEvent(oldevent);
						oldevent->deleteLater();
                  m_Events[filepath] = 0;
                  m_Events[newevent->M_GetFullPath()] = newevent;
                  break;
            }
            break;
         case C_FileEvent::t_FileEventModified:
            switch (newevent->M_GetAction())
            {
               // if delete then add then broadcast or can queue modify event
               case C_FileEvent::t_FileEventDeleted:
                  emit M_OnEvent(newevent);
                  delete oldevent;
						newevent->deleteLater();
                  m_Events[filepath] = 0;
                  break;
               case C_FileEvent::t_FileEventRenamed:
                  emit M_OnEvent(newevent);
						newevent->deleteLater();
                  m_Events[filepath] = oldevent;
                  break;
               case C_FileEvent::t_FileEventModified:
                  m_Events[filepath] = newevent;
                  delete oldevent;
                  break;
            }
            break;
         }
      }
   }
   static quint64 counter = 1;
   static quint64 countermax = m_k_sMapCleaningInterval*1000/m_k_sEventDispatchingInterval;
   bool b = false;
   if (!(counter++ % countermax))
   {
      b = true;
   }
   for (t_EventsMap::iterator iter = m_Events.begin(); iter != m_Events.end(); )
   {
		C_FileEvent *event = (t_FileEventPtr) iter.value();
      if (event)
      {
         if (event->M_GetAction() != C_FileEvent::t_FileEventModified || event->M_IsFireable())
         {
				emit M_OnEvent(event);
				event->deleteLater();
				iter.value() = 0;
            t_EventsMap::iterator it = iter;
            iter++;
            m_Events.erase(it);
         } else
            iter ++;
      } else
      {
         if (b)
         {
            t_EventsMap::iterator it = iter;
            iter ++;
            m_Events.erase(it);
         } else
            iter ++;
      }
   }
   m_EventsList.clear();
}


/**********************************************************************************************//**
 * @fn	void C_DirectoryMonitor::C_FileEventDispatcher::M_StartInternal()
 *
 * @brief	Starts an internal.
 *
 * @author	Fituvu
 * @date	1/13/2012
 **************************************************************************************************/

void C_FileEventDispatcher::M_StartInternal()
{
   m_Running = true;
   while (m_Running)
   {
      SleepEx(m_k_sEventDispatchingInterval, TRUE); // G_Sleep
      M_DispatchEvents();
   }
}

/**********************************************************************************************//**
 * @fn	int C_FileEventDispatcher::M_ThreadRoutine (void *ptr)
 *
 * @brief	Thread routine.
 *
 * @author	Fituvu
 * @date	1/13/2012
 *
 * @param [in,out]	ptr	If non-null, the pointer.
 *
 * @return	.
 **************************************************************************************************/

DWORD	WINAPI C_FileEventDispatcher::M_ThreadRoutine (void *ptr)
{
   C_FileEventDispatcher * thread = (C_FileEventDispatcher*) ptr;
   thread->M_StartInternal();
   return 0;
}

/**********************************************************************************************//**
 * @fn	void C_FileEventDispatcher::M_StopCallback(ULONG_PTR ptr)
 *
 * @brief	Callback, called when the m stop.
 *
 * @author	Fituvu
 * @date	1/13/2012
 *
 * @param	ptr	The pointer.
 **************************************************************************************************/

void C_FileEventDispatcher::M_StopCallback(ULONG_PTR ptr)
{
   C_FileEventDispatcher * thread = (C_FileEventDispatcher*) ptr;
   thread->M_StopInternal();
}

/**********************************************************************************************//**
 * @fn	C_FileRenamedEvent::C_FileRenamedEvent(C_Directory *directory,
 * 		PFILE_NOTIFY_INFORMATION &eventInfo)
 *
 * @brief	Constructor.
 *
 * @author	Fituvu
 * @date	1/13/2012
 *
 * @param [in,out]	directory	If non-null, pathname of the directory.
 * @param [in,out]	eventInfo	Information describing the event.
 **************************************************************************************************/

C_FileRenamedEvent::C_FileRenamedEvent(C_ReplicationDirectory *directory, PFILE_NOTIFY_INFORMATION &eventInfo) : C_FileEvent (directory, eventInfo)
{
   if (eventInfo->Action == FILE_ACTION_RENAMED_OLD_NAME)
   {
      C_FileEvent newevent(directory, eventInfo);
      m_OldFileName = newevent.M_GetFileName();
   } else
   {
      C_FileEvent newevent(directory, eventInfo);
      m_OldFileName = this->M_GetFileName();
      m_FileName = newevent.M_GetFileName();
   }
   if (eventInfo && eventInfo->NextEntryOffset)
   {
      eventInfo = (PFILE_NOTIFY_INFORMATION)((DWORD) eventInfo + eventInfo->NextEntryOffset);
   } else
      eventInfo = 0;
}

/**********************************************************************************************//**
 * @fn	void C_FileEvent::M_SetFileName( const QString &filename)
 *
 * @brief	Sets a file name.
 *
 * @author	Fituvu
 * @date	1/13/2012
 *
 * @param	filename	Filename of the file.
 **************************************************************************************************/

void	C_FileEvent::M_SetFileName( const QString &filename)
{
   m_FileName = filename; m_FullPath.clear();
}

/**********************************************************************************************//**
 * @fn	C_FileEvent::C_FileEvent(C_Directory *directory, PFILE_NOTIFY_INFORMATION &eventInfo)
 *
 * @brief	Constructor.
 *
 * @author	Fituvu
 * @date	1/13/2012
 *
 * @param [in]	directory	If non-null, pathname of the directory.
 * @param [in]	eventInfo	Information describing the event.
 **************************************************************************************************/

C_FileEvent::C_FileEvent(C_ReplicationDirectory *directory, PFILE_NOTIFY_INFORMATION &eventInfo)
{
   m_Directory = directory;
   m_FullPath.clear();
   switch (eventInfo->Action)
   {
      case FILE_ACTION_ADDED:
         m_Action = t_FileEventAdded;
         break;
      case FILE_ACTION_MODIFIED:
         m_Action = t_FileEventModified;
         break;
      case FILE_ACTION_REMOVED:
         m_Action = t_FileEventDeleted;
         break;
      case FILE_ACTION_RENAMED_OLD_NAME:
      case FILE_ACTION_RENAMED_NEW_NAME:
         m_Action = t_FileEventRenamed;
         break;
   };
	m_TimeStamp = QDateTime::currentDateTimeUtc();
	m_FileName = QString::fromWCharArray(eventInfo->FileName, eventInfo->FileNameLength/2);
   if (eventInfo && eventInfo->NextEntryOffset)
   {
      eventInfo = (PFILE_NOTIFY_INFORMATION)((DWORD) eventInfo + eventInfo->NextEntryOffset);
   } else
      eventInfo = 0;
}

/**********************************************************************************************//**
 * @fn	void C_FileEvent::M_SetAction(const t_FileEventCode &code)
 *
 * @brief	Sets an action.
 *
 * @author	Fituvu
 * @date	1/13/2012
 *
 * @param	code	The code.
 **************************************************************************************************/

void	C_FileEvent::M_SetAction(const t_FileEventCode &code)
{
   m_Action = code;
};

/**********************************************************************************************//**
 * @fn	bool C_FileEvent::M_IsFireable()
 *
 * @brief	Gets the is fireable.
 *
 * @author	Fituvu
 * @date	1/13/2012
 *
 * @return	true if fireable, false if not.
 **************************************************************************************************/

bool	C_FileEvent::M_IsFireable()
{
	return  QDateTime::currentDateTimeUtc().toMSecsSinceEpoch() - m_TimeStamp.toMSecsSinceEpoch() > m_sDelayedInterval;
}

/**********************************************************************************************//**
 * @fn	const CSCommon::tstring &C_FileEvent::M_GetFullPath()
 *
 * @brief	Gets the full path.
 *
 * @author	Fituvu
 * @date	1/13/2012
 *
 * @return	The full path.
 **************************************************************************************************/

const QString	&C_FileEvent::M_GetFullPath()
{
	if (!m_Directory)
		return  m_FileName;
	if (m_FullPath.length() <= 0)
	{
		m_FullPath.clear();
		m_FullPath += (m_Directory->M_GetDirectoryPath()) + "\\";
		m_FullPath += (m_FileName);
	}
	return m_FullPath;
}

/**********************************************************************************************//**
 * @fn	const QString &C_FileEvent::M_GetFileName()
 *
 * @brief	Gets the file name.
 *
 * @author	Fituvu
 * @date	1/13/2012
 *
 * @return	The file name.
 **************************************************************************************************/

const QString	&C_FileEvent::M_GetFileName()
{
   return  m_FileName;
}

/**********************************************************************************************//**
 * @fn	C_ReplicationDirectory *C_FileEvent::M_GetDirectory()
 *
 * @brief	Gets the directory.
 *
 * @author	Fituvu
 * @date	1/13/2012
 *
 * @return	null if it fails, else the directory.
 **************************************************************************************************/

C_ReplicationDirectory	*C_FileEvent::M_GetDirectory()
{
   return m_Directory;
}

/**********************************************************************************************//**
 * @fn	const C_FileEvent::t_FileEventCode &C_FileEvent::M_GetAction()
 *
 * @brief	Gets the action.
 *
 * @author	Fituvu
 * @date	1/13/2012
 *
 * @return	The action.
 **************************************************************************************************/

const C_FileEvent::t_FileEventCode	&C_FileEvent::M_GetAction()
{
   return m_Action;
}

/**********************************************************************************************//**
 * @fn	const QString &C_FileRenamedEvent::M_GetOldFileName()
 *
 * @brief	Gets the old file name.
 *
 * @author	Fituvu
 * @date	1/13/2012
 *
 * @return	The old file name.
 **************************************************************************************************/

const QString &C_FileRenamedEvent::M_GetOldFileName()
{
   return m_OldFileName;
}

QString &C_FileRenamedEvent::M_GetOldFullPath()
{
	if (!m_Directory)
		return  m_OldFileName;
	QString oldfullpath;
	oldfullpath = (m_Directory->M_GetDirectoryPath()) + "\\" + m_OldFileName;
	return oldfullpath;
}

/**********************************************************************************************//**
 * @fn	C_DirectoryMonitor::C_FileEventCollector::C_FileEventCollector()
 *
 * @brief	Default constructor.
 *
 * @author	Fituvu
 * @date	1/13/2012
 **************************************************************************************************/

C_DirectoryMonitor::C_FileEventCollector::C_FileEventCollector()
{
   M_Start();
}

/**********************************************************************************************//**
 * @fn	C_DirectoryMonitor::C_FileEventCollector::~C_FileEventCollector()
 *
 * @brief	Destructor.
 *
 * @author	Fituvu
 * @date	1/13/2012
 **************************************************************************************************/

C_DirectoryMonitor::C_FileEventCollector::~C_FileEventCollector()
{
   M_Stop();
}

/**********************************************************************************************//**
 * @fn	void C_DirectoryMonitor::C_FileEventCollector::M_SetState(bool run)
 *
 * @brief	Sets a state.
 *
 * @author	Fituvu
 * @date	1/13/2012
 *
 * @param	run	true to run.
 **************************************************************************************************/

void C_DirectoryMonitor::C_FileEventCollector::M_SetState(bool run)
{
   m_Running = run;
}

/**********************************************************************************************//**
 * @fn	void C_DirectoryMonitor::C_FileEventCollector::M_Start()
 *
 * @brief	Starts this object.
 *
 * @author	Fituvu
 * @date	1/13/2012
 **************************************************************************************************/

void C_DirectoryMonitor::C_FileEventCollector::M_Start()
{
	//M_Initialize(&M_ThreadRoutine, this);
	m_Thread = CreateThread(NULL,	// cannot be inherited
									0,		// default stack size
									M_ThreadRoutine,
									this,
									0,		// start now
									NULL);
}

/**********************************************************************************************//**
 * @fn	void C_DirectoryMonitor::C_FileEventCollector::M_Stop()
 *
 * @brief	Stops this object.
 *
 * @author	Fituvu
 * @date	1/13/2012
 **************************************************************************************************/

void C_DirectoryMonitor::C_FileEventCollector::M_Stop()
{
	QueueUserAPC(M_StopCallback, m_Thread, (ulong)this);
}


/**********************************************************************************************//**
 * @fn	void C_DirectoryMonitor::C_FileEventDispatcher::M_StopInternal()
 *
 * @brief	Stop internal.
 *
 * @author	Fituvu
 * @date	1/13/2012
 **************************************************************************************************/

void	C_FileEventDispatcher::M_StopInternal()
{
   m_Running = false;
}

/**********************************************************************************************//**
 * @fn	const C_DirectoryMonitor::t_DirectoriesList &C_DirectoryMonitor::M_GetDirectoriesList()
 *
 * @brief	Gets the directories list.
 *
 * @author	Fituvu
 * @date	1/13/2012
 *
 * @return	The directories list.
 **************************************************************************************************/

const t_DirectoriesList	&C_DirectoryMonitor::M_GetDirectoriesList()
{
   return m_Directories;
}
bool	C_DirectoryMonitor::M_SilenceFile(const QString &fileName)
{
   m_EventSilencer[fileName] = 1;
   return true;
}
bool	C_DirectoryMonitor::M_RemoveSilencedFile(const QString &fileName)
{
	SleepEx(10, true); // delay
	QMap<QString, quint8>::iterator iter = m_EventSilencer.find(fileName);
   if (iter == m_EventSilencer.end())
      return false;
   m_EventSilencer.erase(iter);
   return true;
}
bool	C_DirectoryMonitor::M_IsFileSilenced(const QString &fileName)
{
	QMap<QString, quint8>::iterator iter = m_EventSilencer.find(fileName);
   if (iter == m_EventSilencer.end())
      return false;
   else
   return true;
}

