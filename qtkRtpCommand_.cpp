#include "qtkRtpCommand_.h"

qtkRtpCommand_::qtkRtpCommand_(QtkJsRpcServer *parent) :
    QObject(parent)
{    
    connect(parent,SIGNAL(commandExecute(int, QByteArray)),this,SLOT(OnCommandExecute(int,QByteArray));
    connect(this,SIGNAL(commandDone(int,QByteArray)),this,SLOT(OnCommandDone(int,QByteArray));
    this->CommandInit();
}

void qtkRtpCommand_::CommandInit()
{
    //OSLL: Assign here the correct commandId for derived command classes on
    //      overrriden function.
    this->m_commandId = 0;
}

void qtkRtpCommand_::OnCommandExecute(int commandId, QByteArray params)
{
    if(commandId == this->m_commandId)
    {
        this->CommandExecute(params);
    }
}
