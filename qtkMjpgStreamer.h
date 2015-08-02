#ifndef QTK_MJPG_STREAMER_H
#define QTK_MJPG_STREAMER_H
#include <QtNetwork>
#include "qtkHttpCommon.h"
#include "qtkVideoServer.h"

#define DEFAULT_BOUNDARY "vejamProjectBoundary"
#define FRAME_TIMER_PRESCALER 20	//ms -> eq. 50 fps.

class QtkMjpgStreamer : public QObject
{
    Q_OBJECT
 public:
	QtkMjpgStreamer(QTcpSocket* socket, QtkVideoServer* videoServer);
    void setMaxFramerate(int maxFrameRate);

private:    	
	QtkVideoServer* m_videoServer;	
    QTcpSocket* m_socket;
    QByteArray m_jpegBytes;
    bool m_frameReady;
    int m_streamerState;
    int m_frameDelayPreset;
    int m_error;

    enum streamerStates
    {
        sstIdle = 0,
        sstStart,
        sstServeHeader,
        sstServeFrameHeader,
        sstServeJpegBytes,
        sstFrameRateDelay,        
		sstConnectionClosed,        
        sstError = 100,
        sstLast
    };

    enum streamerError
    {
        errNoError = 0,
        errLast
    };

    void setStreamerState(int state);    
    QByteArray getHttpHeader();
    QByteArray getFrameHeader();
    QByteArray getBoundaryHeader();
    void setLastError(int error);
    int getLastError();
	
signals:
    void streamerError(int error);

public slots:
    void OnFrameUpdated();
    void OnStreamerRun();
	void OnDisconnected();
};
#endif
