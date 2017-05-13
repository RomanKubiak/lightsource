#include "LightsourceHTTPHandlers.h"

ESP8266WebServer httpServer(80);
ESP8266HTTPUpdateServer httpUpdater;
File fsUploadFile;
extern LightsourceRpc lightsourceRpc;

void registerHTTPHandlers()
{
	DBG("> registerHandlers\n");
 	httpServer.onNotFound(httpNotRegistered);
  	httpServer.on("/", httpLightConfig);
  	httpServer.on("/list", HTTP_GET, httpFileList);
  	httpServer.on("/edit", HTTP_GET, [](){
    	if(!httpFileRead("/edit.htm")) httpServer.send(404, "text/plain", "FileNotFound");
  	});
  	httpServer.on("/edit", HTTP_PUT, httpFileCreate);
  	httpServer.on("/edit", HTTP_DELETE, httpFileDelete);
  	httpServer.on("/edit", HTTP_POST, [](){ httpServer.send(200, "text/plain", ""); }, httpFileUpload);
  	httpServer.on("/rpc", HTTP_POST, httpRpc);
  	httpServer.on("/rpc/", HTTP_POST, httpRpc);
  	httpUpdater.setup(&httpServer);
}

void httpLightConfig()
{
  	DBG("httpLightConfig> root\n");
  	httpFileRead("/index.html");
}

void httpFileCreate()
{
  if(httpServer.args() == 0)
    return httpServer.send(500, "text/plain", "BAD ARGS");
  String path = httpServer.arg(0);
  DBG("HTTP> handleFileCreate: %s\n", path.c_str());
  if(path == "/")
    return httpServer.send(500, "text/plain", "BAD PATH");
  if(SPIFFS.exists(path))
    return httpServer.send(500, "text/plain", "FILE EXISTS");
  File file = SPIFFS.open(path, "w");
  if(file)
    file.close();
  else
    return httpServer.send(500, "text/plain", "CREATE FAILED");
  httpServer.send(200, "text/plain", "");
  path = String();
}

void httpFileDelete()
{
  if(httpServer.args() == 0) return httpServer.send(500, "text/plain", "BAD ARGS");
  String path = httpServer.arg(0);
  DBG_OUTPUT_PORT.println("HTTP> handleFileDelete " + path);
  if(path == "/")
    return httpServer.send(500, "text/plain", "BAD PATH");
  if(!SPIFFS.exists(path))
    return httpServer.send(404, "text/plain", "FileNotFound");
  SPIFFS.remove(path);
  httpServer.send(200, "text/plain", "");
  path = String();
}

void httpFileUpload()
{
  if(httpServer.uri() != "/edit") return;
  HTTPUpload& upload = httpServer.upload();
  if(upload.status == UPLOAD_FILE_START){
    String filename = upload.filename;
    if(!filename.startsWith("/")) filename = "/"+filename;
    DBG_OUTPUT_PORT.println("HTTP> handleFileUpload Name: "); DBG_OUTPUT_PORT.println(filename);
    fsUploadFile = SPIFFS.open(filename, "w");
    filename = String();
  } else if(upload.status == UPLOAD_FILE_WRITE){
    //DBG_OUTPUT_PORT.print("handleFileUpload Data: "); DBG_OUTPUT_PORT.println(upload.currentSize);
    if(fsUploadFile)
      fsUploadFile.write(upload.buf, upload.currentSize);
  } else if(upload.status == UPLOAD_FILE_END){
    if(fsUploadFile)
      fsUploadFile.close();
    DBG_OUTPUT_PORT.println("HTTP> handleFileUpload Size: "); DBG_OUTPUT_PORT.println(upload.totalSize);
  }
}

void httpFileList()
{
  DBG_OUTPUT_PORT.println("HTTP> file list handler");

  if(!httpServer.hasArg("dir")) {httpServer.send(500, "text/plain", "BAD ARGS"); return;}

  String path = httpServer.arg("dir");
  DBG_OUTPUT_PORT.println("HTTP> httpFileList " + path);
  Dir dir = SPIFFS.openDir(path);
  path = String();

  String output = "[";
  while(dir.next()){
    File entry = dir.openFile("r");
    if (output != "[") output += ',';
    bool isDir = false;
    output += "{\"type\":\"";
    output += (isDir)?"dir":"file";
    output += "\",\"name\":\"";
    output += String(entry.name()).substring(1);
    output += "\"}";
    entry.close();
  }

  output += "]";
  httpServer.send(200, "text/json", output);
}

bool httpFileRead(String path)
{
  DBG_OUTPUT_PORT.println("HTTP> handleFileRead: " + path);
  if(path.endsWith("/")) path += "index.html";
  String contentType = httpGetContentType(path);
  String pathWithGz = path + ".gz";
  if(SPIFFS.exists(pathWithGz) || SPIFFS.exists(path)){
    if(SPIFFS.exists(pathWithGz))
      path += ".gz";
    File file = SPIFFS.open(path, "r");
    size_t sent = httpServer.streamFile(file, contentType);
    file.close();
    return true;
  }
  return false;
}

void httpNotRegistered()
{
  if (!httpFileRead(httpServer.uri()))
  {
    String message = "Location not found in SPIFFS or not registsred as a callback \r\n";
    message += "URI: ";
    message += httpServer.uri();
    message += "\r\nMethod: ";
    message += (httpServer.method() == HTTP_GET)?"GET":"POST";
    message += "\r\nArguments: ";
    message += httpServer.args();
    message += "\r\n";
    for (uint8_t i=0; i<httpServer.args(); i++){
     message += " NAME:"+httpServer.argName(i) + "\n VALUE:" + httpServer.arg(i) + "\r\n";
    }
    httpServer.send(404, "text/plain", message);
    DBG_OUTPUT_PORT.println(message);
  }
}

String httpGetContentType(String filename)
{
  if(httpServer.hasArg("download")) return "application/octet-stream";
  else if(filename.endsWith(".htm")) return "text/html";
  else if(filename.endsWith(".html")) return "text/html";
  else if(filename.endsWith(".css")) return "text/css";
  else if(filename.endsWith(".map")) return "text/css";
  else if(filename.endsWith(".js")) return "application/javascript";
  else if(filename.endsWith(".png")) return "image/png";
  else if(filename.endsWith(".gif")) return "image/gif";
  else if(filename.endsWith(".jpg")) return "image/jpeg";
  else if(filename.endsWith(".ico")) return "image/x-icon";
  else if(filename.endsWith(".xml")) return "text/xml";
  else if(filename.endsWith(".pdf")) return "application/x-pdf";
  else if(filename.endsWith(".zip")) return "application/x-zip";
  else if(filename.endsWith(".gz")) return "application/x-gzip";
  else if(filename.endsWith(".json")) return "application/json";
  return "text/plain";
}

void httpRpc()
{
  DBG_OUTPUT_PORT.println("HTTP> httpRpc");
  lightsourceRpc.process(httpServer);
}
