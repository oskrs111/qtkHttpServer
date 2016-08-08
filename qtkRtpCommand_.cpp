#include "qtkRtpCommand_.h"
#include "qtkRtpCommand_id.h"

qtkRtpCommand_::qtkRtpCommand_(QtkJsRpcServer *parent) :
    QObject(parent)
{    
    connect(parent,SIGNAL(commandExecute(int, QJsonObject,int)),this,SLOT(OnCommandExecute(int,QJsonObject,int)));
    connect(this,SIGNAL(commandDone(int,QByteArray)),parent,SLOT(OnCommandDone(int,QByteArray)));    
}

void qtkRtpCommand_::SetCommandId(int commandId)
{
    this->m_commandId = commandId;
    qDebug() << "[rpcObject] New with id= " << commandId << " method= " << k_rtp_command_id::rtpCommands[(commandId - RCI_BASE)].p_commandAlias;
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

void qtkRtpCommand_::OnCommandExecute(int commandId, QJsonObject params, int seqId)
{
    if(commandId == this->m_commandId)
    {
        this->CommandExecute(params, seqId);
    }
}
