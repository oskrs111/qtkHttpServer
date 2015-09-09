#ifndef QTKRTPCOMMAND__H
#define QTKRTPCOMMAND__H
#include <QObject>
#include "qtkJsRpcServer.h"

class qtkRtpCommand_ : public QObject
{
    Q_OBJECT
public:
    explicit qtkRtpCommand_(QtkJsRpcServer *parent = 0);

private:
    int m_commandId;

public:
    virtual void CommandInit() = 0;
    virtual void CommandExecute(QJsonObject params, int seqId) = 0;
    void SetCommandId(int commandId);
    int GetCommandId();

Q_SIGNALS:
    void commandDone(int commandId, QByteArray result);

public slots:
    void OnCommandExecute(int commandId, QJsonObject params, int seqId);
};

#endif // QTKRTPCOMMAND__H
