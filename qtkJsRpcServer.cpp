#include "qtkRtpCommand_headers.h"
#include "qtkRtpCommand_id.h"
#include "qtkJsRpcServer.h"

QtkJsRpcServer::QtkJsRpcServer(QTcpSocket* socket, QJsonDocument* jsData)
{
    this->m_error = errNoError;
    this->m_socket = socket;    
    this->m_serverState = 0;

	if(jsData)
	{
		this->m_jsData = jsData;
        if(this->m_jsData->isObject() == true)
        {
            this->commandsInit();
			this->setServerState(sstGetCommand);
            QTimer::singleShot(SM_TIMER_PRESCALER, this, SLOT(OnServerRun()));
        }
        else
        {
            goto error_;
        }
	}
	else
    {
error_:
		socket->close(); 
		socket->deleteLater();
		this->deleteLater();
	}	
}

QtkJsRpcServer::~QtkJsRpcServer()
{
    int t = 0;
    for(t = 0; t < this->l_commands.size(); t++)
    {
       delete  this->l_commands.at(t);
    }
}
	
void QtkJsRpcServer::OnServerRun()
{	
    int commandId = 0;
    QJsonObject params;
    QString command;

    switch(this->m_serverState)
    {
		case sstIdle:
			break;

        case sstGetCommand:
            command = this->m_jsData->object().take("method").toString();
            params = this->m_jsData->object().take("params").toObject();
            commandId = this->findCommandId(command);
            this->setServerState(sstExecuteCommand);
		
        case sstExecuteCommand:
			this->setServerState(sstWaitCommandReply);
            emit commandExecute(commandId, params);            
			break;
			
        case sstWaitCommandReply:

            //Poasr un timeout....
			break;

		case sstConnectionClose:
			if(this->m_socket->bytesToWrite()) break;
            this->m_socket->close();
		case sstConnectionClosed:
            this->m_socket->deleteLater();
		    this->deleteLater();			
			qDebug() << "Rpc end...";
			return;

		case sstError:
            emit serverError(this->getLastError());
            this->setServerState(sstConnectionClose);
            return;

        default:
            break;
    }

    QTimer::singleShot(SM_TIMER_PRESCALER, this, SLOT(OnServerRun()));
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
    QTextStream os(this->m_socket);
    os.setAutoDetectUnicode(true);

    os << QString("%1").arg(QString(H200_STD_HEADER));
    os << QString("%1").arg(QString(HRPC_STD_HEADER));
    os << QString("%1\r\n").arg(result.size(),10);
    os << "\r\n";
    os.flush();
    this->m_socket->write(result);
	this->m_socket->flush();
    this->setServerState(sstConnectionClose);
    qDebug() << "[200] RPC reply: " << result << " commandId: " << commandId;
}

void QtkJsRpcServer::commandsInit()
//OSLL: Command objects initialization...
{
    qtkRtpCommand_Test* cmd1 = new qtkRtpCommand_Test(this);
    this->l_commands.append(cmd1);
}

int QtkJsRpcServer::findCommandId(QString commandAlias)
{
    struct k_rtp_command_id::rtpCommandStruct* p;
    int t = 0;

    for(t = 0; t < (sizeof(k_rtp_command_id::rtpCommands)/sizeof(k_rtp_command_id::rtpCommandStruct)); t++)
    {
        p = &k_rtp_command_id::rtpCommands[t];
        if(commandAlias.compare(QString(p->p_commandAlias)) == 0)
        {
            return p->m_commandId;
        }
    }

    return 0;
}
