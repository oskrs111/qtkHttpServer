#include "qtkMjpgStreamer.h"

QtkMjpgStreamer::QtkMjpgStreamer(QTcpSocket* socket, QtkVideoServer* videoServer)
{
    this->m_error = errNoError;
    this->m_socket = socket;
    if(videoServer)
    {
        this->m_videoServer = videoServer;        
    }
    else
    {
        this->m_videoServer = 0;
    }
    this->m_streamerState = 0;
    this->m_frameReady = false;
    this->m_frameDelayPreset = 0;    
    this->setStreamerState(sstStart);

	connect(videoServer, SIGNAL(frameUpdated()), this, SLOT(OnFrameUpdated()));
    QTimer::singleShot(FRAME_TIMER_PRESCALER, this, SLOT(OnStreamerRun()));
}

void QtkMjpgStreamer::OnFrameUpdated()
{
    this->m_jpegBytes = this->m_videoServer->currentFrame2ByteArrayJpeg();
    this->m_frameReady = true;
	
}
	
void QtkMjpgStreamer::OnStreamerRun()
{
    static int frameDelay = 0;	
	
    switch(this->m_streamerState)
    {
       case sstIdle:

       case sstStart:
            //OSLL: Checking system is ready to run!!!
            if(this->m_videoServer)
            {
                if(this->m_videoServer->getServerState() == QCamera::ActiveState)
                this->setStreamerState(sstServeHeader);
				this->m_videoServer->Capture();
				this->m_socket->setSocketOption(QAbstractSocket::LowDelayOption,1);
				//dump.open(QIODevice::WriteOnly);
				//qDebug() << "Dump path:" << QFileInfo(dump).absoluteFilePath();
            }
            break;

       case sstServeHeader:
		    if(this->m_socket->state() != QAbstractSocket::ConnectedState) 
			{
				this->setStreamerState(sstConnectionClosed);
				break;
			}

            this->m_socket->write(this->getHttpHeader());
			//dump.write(this->getHttpHeader());
            this->m_socket->flush();
            this->setStreamerState(sstServeFrameHeader);
			goto sstServeFrameHeader;
            break;

       case sstServeFrameHeader:
sstServeFrameHeader:
		    if(this->m_socket->state() != QAbstractSocket::ConnectedState) 
			{
				this->setStreamerState(sstConnectionClosed);
				break;
			}

            if(this->m_frameReady == true)
            {
                this->m_frameReady = false;
				this->m_socket->write(this->getBoundaryHeader());
				//dump.write(this->getBoundaryHeader());
				//this->m_socket->flush();
                this->m_socket->write(this->getFrameHeader());
				//dump.write(this->getFrameHeader());
				this->m_socket->flush();
                this->setStreamerState(sstServeJpegBytes);				
				goto sstServeJpegBytes;
            }
            else
            {
				this->m_videoServer->Capture();
                break;
            }
            //OSLL: Continues to 'sstServeJpegBytes'

       case sstServeJpegBytes:
sstServeJpegBytes:
		   	if(this->m_socket->state() != QAbstractSocket::ConnectedState) 
			{
				this->setStreamerState(sstConnectionClosed);
				break;
			}
			
			if(this->m_socket->bytesToWrite() == 0)
			{
				this->m_socket->write(this->m_jpegBytes);				
				this->m_socket->flush();
			}
			else
			{
				this->m_videoServer->Capture();
				break;
			}

            this->setStreamerState(sstFrameRateDelay);
            frameDelay = this->m_frameDelayPreset;
            //OSLL: Continues to 'sstFrameRateDelay'
			break;

       case sstFrameRateDelay:
            if(frameDelay == 0)
            {
                this->setStreamerState(sstServeFrameHeader);
            }
            else if(frameDelay)
            {
                frameDelay--;
				this->m_frameReady = false;
            }
            break;

	   case sstConnectionClosed:
			//dump.close();
		    this->deleteLater();
			qDebug() << "Mjpeg Client gone...";
			return;

       case sstError:
            emit streamerError(this->getLastError());
            return;

        default:
            break;
    }

    QTimer::singleShot(FRAME_TIMER_PRESCALER, this, SLOT(OnStreamerRun()));
}

QByteArray QtkMjpgStreamer::getHttpHeader()
{
    QString s;
    s = QString("HTTP/1.0 200 OK\r\n" \
                HMIN_STD_HEADER \
                "Content-Type: multipart/x-mixed-replace;boundary=--" DEFAULT_BOUNDARY "\r\n" \
                "\r\n" );
                
    return s.toUtf8();
}

QByteArray QtkMjpgStreamer::getFrameHeader()
{
    QString s;
    s = QString("Content-Type: image/jpeg\r\n" \
                "Content-Length: %1\r\n" \
                "\r\n").arg(this->m_jpegBytes.size(),0,10);

    return s.toUtf8();
}

QByteArray QtkMjpgStreamer::getBoundaryHeader()
{
    QString s;
    s = QString("\r\n--" DEFAULT_BOUNDARY "\r\n");
    return s.toUtf8();
}

void QtkMjpgStreamer::setMaxFramerate(int maxFrameRate)
{
    if(maxFrameRate > 25) this->m_frameDelayPreset = 0;
    if(maxFrameRate <= 0 ) this->m_frameDelayPreset = 0;

    maxFrameRate = 1000 / maxFrameRate;
    this->m_frameDelayPreset = maxFrameRate / FRAME_TIMER_PRESCALER;
}

void QtkMjpgStreamer::setStreamerState(int state)
{
    this->m_streamerState = state;
}

void QtkMjpgStreamer::setLastError(int error)
{
    this->m_error = error;
}

int QtkMjpgStreamer::getLastError()
{
    int lastError = this->m_error;
    this->m_error = errNoError;
    return lastError;
}

void QtkMjpgStreamer::OnDisconnected()
{
	 this->setStreamerState(sstConnectionClosed);
	 this->deleteLater();
}

