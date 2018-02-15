#include "abb_replicationfile.h"
#include <QDataStream>

C_RFile::C_RFile(const QFileInfo& fileInfo, const QString &replicationFolder, const QString &replicationId)
{
}
QString		C_RFile::M_GetFileName()
{
	QString fileName = m_RelativeFilePath;
	int index = fileName.lastIndexOf("\\");
	if (index >=0)
	{
		return fileName.right(index);
	}
	return fileName;
}

const QString		&C_RFile::M_GetFullRelativePath()
{
	return m_RelativeFilePath;
}

QString	C_RFile::M_GetRelativePath()
{
	int index =  m_RelativeFilePath.lastIndexOf("\\");
	if (index >=0)
	{
		return  m_RelativeFilePath.left(index);
	} else
		return "";
}

const QDateTime	&C_RFile::M_GetLastWrite()
{
	return m_LastWrite;
}

const quint64		&C_RFile::M_GetFileSize()
{
	return m_FileSize;
}

const quint64		&C_RFile::M_GetCRC()
{
	return m_CRC;
}
QDataStream &operator>>(QDataStream &out, C_RFile& file)
{
	out >> file.m_ReplicationId;
	out >> file.m_RelativeFilePath;
	out >> file.m_LastWrite;
	out >> file.m_FileSize;
	out >> file.m_CRC;
	return out;
}

QDataStream &operator<<(QDataStream &out, const C_RFile& file)
{
	out << file.m_ReplicationId;
	out << file.m_RelativeFilePath;
	out << file.m_LastWrite;
	out << file.m_FileSize;
	out << file.m_CRC;
	return out;
}
