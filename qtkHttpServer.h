#ifndef QTK_HTTP_SERVER_H
#define QTK_HTTP_SERVER_H
#include <QtNetwork>
#include "qtkvideoserver.h"

#define HS_MJPEG_DEFAULT_URI "/stream.mjpg"
#define HS_WWW_DEFAULT_ROOT  "/index.html"
#define HS_WWW_DEFAULT_PATH  "/3w"
#define HS_GET_MJPG_STREAM 1
#define HS_GET_LOCAL_FILE  2

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
     void setMjpgUri(QString uri);
     void setFilesRootPath(QString path);
	 void setAppRootPath(QString path);
     void setVideoServer(QtkVideoServer* videoServer);
	 void setMaxFramerate(int maxFrameRate);

 public slots:
     void readClient();
     void discardClient();

 private:     
     QString getMIMEType(QString extension);
     int getFilename(QString* filename);
     QtkVideoServer* m_videoServer;
	 QString m_fileRootPath;
	 QString m_appRootPath;
	 QString m_mjpegUri;    
	 int m_maxFrameRate;
	 int m_clientCount;
};
#endif
