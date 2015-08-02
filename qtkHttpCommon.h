#ifndef QTK_HTTP_COMMON_STREAMER_H
#define QTK_HTTP_COMMON_STREAMER_H

//OSLL: This macro is taken from 'httpd.h" header from output_http pluguin that is part of mjpeg-streamer application.
//OSLL: mjpeg-streamer is Tom Stöveken Copyright (C).
#define STD_HEADER "Connection: close\r\n" \
                   "Server: QtkJsRpcServer-1.0\r\n" \
                   "Cache-Control: no-store, no-cache, must-revalidate, pre-check=0, post-check=0, max-age=0\r\n" \
                   "Pragma: no-cache\r\n" \
                   "Expires: Mon, 3 Jan 2000 12:34:56 GMT\r\n"			  
#endif				   