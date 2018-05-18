#include "htv_replicationconnection.h"
#include "core/htv_replicationfile.h"
#include <QTimerEvent>

static int m_sHeaderSize = 3;
static char m_k_sSeparatorToken = ' ';
static int m_k_sTransferTimeout = 30 * 1000;
C_ReplicationConnection::C_ReplicationConnection(QObject *parent):
	QTcpSocket(parent),
	m_HandsakeSent(false)
{
	m_State = t_ConnectionStateWaitingForHandsake;
	QObject::connect(this, SIGNAL(readyRead()), this, SLOT(slt_ProcessReadyRead()));
	QObject::connect(this, SIGNAL(connected()), this, SLOT(slt_SendHandsakeMessage()));
	m_MessageType = C_Message::t_Undefined;
	m_nBytesRequire = 0;
}
bool C_ReplicationConnection::M_SendMessage(C_Message* message)
{
	QMutexLocker lock(&m_Lock);
	qDebug()<<QDateTime::currentDateTimeUtc()<<" (C_ReplicationConnection): Send message ";
	if (!message) return false;
	char buffer[2];
	buffer[0] = message->M_GetMessageType();
	if (write(buffer,1) != 1) return false;
	quint16 size = message->M_GetByteArray().size();
	if (size > 65000) return false;

	memcpy(buffer, &size, 2);
	if (write(buffer, 2) != 2) return false;
	if (write(message->M_GetByteArray()) != message->M_GetByteArray().size())
		return false;
	return true;
}

bool C_ReplicationConnection::M_HasEnoughData()
{
	/*if (m_nBytesRequire <= 0)
	{
		m_nBytesRequire = M_CurrentTypeSize();
	}*/
	if (bytesAvailable() < m_nBytesRequire || m_nBytesRequire <= 0)
	{
		m_TransferTimerId = startTimer(m_TransferTimerId);
		return false;
	}
	return true;
}
quint32 C_ReplicationConnection::M_CurrentTypeSize()
{
	if (m_Buffer.size() < 3)
	{
		return 0;
	}
	quint16 size = (m_Buffer[2]&0xFF << 8) + (m_Buffer[1]&0xFF);
	return size;
}
void C_ReplicationConnection::M_ResetBuffer()
{
	m_Buffer.clear();
	m_nBytesRequire = 0;
}
bool	C_ReplicationConnection::M_IsHandsakeSent()
{
	return m_HandsakeSent;
}

void C_ReplicationConnection::slt_ProcessReadyRead()
{
	qDebug()<<QDateTime::currentDateTimeUtc()<<" (C_ReplicationConnection): Process ready read ";
	switch (m_State)
	{
	case t_ConnectionStateWaitingForHandsake:
		qDebug()<<QDateTime::currentDateTimeUtc()<<" (C_ReplicationConnection): Connection state waiting for handshake ";
		if (!M_ReadMessage() || m_MessageType != C_Message::t_HandsakeRequest)
		{
			abort();
			return;
		}
		if (!M_IsHandsakeSent())
			slt_SendHandsakeMessage();
		m_MessageType = C_Message::t_Undefined;
		m_State = t_ConnectionStateReadyForUse;
		emit S_ReadyForUse(this);
		m_Buffer.clear();
		break;
	case t_ConnectionStateReadyForUse:
		qDebug()<<QDateTime::currentDateTimeUtc()<<" (C_ReplicationConnection): Connection state ready for use ";
		if (!M_ReadMessage()) return;
		emit S_NewMessage(m_CurrentMessage);
		m_CurrentMessage = 0;
		m_Buffer.clear();
		m_MessageType = C_Message::t_Undefined;
		m_nBytesRequire = 0;
		break;
	case t_ConnectionStateDisconnected:
		qDebug()<<QDateTime::currentDateTimeUtc()<<" (C_ReplicationConnection): Connection state disconnected ";
		break;
	}
}
void C_ReplicationConnection::M_TimerEvent(QTimerEvent *timerEvent)
{
	if (timerEvent->timerId() == m_TransferTimerId)
	{
		abort();
		killTimer(m_TransferTimerId);
		m_TransferTimerId = 0;
	}
}

