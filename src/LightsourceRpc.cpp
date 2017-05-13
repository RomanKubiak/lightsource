#include "LightsourceRpc.h"

void LightsourceRpc::begin()
{
	DBG("LightsourceRpc::begin> \n");
}

void LightsourceRpc::process(ESP8266WebServer &srv)
{
	StaticJsonBuffer<1024> jsonBuffer;
	JsonObject& jsonRpcRequest = jsonBuffer.parseObject(srv.arg(0));
	DBG("LightsourceRpc::process>\n");
	jsonRpcRequest.prettyPrintTo(Serial);

	if (strcmp(jsonRpcRequest["method"], "lightsource.init") == 0)
		srv.send(200, JSON_CONTENT_TYPE, init(jsonRpcRequest));
 	if (strcmp(jsonRpcRequest["method"], "lightsource.testProgram") == 0)
  		srv.send(200, JSON_CONTENT_TYPE, testProgram(jsonRpcRequest));
	if (strcmp(jsonRpcRequest["method"], "lightsource.writeProgram") == 0)
		srv.send(200, JSON_CONTENT_TYPE, writeProgram(jsonRpcRequest));
	if (strcmp(jsonRpcRequest["method"], "lightsource.writeDefaultProgram") == 0)
		srv.send(200, JSON_CONTENT_TYPE, writeDefaultProgram(jsonRpcRequest));
	else
		srv.send(200, JSON_CONTENT_TYPE, error(jsonRpcRequest["id"], -1, "Unknown metod"));
}

String LightsourceRpc::init(JsonObject &request)
{
	String configString = getFileContents("/default.json");

	if (configString.length() > 0)
		return (success(request["id"], configString));
	else
		return (error(request["id"], -2, "config.json file read failed, init defaults"));
}

String LightsourceRpc::testProgram( JsonObject &request)
{
	return (success(request["id"]));
}

String LightsourceRpc::writeDefaultProgram( JsonObject &request)
{
	if (lightsourceStrips.updateProgram(request,"default"))
  		return (success(request["id"]));
	else
		return (error(request["id"]));
}

String LightsourceRpc::writeProgram(JsonObject &request)
{
	return (success(request["id"]));
}

String LightsourceRpc::success(int id, String response)
{
	if (response.length() == 0)
		return JSONRPC_PREFIX(id) + "\"result\": 0}";
	else
		return JSONRPC_PREFIX(id) + "\"result\": "+response+"}";
}

String LightsourceRpc::error(int id, int code, String message)
{
	return (JSONRPC_PREFIX(id) +
		"\"error\": [\"code\":"+
		String(code)+", \"message\": \""+message+"\"]}");
}
