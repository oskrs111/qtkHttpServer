#ifndef QTK_MJPG_STREAMER_H
#define QTK_MJPG_STREAMER_H
#include <QtNetwork>
#include "qtkvideoserver.h"

#define FRAME_TIMER_PRESCALER 20	//ms -> eq. 50 fps.

//OSLL: This macro is taken from 'httpd.h" header from output_http pluguin that is part of mjpeg-streamer application.
//OSLL: mjpeg-streamer is Tom Stöveken Copyright (C).

#define STD_HEADER "Connection: close\r\n" \
                   "Server: QtkMjpgStreamer-1.0\r\n" \
                   "Cache-Control: no-store, no-cache, must-revalidate, pre-check=0, post-check=0, max-age=0\r\n" \
                   "Pragma: no-cache\r\n" \
                   "Expires: Mon, 3 Jan 2000 12:34:56 GMT\r\n"

#define DEFAULT_BOUNDARY "vejamProjectBoundary"

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
