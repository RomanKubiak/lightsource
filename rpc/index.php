<?php
$json = json_decode(file_get_contents("php://input"), true);
header("Content-type: application/json-rpc");

if ($json["method"] == "lightsource.init")
{
	if ($json["params"][0] != "")
	{
		$f = sprintf("/data/devel/lightsource/html/%s.json", $json["params"][0]);
		if (!file_exists($f)) {
			printf ('{"jsonrpc": "2.0", "id": "1", "result": -2, "error":{"message":"Program %s does not exist"}}', $json["params"][0]);;
			exit (0);
		}
		$d = file_get_contents($f);
	}
	else
	{
		$d = file_get_contents("/data/devel/lightsource/html/default.json");# code...
	}

    echo '{"jsonrpc": "2.0", "id": "1", "result":';
    echo $d;
    echo '}';
}
else if ($json["method"] == "lightsource.programList")
{
    printf ('{"jsonrpc": "2.0", "id": "1", "result":%s}', file_get_contents("/data/devel/lightsource/html/programs.json"));
}
else
{
    echo '{"jsonrpc": "2.0", "id": "1", "result": -1, "error":{"message":"WTF?"}}';
}
?>
