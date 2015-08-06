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
	QList <QObject*> l_commands;

    enum serverStates
    {
        sstIdle = 0,
        sstGetCommand,
        sstExecuteCommand,
        sstWaitCommandReply,
        sstSendCommandReply,
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

    ~QtkJsRpcServer();
    void setServerState(int state);    
    QByteArray getHttpHeader();    
    
    void setLastError(int error);
    int getLastError();	
	void commandsInit();
	
signals:
    void serverError(int error);
	void commandExecute(int commandId, QByteArray params);
public slots:
	void OnServerRun();
	void OnDisconnected();
	void OnCommandDone(int commandId, QByteArray result);
};
#endif
