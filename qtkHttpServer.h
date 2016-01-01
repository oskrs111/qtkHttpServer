#ifndef QTK_HTTP_SERVER_H
#define QTK_HTTP_SERVER_H
#include <QtNetwork>
#include "qtkHttpServerConfig.h"

#define HS_WWW_DEFAULT_ROOT  "/index.html"
#define HS_WWW_DEFAULT_PATH  ":/3w/"

#define HS_GET_MJPG_STREAM 	100
#define HS_GET_LOCAL_FILE  	101
#define HS_POST_JSON_RPC  	200

#ifdef HS_MJPG_STREAMER_ENABLE
#include "qtkVideoServer.h"
#include "qtkMjpgStreamer.h"
#define HS_MJPEG_DEFAULT_URI "/stream.mjpg"
#endif

#ifdef HS_RPC_SERVER_ENABLE
#include "qtkJsRpcServer.h"
#define HS_JSRPC_DEFAULT_URI "/json.rpc"
#endif


//OSLL: This struct is taken from 'httpd.h" header from output_http pluguin that is part of mjpeg-streamer application.
//OSLL: mjpeg-streamer is Tom Stöveken Copyright (C).

#define LENGTH_OF(x) (sizeof(x)/sizeof(x[0]))
static const struct {
  const char *dot_extension;
  const char *mimetype;
} mimetypes[] = {
  { "html", "text/html" },
  { "htm",  "text/html" },
  { "css",  "text/css" },
  { "js",   "text/javascript" },
  { "txt",  "text/plain" },
  { "jpg",  "image/jpeg" },
  { "jpeg", "image/jpeg" },
  { "png",  "image/png"},
  { "gif",  "image/gif" },
  { "ico",  "image/x-icon" },
  { "swf",  "application/x-shockwave-flash" },
  { "cab",  "application/x-shockwave-flash" },
  { "jar",  "application/java-archive" }
};

class QtkHttpServer : public QTcpServer
{
     Q_OBJECT
 public:
     QtkHttpServer(quint16 port, QObject* parent = 0);
     void incomingConnection(int socket);     
     void setFilesRootPath(QString path);
	 void setAppRootPath(QString path);
     QObject* getEventTarget(QString targetName);
#ifdef HS_MJPG_STREAMER_ENABLE
	 void setMjpgUri(QString uri);
	 void setVideoServer(QtkVideoServer* videoServer);
	 void setMaxFramerate(int maxFrameRate);
#endif	 

 public slots:
     void readClient();
     void discardClient();

 private:     
	 QMap<QByteArray, QByteArray> parseHttpHeaders(QByteArray httpHeaders);
	 QByteArray getPostBody(QByteArray http);
     QString getMIMEType(QString extension);
     int getFilename(QString* filename);     
     void printNetInterfaces();
	 QString m_fileRootPath;
	 QString m_appRootPath;
	 int m_clientCount;
#ifdef HS_MJPG_STREAMER_ENABLE
	 QtkVideoServer* m_videoServer;
	 QString m_mjpegUri;    
	 int m_maxFrameRate;
#endif

};
#endif
