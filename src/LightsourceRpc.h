#ifndef LIGHTSOURCE_RPC_H
#define LIGHTSOURCE_RPC_H

#include "LightsourceStrips.h"
#include "LightsourceUtils.h"

#define JSON_CONTENT_TYPE	 "application/json-rpc"
#define JSONRPC_PREFIX(id) 	 String("{\"jsonrpc\": \"2.0\", \"id\":\""+String(id)+"\", ")
class LightsourceRpc
{
	public:
		LightsourceRpc(){}
		~LightsourceRpc(){}
		void begin();
		void process(ESP8266WebServer &srv);
		String init(JsonObject &request);
		String testProgram(JsonObject &request);
		String programList(JsonObject &request);
		String writeProgram(JsonObject &request);
	private:
		LightsourceStrips lightsourceStrips;
		static String success(int id, String responseBody="");
		static String error(int id, int code=-1, String message="Unknow error");
};
#endif
