#include "htv_message.h"
#include <QDataStream>
#include "core/htv_replicationfile.h"
#include <QFileInfo>
#include "core/htv_directorymonitor.h"
#include "core/htv_config.h"

#define DEFAULT_GREETING_MESSAGE "1704"
C_FileCRCRequest::C_FileCRCRequest(QByteArray &data) : C_Message(t_FileCRCRequest)
{
	 QDataStream in(&data, QIODevice::ReadOnly);
	 in >> m_ReplicationId;
	 in >> m_Path;
}
C_FileBlockRequest::C_FileBlockRequest(QByteArray &data): C_Message(t_FileBlockRequest)
{
	 QDataStream in(&data, QIODevice::ReadOnly);
	 in >> m_ReplicationId;
	 in >> m_Path;
}
C_FileCRCRequest::C_FileCRCRequest(quint16 replicationid, QString path) : C_Message(t_FileCRCRequest)
{

}
C_FileBlockCRCRequest::C_FileBlockCRCRequest(QByteArray &data) : C_Message(t_FileBlockCRCRequest)
{

}
C_FileListMessage::C_FileListMessage(const QString &replicationId):
	m_ReplicationId(replicationId),
	C_Message(C_Message::t_FileListReply)
{

}

C_FileListMessage::C_FileListMessage(QByteArray &data):
	C_Message(C_Message::t_FileListReply)
{
	QDataStream in(&data, QIODevice::ReadOnly);
	in>>m_Files;
}

const QMap<QString, C_RFile> &C_FileListMessage::M_GetFilesMap()
{
	return m_Files;
}

void C_FileListMessage::M_AddFile(QFileInfo &fileInfo)
{
	C_RFile file(fileInfo, fileInfo.filePath(), m_ReplicationId);
	M_AddFile(file);
}

void C_FileListMessage::M_AddFile(C_RFile &file)
{
	m_Files[file.M_GetFullRelativePath()] = file;
}

QByteArray &C_FileListMessage::M_GetByteArray()
{
	m_Buffer.clear();
	QDataStream out(&m_Buffer, QIODevice::WriteOnly);
	out << m_Files;
	return m_Buffer;
}
C_HandsakeMessage::C_HandsakeMessage() : C_Message(t_HandsakeRequest)
{
	m_Buffer = QByteArray((DEFAULT_GREETING_MESSAGE));
}

C_HandsakeMessage::C_HandsakeMessage(QByteArray &data) : C_Message(t_HandsakeRequest)
{
	QString message (data);
	m_IsValid = (message.compare(DEFAULT_GREETING_MESSAGE) == 0);
}

bool	C_HandsakeMessage::M_IsValidHandsakeMessage()
{
	return m_IsValid;
}

C_FileAction::C_FileAction(QByteArray &data) : C_Message(t_FileAction)
{
	QDataStream in(&data, QIODevice::ReadOnly);
	quint8 action;
	in >> action;
	m_Action = (C_FileEvent::t_FileEventCode)(action);
	in >> m_ReplicationId;
	in >> m_Path;
	if (m_Action == C_FileEvent::t_FileEventRenamed)
	{
		in >> m_NewPath;
	}
}
C_FileAction::C_FileAction(C_FileEvent* event) : C_Message(t_FileAction)
{
	m_Action = event->M_GetAction();
	m_ReplicationId = event->M_GetDirectory()->M_GetReplicationId();

	m_Buffer.clear();
	QDataStream out(&m_Buffer, QIODevice::WriteOnly);
	quint8 action = (event->M_GetAction());
	out << action;
	out << event->M_GetDirectory()->M_GetReplicationId();
	C_ReplicationManagerConfig *config = C_ReplicationManagerConfig::getInstance();
	if (event->M_GetAction()!= C_FileEvent::t_FileEventRenamed)
	{
		m_Path = config->M_GetRelativePath(event->M_GetDirectory()->M_GetReplicationId(), event->M_GetFullPath());
		out << m_Path;
	} else
	{
		C_FileRenamedEvent *renameevent = (C_FileRenamedEvent *)(event);
		m_Path = config->M_GetRelativePath(event->M_GetDirectory()->M_GetReplicationId(),renameevent->M_GetOldFullPath());
		m_NewPath = config->M_GetRelativePath(event->M_GetDirectory()->M_GetReplicationId(),renameevent->M_GetFullPath());
		out << m_Path;
		out << m_NewPath;
	}
}
C_FileListRequest::C_FileListRequest(const QString &replicationId) : m_ReplicationId(replicationId), C_Message(t_FileListRequest)
{
	QDataStream out(&m_Buffer, QIODevice::WriteOnly);
	out << m_ReplicationId;
}
C_FileListRequest::C_FileListRequest(QByteArray &data) : C_Message(t_FileListRequest)
{
	QDataStream in(&data, QIODevice::ReadOnly);
	in >> m_ReplicationId;
}

C_FolderListRequest::C_FolderListRequest(const QString &replicationId): m_ReplicationId(replicationId), C_Message(t_FolderListRequest)
{
	QDataStream out(&m_Buffer, QIODevice::WriteOnly);
	out << m_ReplicationId;
}
C_FolderListRequest::C_FolderListRequest(QByteArray &data) : C_Message(t_FolderListRequest)
{
	QDataStream in(&data, QIODevice::ReadOnly);
	in >> m_ReplicationId;
}
C_FileSimpleMessage::C_FileSimpleMessage(const QString &replicationId, const QString &relativePath, const QString &fullPath, const qint8 &compressionLevel) : C_Message(t_FileSimpleMessage)
{
	m_ReplicationId = replicationId;
	m_RelativePath = relativePath;
	m_CompressionLevel = compressionLevel;
	m_FullPath = fullPath;
}
bool C_FileSimpleMessage::M_ReadDataToBuffer()
{
	QFile file(m_FullPath);
	if (!file.open(QIODevice::ReadOnly)) return false;
	m_Buffer.clear();
	QDataStream out(&m_Buffer, QIODevice::WriteOnly);
	out << m_ReplicationId;
	out << m_RelativePath;
	out << m_CompressionLevel;
	if (!m_CompressionLevel)
	{
		m_Buffer.reserve(m_Buffer.size() + file.size());
		out << file.readAll();
	} else
	{
		QByteArray data = qCompress(file.readAll(), m_CompressionLevel);
		out << data;
	}
	return true;
}

C_FileSimpleMessage::C_FileSimpleMessage(QByteArray &data) : C_Message(t_FileSimpleMessage)
{
	QDataStream in(&data, QIODevice::ReadOnly);
	in >> m_ReplicationId;
	in >> m_RelativePath;
	in >> m_CompressionLevel;
	if (m_CompressionLevel)
	{
		QByteArray buffer;
		in >> buffer;
		m_Buffer = qUncompress(buffer);
	} else
	{
		in >> m_Buffer;
	}
}

