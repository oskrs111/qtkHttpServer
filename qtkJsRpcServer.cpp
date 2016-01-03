#include "qtkRtpCommand_headers.h"
#include "qtkRtpCommand_id.h"
#include "qtkHttpServer.h"
#include "qtkJsRpcServer.h"

QtkJsRpcServer::QtkJsRpcServer(QTcpSocket* socket, QJsonDocument* jsData, QObject *parent)
               :QObject(parent)
{
    this->m_error = errNoError;
    this->p_socket = socket;
    this->m_serverState = 0;

	if(jsData)
	{
        this->p_jsData = jsData;        
        if(this->p_jsData->isObject() == true)
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
//http://xmlrpc-epi.sourceforge.net/specs/rfc.fault_codes.php
{	
    int commandId = 0;
    int seqId = 0;
    QJsonObject params;
    QString command;

    switch(this->m_serverState)
    {
		case sstIdle:
			break;

        case sstGetCommand:
            command = this->p_jsData->object().take("method").toString();
            params = this->p_jsData->object().take("params").toObject();
            seqId  = this->p_jsData->object().take("id").toInt();
            commandId = this->findCommandId(command);
            if(commandId == 0)
            {
                QString ret = QString("{\"jsonrpc\": \"2.0\", \"error\": {\"code\": -32601, \"message\": \"server error. requested method not found\"}, \"id\": %1}").arg(seqId,0,10);
				this->OnCommandDone(commandId, ret.toLatin1());

                this->setServerState(sstConnectionClose);
                break;
            }
            else this->setServerState(sstExecuteCommand);
		
        case sstExecuteCommand:
			this->setServerState(sstWaitCommandReply);
            emit commandExecute(commandId, params, seqId);
			break;
			
        case sstWaitCommandReply:

            //Poasr un timeout....
			break;

		case sstConnectionClose:
            if(this->p_socket->bytesToWrite()) break;
            this->p_socket->close();
		case sstConnectionClosed:
            this->p_socket->deleteLater();
		    this->deleteLater();			
            qDebug() << "[rpc] Rpc end...";
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
    QTextStream os(this->p_socket);
    os.setAutoDetectUnicode(true);

    os << QString("%1").arg(QString(H200_STD_HEADER));
    os << QString("%1").arg(QString(HRPC_STD_HEADER));
    os << QString("%1\r\n").arg(result.size(),10);
    os << "\r\n";
    os.flush();
    this->p_socket->write(result);
    this->p_socket->flush();
    this->setServerState(sstConnectionClose);
    qDebug() << "[rpc] RPC-200 reply: " << result << " commandId: " << commandId;
}

void QtkJsRpcServer::commandsInit()
//OSLL: Command objects initialization...
{
    #include "qtkRtpCommandsInit.h"
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

QObject* QtkJsRpcServer::getEventTarget(QString targetName)
{
    QtkHttpServer* parent = 0;

    parent = (QtkHttpServer*)this->parent();
    return parent->getEventTarget(targetName);
}
