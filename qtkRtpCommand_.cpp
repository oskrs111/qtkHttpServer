#include "qtkRtpCommand_.h"

qtkRtpCommand_::qtkRtpCommand_(QtkJsRpcServer *parent) :
    QObject(parent)
{    
    connect(parent,SIGNAL(commandExecute(int, QJsonObject)),this,SLOT(OnCommandExecute(int,QJsonObject)));
    connect(this,SIGNAL(commandDone(int,QByteArray)),parent,SLOT(OnCommandDone(int,QByteArray)));    
}

void qtkRtpCommand_::SetCommandId(int commandId)
{
    this->m_commandId = commandId;
    qDebug() << "[RPC-COMMAND] New with id= " << commandId;
}

int qtkRtpCommand_::GetCommandId()
{
    return this->m_commandId;
}

//void qtkRtpCommand_::CommandInit()
//{
    //OSLL: Assign here the correct commandId for derived command classes on
    //      overrriden function.
//    this->m_commandId = 0;
//}

void qtkRtpCommand_::OnCommandExecute(int commandId, QJsonObject params)
{
    if(commandId == this->m_commandId)
    {
        this->CommandExecute(params);
    }
}
