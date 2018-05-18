#ifndef HTV_REPLICATIONCONNECTION_H
#define HTV_REPLICATIONCONNECTION_H

#include "htv_message.h"
#include <QObject>
#include <QHostAddress>
#include <QString>
#include <QTcpSocket>
#include <QTime>
#include <QTimer>
#include <QMutex>
 static const int m_k_sMaxBufferSize = 1024000;

class C_ReplicationConnection : public QTcpSocket
{
    Q_OBJECT
public:
	enum T_ConnectionState {
		t_ConnectionStateWaitingForHandsake,
		t_ConnectionStateHandsaking,
		t_ConnectionStateReadyForUse,
		t_ConnectionStateDisconnected
	};
	explicit C_ReplicationConnection(QObject *parent = 0);
	bool M_SendMessage(C_Message* );
signals:
	void S_NewMessage(C_Message* );
	void S_NewFile(C_Message* );
	void S_ReadyForUse(C_ReplicationConnection* connection);

public slots:
private slots:
	void	slt_ProcessReadyRead();
	void	slt_SendHandsakeMessage();
private:
	bool		M_HasEnoughData();
	int		M_ReadDataIntoBuffer(quint64 maxSize = m_k_sMaxBufferSize);
	quint32	M_CurrentTypeSize();
	void		M_TimerEvent(QTimerEvent *timerEvent);
	void		M_ResetBuffer();
	bool		M_IsHandsakeSent();
	bool		M_ReadMessage();
	bool		M_GetMessageType();
	bool		M_GetMessageSize();
	bool		M_GetMessage();
	T_ConnectionState		m_State;
	QByteArray				m_Buffer;
	quint64					m_nBytesRequire;
	quint32					m_TransferTimerId;
	C_Message::t_Message	m_MessageType;
	bool						m_HandsakeSent;
	QMutex					m_Lock;
	C_Message				*m_CurrentMessage;
};

#endif // htv_REPLICATIONCONNECTION_H
