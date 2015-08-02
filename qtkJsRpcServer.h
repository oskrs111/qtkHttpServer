#ifndef QTK_JSRPC_STREAMER_H
#define QTK_JSRPC_STREAMER_H
#include <QtNetwork>
#include "qtkHttpCommon.h"

#define SM_TIMER_PRESCALER 	50

class QtkJsRpcServer : public QObject
{
    Q_OBJECT
 public:
	QtkJsRpcServer(QTcpSocket* socket, QJsonDocument* jsData);
    void setMaxFramerate(int maxFrameRate);

private:    	
	QJsonDocument* m_jsData;		
    QTcpSocket* m_socket;       
	QString	m_replyString;	
    int m_error;
	int m_serverState;

    enum serverStates
    {
        sstIdle = 0,
        sstGetMethod,    
		sstExecuteMethod,    
		sstSendReply,   
		sstConnectionClose,
		sstConnectionClosed,
        sstError = 100,
        sstLast
    };

    enum serverError
    {
        errNoError = 0,
        errLast
    };

    void setServerState(int state);    
    QByteArray getHttpHeader();    
    
    void setLastError(int error);
    int getLastError();
	
signals:
    void serverError(int error);

public slots:
	void OnServerRun();
	void OnDisconnected();
};
#endif
