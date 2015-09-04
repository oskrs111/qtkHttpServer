#include "qtkHttpServer.h"

QtkHttpServer::QtkHttpServer(quint16 port, QObject* parent)
         : QTcpServer(parent)
{
         listen(QHostAddress::Any, port);
		 this->m_fileRootPath= HS_WWW_DEFAULT_PATH;
		 this->m_clientCount = 0;
         qDebug() << "httpServer listening on tcp port: " << port;
         qDebug() << "httpServer files at: " << this->m_fileRootPath;
#ifdef HS_MJPG_STREAMER_ENABLE
		 this->m_videoServer = 0;
		 this->m_mjpegUri = HS_MJPEG_DEFAULT_URI;
#endif
}

void QtkHttpServer::setFilesRootPath(QString path)
{
	this->m_fileRootPath = path;
}

void QtkHttpServer::setAppRootPath(QString path)
{
	this->m_appRootPath = path;
}

void QtkHttpServer::incomingConnection(int socket)
{
         // When a new client connects, the server constructs a QTcpSocket and all
         // communication with the client is done over this QTcpSocket. QTcpSocket
         // works asynchronously, this means that all the communication is done
         // in the two slots readClient() and discardClient().
         QTcpSocket* s = new QTcpSocket(this);
		 if(s)
		 {
			 connect(s, SIGNAL(readyRead()), this, SLOT(readClient()));
			 connect(s, SIGNAL(disconnected()), this, SLOT(discardClient()));
			 s->setSocketDescriptor(socket);
			 
			 qDebug() << "New Remote Connection..: " << this->m_clientCount;
			 this->m_clientCount++;
		 }
		 else
		 {
			qDebug() << "New Remote Connection..: OUT OF MEMORY!!!";
		 }

}

void QtkHttpServer::readClient()
{
         // This slot is called when the client sent data to the server. The
         // server looks if it was a get request and sends a very simple HTML
         // document back.
         QTcpSocket* socket = (QTcpSocket*)sender();
         QTextStream os(socket);
         os.setAutoDetectUnicode(true);

#ifdef HS_RPC_SERVER_ENABLE
    QJsonDocument* doc = new QJsonDocument();
    QJsonParseError err;
#endif

         if (socket->canReadLine())
         {
             QStringList tokens = QString(socket->readLine()).split(QRegExp("[ \r\n][ \r\n]*"));
             QString fileName = tokens[1];
			 
			 if (tokens[0] == "GET") 
			 {				
			   switch(this->getFilename(&fileName))
				{
#ifdef HS_MJPG_STREAMER_ENABLE				
                    case HS_GET_MJPG_STREAM:
                        if(this->m_videoServer)
                        {
                            QtkMjpgStreamer* streamer = new QtkMjpgStreamer(socket, this->m_videoServer);	
							streamer->setMaxFramerate(this->m_maxFrameRate);
							connect(socket, SIGNAL(disconnected()), streamer, SLOT(OnDisconnected())); 
                            return;
                        }
                        else
                        {
                            os << "HTTP/1.0 404 Not found\r\n\r\n";
							os.flush();
                            qDebug() << "[404] File not found: No video source available?" << fileName;
                            socket->close();
                            return;
                        }
						break;
#endif						
                    case HS_GET_LOCAL_FILE:
						qDebug() << "Client file request: " << fileName;
					default:
						break;
				}				
			 
				QFile file(fileName);					
				qDebug() << "Serving: " << QFileInfo(file).absoluteFilePath();
                if(file.open(QFile::ReadOnly))
				{
					os << "HTTP/1.0 200 Ok\r\n";				
					//os << "Content-Type: text/html; charset=\"utf-8\"\r\n";
					os << "Cache-Control: no-cache\r\n";
					os << "Connection: close\r\n";
					os << "Content-Length: ";
					os << QString("%1\r\n").arg(file.size(),10);
                    os << "Content-type: " ;
                    os << QString("%1\r\n").arg(getMIMEType(QFileInfo(file).suffix()));
					os << "\r\n";
					os.flush();
                    socket->write(file.readAll());
                    file.close();
					qDebug() << "[200] File sent: " << file.fileName();
				}
				else
				{
					os << "HTTP/1.0 404 Error file not found\r\n\r\n";				
					qDebug() << "[404] File not found (GET): " << file.fileName();
				}									
                     
                socket->close();                
                if (socket->state() == QTcpSocket::UnconnectedState) 
				{
					socket->deleteLater();                
                }
             }
			 else if (tokens[0] == "POST") 
			 {	
				QMap<QByteArray, QByteArray> headers;
				QByteArray http = socket->readAll();
				headers = this->parseHttpHeaders(http);
				QByteArray rpc;

				switch(this->getFilename(&fileName))
				{
#ifdef HS_RPC_SERVER_ENABLE				
                    case HS_POST_JSON_RPC:				
						rpc = this->getPostBody(http);						
						*doc = QJsonDocument::fromJson(rpc,&err);
						qDebug() << *doc;
						if(err.error ==  QJsonParseError::NoError)
						{
                            QtkJsRpcServer*  server = new QtkJsRpcServer(socket, doc, this);
							connect(socket, SIGNAL(disconnected()), server, SLOT(OnDisconnected())); 
							return;						                        
						}
						else
						{
							qDebug() << "JSON parse error!" << fileName;
						}
						//OSLL: Continues to 'default' on error...		
#endif			
					default:
						os << "HTTP/1.0 404 Error file not found\r\n\r\n";				
						qDebug() << "[404] File not found (POST): " << fileName;
			            socket->close();                
						if (socket->state() == QTcpSocket::UnconnectedState) 
						{
							socket->deleteLater();                
						}
						break;				
                }
         }
    }
}

