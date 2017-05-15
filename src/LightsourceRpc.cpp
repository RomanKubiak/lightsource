#include "LightsourceRpc.h"

void LightsourceRpc::begin()
{
	DBG("LightsourceRpc::begin> \n");
}

void LightsourceRpc::process(ESP8266WebServer &srv)
{
	StaticJsonBuffer<2048> jsonBuffer;
	JsonObject& jsonRpcRequest = jsonBuffer.parseObject(srv.arg(0));
	DBG("LightsourceRpc::process>\n");
	jsonRpcRequest.prettyPrintTo(Serial);
	DBG("\n");
	if (strcmp(jsonRpcRequest["method"], "lightsource.init") == 0)
		srv.send(200, JSON_CONTENT_TYPE, init(jsonRpcRequest));
 	if (strcmp(jsonRpcRequest["method"], "lightsource.testProgram") == 0)
  		srv.send(200, JSON_CONTENT_TYPE, testProgram(jsonRpcRequest));
	if (strcmp(jsonRpcRequest["method"], "lightsource.writeProgram") == 0)
		srv.send(200, JSON_CONTENT_TYPE, writeProgram(jsonRpcRequest));
	if (strcmp(jsonRpcRequest["method"], "lightsource.programList") == 0)
		srv.send(200, JSON_CONTENT_TYPE, programList(jsonRpcRequest));
	else
		srv.send(200, JSON_CONTENT_TYPE, error(jsonRpcRequest["id"], -1, "Unknown metod"));
}

String LightsourceRpc::init(JsonObject &request)
{
	String configString;

	if (request["params"].size() <= 0)
		configString = getFileContents("/default.json");
	else if (request["params"].size() == 1)
		configString = getFileContents(_STR("/")+_STR((const char *)request["params"][0])+_STR(".json"));
	else
		return (error(request["id"], -3, "Invalid number of request parameters"));

	if (configString.length() > 0)
		return (success(request["id"], configString));
	else
		return (error(request["id"], -2, "Program file read failed, init defaults"));
}

String LightsourceRpc::testProgram(JsonObject &request)
{
	DBG("> LightsourceRpc::testProgram\n");
	request.printTo(Serial);
	DBG("\n");
	if (lightsourceStrips.applyProgram((const JsonVariant &)request["params"][0]))
		return (success(request["id"]));
	else
		return (error(request["id"], -4, "Can't test program"));
}

String LightsourceRpc::programList(JsonObject &request)
{
	String programs = getFileContents("/programs.json");

	if (programs.length() > 0)
  		return (success(request["id"], programs));
	else
		return (error(request["id"]));
}

String LightsourceRpc::writeProgram(JsonObject &request)
{
	DBG("> writeProgram\n");

	if (request["params"].size() == 2)
	{
		String programName = request["params"][0];
		JsonObject &program = request["params"][1];
		DBG("\t> program name: \"%s\"", programName.c_str());

		if (lightsourceStrips.updateProgram(program, programName))
			return success(request["id"]);
		else
			return error(request["id"], -4, "Can't write program");
	}

	return (error(request["id"], -5, "Not enough parameters"));
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
		"\"error\": {\"code\": "
		+ String(code)
		+ ", \"message\": \""+message+"\"} }");
}
