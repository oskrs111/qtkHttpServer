#ifndef QTKRTPCOMMAND_TEST__H
#define QTKRTPCOMMAND_TEST_H
#include <QObject>
#include "qtkRtpCommand_id.h"

class qtkRtpCommand_Test : public qtkRtpCommand_
{
    Q_OBJECT
public:
    explicit qtkRtpCommand_Test(QtkJsRpcServer *parent = 0);

private:
    void CommandInit();
    void CommandExecute(QByteArray params);
};

inline void qtkRtpCommand_Test::CommandInit()
{
	this->m_commandId = rci_TestCommand;
}

inline void qtkRtpCommand_Test::CommandExecute(QByteArray  params)
{
	QByteArray result = "{'jsonrpc': '2.0', 'result': 19, 'id': 1}"
	result = result.replace(''', '"');
	emit commandDone(this->m_commandId, result);	
}

#endif