QString QtkHttpServer::getMIMEType(QString extension)
{
    for (int i=0; i < LENGTH_OF(mimetypes); i++ )
    {
        if(strcmp(mimetypes[i].dot_extension, extension.toUtf8().data()) == 0)
        {
            return QString((char *)mimetypes[i].mimetype);
            break;
        }
    }

    return QString("text/plain");
}
	 
void QtkHttpServer::discardClient()
{
	QTcpSocket* socket = (QTcpSocket*)sender();
    socket->deleteLater();         
	
	this->m_clientCount--;
    qDebug() << "Release Remote Connection..: " << this->m_clientCount;
}

int QtkHttpServer::getFilename(QString* filename)
{

	if(filename->compare(QString("/")) == 0)
	{	
		filename->clear();		
		filename->append(QString(this->m_fileRootPath));		
		filename->append(QString(HS_WWW_DEFAULT_ROOT));		
	}
#ifdef HS_MJPG_STREAMER_ENABLE	
	else if(filename->compare(QString(HS_MJPEG_DEFAULT_URI)) == 0)
	{
		return HS_GET_MJPG_STREAM;
	}
#endif	
#ifdef HS_RPC_SERVER_ENABLE
	else if(filename->compare(QString(HS_JSRPC_DEFAULT_URI)) == 0)
	{
		return HS_POST_JSON_RPC;
	}
#endif	
	else
	{
		filename->prepend(QString(HS_WWW_DEFAULT_PATH));		
	}
	
	filename->prepend(qApp->applicationDirPath());

	return HS_GET_LOCAL_FILE  ;
}

QMap<QByteArray, QByteArray> QtkHttpServer::parseHttpHeaders(QByteArray httpHeaders)
//http://stackoverflow.com/questions/10893525/how-can-we-parse-http-response-header-fields-using-qt-c
{
	QMap<QByteArray, QByteArray> headers;
	foreach(QByteArray line, httpHeaders.split('\n')) 
	{
		int colon = line.indexOf(':');
		QByteArray headerName = line.left(colon).trimmed();
		QByteArray headerValue = line.mid(colon + 1).trimmed();
		headers.insertMulti(headerName, headerValue);
	}
	return headers;
}

QByteArray QtkHttpServer::getPostBody(QByteArray http)
{
	int pos = 0;
	int size = 0;
	pos = http.indexOf(QString("\r\n\r\n"));
	size = http.size();

	return http.right(size - pos);
}

#ifdef HS_MJPG_STREAMER_ENABLE
void QtkHttpServer::setMaxFramerate(int maxFrameRate)
{
	 this->m_maxFrameRate = maxFrameRate;
}
 
void QtkHttpServer::setMjpgUri(QString uri)
{
	this->m_mjpegUri = uri;
}

void QtkHttpServer::setVideoServer(QtkVideoServer* videoServer)
{
	this->m_videoServer = videoServer;
}
#endif

QObject* QtkHttpServer::getEventTarget(QString targetName)
{
    QObject* target;
    target = this->parent()->findChild<QObject *>(targetName);
    return target;
}
