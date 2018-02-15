#ifndef ABB_REPLICATIONFILE_H
#define ABB_REPLICATIONFILE_H

#include <QFileInfo>
#include <QDateTime>
class C_RFile
{
public:
	C_RFile(){}
	C_RFile(const QFileInfo& fileInfo, const QString &replicationFolder, const QString &replicationId);
	QString				M_GetFileName();
	const QString		&M_GetFullRelativePath();
	QString				M_GetRelativePath();
	const QDateTime	&M_GetLastWrite();
	const quint64		&M_GetFileSize();
	const quint64		&M_GetCRC();
private:
	QString		m_ReplicationId;
	QString		m_RelativeFilePath;
	QDateTime	m_LastWrite;
	quint64		m_FileSize;
	quint64		m_CRC;
	friend QDataStream &operator>>(QDataStream &out, C_RFile& file);
	friend QDataStream &operator<<(QDataStream &out, const C_RFile& file);
};

#endif // ABB_REPLICATIONFILE_H
