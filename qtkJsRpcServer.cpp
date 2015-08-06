#include "qtkRtpCommand_headers.h"
#include "qtkJsRpcServer.h"

QtkJsRpcServer::QtkJsRpcServer(QTcpSocket* socket, QJsonDocument* jsData)
{
    this->m_error = errNoError;
    this->m_socket = socket;    
    this->m_serverState = 0;

	if(jsData)
	{
		this->m_jsData = jsData;
		this->setServerState(sstGetMethod);	
		QTimer::singleShot(SM_TIMER_PRESCALER, this, SLOT(OnServerRun()));
	}
	else
	{
		socket->close(); 
		socket->deleteLater();
		this->deleteLater();
	}	
}

QtkJsRpcServer::~QtkJsRpcServer()
{
    //lliberar this->l_commands
}

void QtkJsRpcServer::OnFrameUpdated()
{
    this->m_jpegBytes = this->m_videoServer->currentFrame2ByteArrayJpeg();
    this->m_frameReady = true;	
}
	
void QtkJsRpcServer::OnServerRun()
{	
    static int commandId = 0;

    switch(this->m_serverState)
    {
		case sstIdle:
			break;

        case sstGetCommand:
            this->setServerState(sstExecuteCommand);
			break;
		
        case sstExecuteCommand:
            emit commandExecute(commandId, QByteArray params);
            this->setServerState(sstWaitCommandReply);
			break;
			
        case sstWaitCommandReply:
            this->setServerState(sstSendCommandReply);
            //Poasr un timeout....
			break;
			
        case sstSendCommandReply:
            this->setServerState(sstConnectionClose);
            break;

		case sstConnectionClose:
			socket->close(); 			
		case sstConnectionClosed:
			//dump.close();
			socket->deleteLater();		
		    this->deleteLater();			
			qDebug() << "Rpc end...";
			return;

		case sstError:
            emit streamerError(this->getLastError());
            return;

        default:
            break;
    }

    QTimer::singleShot(SM_TIMER_PRESCALER, this, SLOT(OnServerRun()));
}

QByteArray QtkJsRpcServer::getHttpHeader()
{
    QString s;
    s = QString("HTTP/1.0 200 OK\r\n" \
                STD_HEADER \
                "Content-Type: application/json\r\n" \
				"Content-Length: %1\r\n" \
                "\r\n" ).arg(this->m_replyString.size(),0,10);
                
    return s.toUtf8();
}

void QtkJsRpcServer::setServerState(int state)
{
    this->m_serverState = state;
}

void QtkJsRpcServer::setLastError(int error)
{
    this->m_error = error;
}

int QtkJsRpcServer::getLastError()
{
    int lastError = this->m_error;
    this->m_error = errNoError;
    return lastError;
}

void QtkJsRpcServer::OnDisconnected()
{
	 this->setServerState(sstConnectionClosed);
     this->deleteLater();
}

void QtkJsRpcServer::OnCommandDone(int commandId, QByteArray result)
{

}

void QtkJsRpcServer::commandsInit()
//OSLL: Command objects initialization...
{
    qtkRtpCommand_Test* cmd1 = new qtkRtpCommand_Test(this);
    this->l_commands.append(cmd1);
}

