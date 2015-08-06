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
    virtual void CommandInit();
    virtual void CommandExecute(QByteArray params) = 0;

public:
signals:
    void commandDone(int commandId, QByteArray result);

public slots:
    void OnCommandExecute(int commandId, QByteArray params);
};

#endif // QTKRTPCOMMAND__H
