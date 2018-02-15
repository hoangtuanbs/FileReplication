#ifndef ABB_MESSAGE_H
#define ABB_MESSAGE_H

#include <QObject>
#include <QMap>
#include <core/abb_directorymonitor.h>

class C_RFile;
class C_FileEvent;
class QFileInfo;



class C_Message : QObject
{
	Q_OBJECT
	Q_ENUMS(t_Message)
public: enum t_Message
{
	t_Undefined,
	t_HandsakeRequest,
	t_FileListRequest,
	t_FileListReply,
	t_FolderListRequest,
	t_FileCRCRequest,
	t_FileCRCReply,
	t_FileRequest,
	t_FileReply,
	t_FileEventAcknowledgement,
	t_FileEventReply,
	t_FileBlockCRCRequest,
	t_FileBlockCRCReply,
	t_FileBlockRequest,
	t_FileBlockReply,
	t_FileAction,
	t_FileSimpleMessage,
	t_FileBlockMessage,
};
protected:
	t_Message	m_Message;
	QByteArray	m_Buffer;
public:
	explicit C_Message(const t_Message &code, QObject *parent = 0) : m_Message(code){}
	const		t_Message &M_GetMessageType() { return m_Message;}
	virtual QByteArray &M_GetByteArray() { return m_Buffer;}
};
class C_HandsakeMessage : public C_Message
{
	public:			C_HandsakeMessage();
	public:			C_HandsakeMessage(QByteArray &data);
	public: bool	M_IsValidHandsakeMessage();
	private: bool	m_IsValid;
};

class C_FileListMessage : public C_Message
{
	public: C_FileListMessage(const QString &replicationId);
	public: C_FileListMessage(QByteArray &data);
	public: void M_AddFile(QFileInfo &fileInfo);
	public: void M_AddFile(C_RFile &file);
	public: virtual QByteArray &M_GetByteArray();
	public: const QMap<QString, C_RFile> &M_GetFilesMap();

	private:	QMap<QString, C_RFile>	m_Files;
	private: QString	m_ReplicationId;
};
class C_FileListRequest : public C_Message
{
	public: C_FileListRequest(const QString &replicationId);
	public: C_FileListRequest(QByteArray &data);
	public: const QString &M_GetReplicationId() { return m_ReplicationId;}
	private: QString	m_ReplicationId;
};
class C_FolderListRequest : public C_Message
{
	public: C_FolderListRequest(const QString &replicationId);
	public: C_FolderListRequest(QByteArray &data);
	public: const QString &M_GetReplicationId() { return m_ReplicationId;}

	private: QString	m_ReplicationId;
};
class C_FileAction : public C_Message
{
	public:	C_FileAction(C_FileEvent* event);
	public:	C_FileAction(QByteArray &data);
	public:	const QString	&M_GetPath() { return m_Path;}
	public:	C_FileEvent::t_FileEventCode &M_GetAction() { return m_Action;}
	public:	const	QString	&M_GetNewPath() { return m_NewPath;}
	public:	const	QString	&M_GetReplicationId() { return m_ReplicationId;}

	private: C_FileEvent::t_FileEventCode	m_Action;
	private:	QString	m_ReplicationId;
	private:	QString	m_Path;
	private: QString	m_NewPath;
};

class C_FileCRCRequest : public C_Message
{
public:
	 C_FileCRCRequest(QByteArray &data);
	 C_FileCRCRequest(quint16 replicationid, QString path);
	 const quint16 &M_GetReplicationId() { return m_ReplicationId; }
	 const QString &M_GetPath() { return m_Path;}
	 friend QDataStream &operator<< (QDataStream & os, C_FileCRCRequest &message);

private:
	 quint16 m_ReplicationId;
	 quint8  m_RequestType;
	 QString m_Path;
};

class C_FileCRCReply : public C_Message
{
public:
	 C_FileCRCReply(quint16 replicationid, QString path);
	 const quint16 &M_GetReplicationId() { return m_ReplicationId; }
	 const QString &M_GetPath() { return m_Path;}
	 friend QDataStream &operator<< (QDataStream & os, C_Message &message);

private:
	 quint16 m_ReplicationId;
	 QString m_Path;
};

class C_FileBlockRequest : public C_Message
{
public:
	 C_FileBlockRequest(QByteArray &data);
	 C_FileBlockRequest(quint16 replicationid, QString path);
	 const quint16 &M_GetReplicationId() { return m_ReplicationId; }
	 const QString &M_GetPath() { return m_Path;}
	 friend QDataStream &operator<< (QDataStream & os, C_Message &message);

private:
	 quint16 m_SessionId;
	 quint16 m_ReplicationId;
	 QString m_Path;
	 quint64 m_BlockId;
	 quint64 m_BlockSize;
	 QByteArray m_Data;
};

class C_FileBlockCRCRequest : public C_Message
{
public:
	 C_FileBlockCRCRequest(QByteArray &data);
	 C_FileBlockCRCRequest(quint16 replicationid, QString path);
	 const quint16 &M_GetReplicationId() { return m_ReplicationId; }
	 const QString &M_GetPath() { return m_Path;}
	 friend QDataStream &operator<< (QDataStream & os, C_Message &message);

private:
	 quint16 m_ReplicationId;
	 QString m_Path;
	 quint64 m_BlockId;
	 quint64 m_BlockSize;
	 QByteArray m_Data;
};

class C_FileSimpleMessage : public C_Message
{
	public: C_FileSimpleMessage(const QString &replicationId, const QString &relativePath, const QString &fullPath, const qint8 &compressionLevel = 0);
	public: C_FileSimpleMessage(QByteArray &data);
	public: const QString &M_GetReplicationId(){ return m_ReplicationId;}
	public: const QString &M_GetRelativePath(){ return m_RelativePath;}
	public: const QString &M_GetFileName(){ return m_FileName;}
	public: bool M_ReadDataToBuffer();
	private: QString	m_ReplicationId;
	private: QString	m_RelativePath;
	private: QString	m_FileName;
	private: quint64	m_CRC;
	private: qint8		m_CompressionLevel;
	private: QString	m_FullPath;
};
class C_FileHeaderMessage : public C_Message
{
	public: C_FileHeaderMessage(const QString &replicationId);
	public: C_FileHeaderMessage(QByteArray &data);
	public: const QString &M_GetReplicationId() { return m_ReplicationId;}

	private: QString	m_ReplicationId;
};
class C_FileBlockMessage : public C_Message
{
	public: C_FileBlockMessage(const QString &replicationId);
	public: C_FileBlockMessage(QByteArray &data);
	public: const QString &M_GetReplicationId() { return m_ReplicationId;}

	private: QString	m_ReplicationId;
};
#endif // ABB_MESSAGE_H