void C_ReplicationConnection::slt_SendHandsakeMessage()
{
	qDebug()<<QDateTime::currentDateTimeUtc()<<" (C_ReplicationConnection): Sending handshake message ";
	m_HandsakeSent = true;
	// TODO: Send handsake message
	C_HandsakeMessage message;
	m_HandsakeSent = M_SendMessage(&message);
	m_Buffer.clear();
}
int C_ReplicationConnection::M_ReadDataIntoBuffer(quint64 maxSize)
{
	qDebug()<<QDateTime::currentDateTimeUtc()<<" (C_ReplicationConnection): Read data into buffer ";
	if (maxSize > m_k_sMaxBufferSize)
	{
		return 0;
	}
	int nbytes = m_Buffer.size();
	if (nbytes >= m_k_sMaxBufferSize)
	{
		abort();
		return 0;
	}
	while (bytesAvailable() > 0 && m_Buffer.size() < maxSize)
	{
		m_Buffer.append(read(1));
	}
	return m_Buffer.size() - nbytes;
}
bool C_ReplicationConnection::M_ReadMessage()
{
	if (m_MessageType == C_Message::t_Undefined)
	{
		if (!M_GetMessageType()) return false;
		m_nBytesRequire = 0;
	}
	if (m_nBytesRequire <= 0)
	{
		if (!M_GetMessageSize()) return false;
	}
	return (M_GetMessage());
}
bool	C_ReplicationConnection::M_GetMessageType()
{
	if (m_MessageType == C_Message::t_Undefined)
	{
		if (M_ReadDataIntoBuffer(1) != 1) return false;
		m_MessageType = (C_Message::t_Message)(m_Buffer[0]&0xFF);
		m_Buffer.clear();
		return m_MessageType != C_Message::t_Undefined;
	}
	return false;
}

bool	C_ReplicationConnection::M_GetMessageSize()
{
	if (m_MessageType == C_Message::t_Undefined)
		return false;
	if (m_MessageType < C_Message::t_FileBlockMessage)
	{
		if (bytesAvailable() >= 2)
		{
			m_Buffer.clear();
			if (M_ReadDataIntoBuffer(2)!=2) return false;
			m_nBytesRequire = ((m_Buffer[1]&0xFF)<<8) | (m_Buffer[0]&0xFF);
			m_Buffer.clear();
			return true;
		}
	} else
	{
		if (bytesAvailable() >= 3)
		{
			m_Buffer.clear();
			if (M_ReadDataIntoBuffer(3)!=3) return false;
			m_nBytesRequire = ((m_Buffer[2]&0xFF)<<16) + ((m_Buffer[1]&0xFF)<<8) + (m_Buffer[0]&0xFF);
			m_Buffer.clear();
			return true;
		}
	}
	return false;
}

bool	C_ReplicationConnection::M_GetMessage()
{
	if (bytesAvailable() < m_nBytesRequire) return false;
	m_Buffer.clear();
	m_Buffer.append(read(m_nBytesRequire));

	switch (m_MessageType)
	{
		case C_Message::t_HandsakeRequest:
		{
			m_CurrentMessage = new C_HandsakeMessage(m_Buffer);
			if (!m_HandsakeSent)
				slt_SendHandsakeMessage();
			break;
		}
		case C_Message::t_FileListReply:
		{
			m_CurrentMessage = new C_FileListMessage(m_Buffer);
			break;
		}

		case C_Message::t_FileListRequest:
		{
			m_CurrentMessage = new C_FileListRequest (m_Buffer);
			break;
		}
		case C_Message::t_FileSimpleMessage:
		{
			m_CurrentMessage = new C_FileSimpleMessage(m_Buffer);
			break;
		}
	}
	return true;
}
