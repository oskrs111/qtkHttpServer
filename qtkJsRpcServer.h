#ifndef QTK_JSRPC_STREAMER_H
#define QTK_JSRPC_STREAMER_H
#include <QtNetwork>
#include "qtkHttpCommon.h"

#define SM_TIMER_PRESCALER 	25

class QtkJsRpcServer : public QObject
{
    Q_OBJECT
 public:
	QtkJsRpcServer(QTcpSocket* socket, QJsonDocument* jsData);
    void setMaxFramerate(int maxFrameRate);

private:    	
	QJsonDocument* m_jsData;		
    QTcpSocket* m_socket;       	
    int m_error;
	int m_serverState;
	QList <QObject*> l_commands;

    enum serverStates
    {
        sstIdle = 0,
        sstGetCommand,
        sstExecuteCommand,
        sstWaitCommandReply,
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
    void setLastError(int error);
    int getLastError();	
	void commandsInit();
    int findCommandId(QString commandAlias);
	
signals:
    void serverError(int error);
    void commandExecute(int commandId, QJsonObject params);
public slots:
	void OnServerRun();
	void OnDisconnected();
	void OnCommandDone(int commandId, QByteArray result);
};
#endif
