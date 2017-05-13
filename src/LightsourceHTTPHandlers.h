#ifndef LIGHTSOURCE_HTTPHANDLERS_H
#define LIGHTSOURCE_HTTPHANDLERS_H
#include "LightsourceRpc.h"

/* HTTP callbacks */
void httpLightConfig();
void httpFileList();
String httpGetContentType(String filename);
bool httpFileRead(String path);
void httpFileCreate();
void httpFileDelete();
void httpFileUpload();
void httpNotRegistered();
void httpRpc();
void registerHTTPHandlers();
#endif
