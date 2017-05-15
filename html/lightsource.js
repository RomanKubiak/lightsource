var NEOTYPES = [ "NEO_RGB", "NEO_RBG", "NEO_GRB", "NEO_GBR", "NEO_BRG",
	"NEO_BGR", "NEO_WRGB", "NEO_WRBG", "NEO_WGRB", "NEO_WGBR", "NEO_WBRG",
	"NEO_WBGR", "NEO_RWGB", "NEO_RWBG", "NEO_RGWB", "NEO_RGBW", "NEO_RBWG",
	"NEO_RBGW", "NEO_GWRB", "NEO_GWBR", "NEO_GRWB", "NEO_GRBW", "NEO_GBWR",
	"NEO_GBRW", "NEO_BWRG", "NEO_BWGR", "NEO_BRWG", "NEO_BRGW", "NEO_BGWR",
	 "NEO_BGRW" ];

var ESPPINS = { "D0":16, "D1":5,
				"D2":4,  "D3":0,
				"D4":2,  "D5":14,
				"D6":12, "D7":13,
				"D8":15, "D9":3,
				"D10":1 };

$(document).ready(function ()
{
    stripTemplate                   = $.templates("#lightsource-strip-template");
    sliceTemplate                   = $.templates("#lightsource-slice-template");
    $.templates("lightsource-slice-template", "#lightsource-slice-template");

    $(".lightsource-test-program").click(function (e){
		console.log($('form#lightsource-config-form').serializeObject().strips);
        $.jsonRPC.request("testProgram",
            {
                params:[$('form#lightsource-config-form').serializeObject().strips],
                success: testProgramSuccess,
                error: testProgramError
            }
        );
    });

	$(".lightsource-write-program").click(function (e){
		$.jsonRPC.request("writeProgram",
            {
                params: [ $("#currentProgramName").val(),
				$('form#lightsource-config-form').serializeObject().strips ],
                success: writeProgramSuccess,
                error: writeProgramError
            }
        );
    });

	$.jsonRPC.setup({
    	endPoint: "/rpc/",
    	namespace: "lightsource"
	});

	$.jsonRPC.request("init",
	{
		success: programReceived,
		error: programReceptionError
	});

	$.jsonRPC.request("programList",
	{
		success: programListReceived,
		error: programListReceptionError
	});
});

function addStrip(e)
{
    var helper =
	{
		n: lastStripIndex,
		esppins: ESPPINS,
		neotypes: NEOTYPES
	};

    $("#lightsource-config-form").append(stripTemplate.render({strip: createEmptyStrip()}, helper));
    lastStripIndex++;
}

function addSlice(e)
{
    var lastElement = $(e).parent().parent().prev().attr("sliceIndex");
    if (lastElement == null)
        lastElement = 0;

    var n = e.getAttribute("stripIndex");
    var helper = {
        n: n,
        slice: createEmptySlice(),
        getSliceIndex: function(index)
        {
            return parseInt(lastElement) + 1;
        }
    };

    $(sliceTemplate.render({},helper)).insertBefore($(e).parent().parent());
}

function removeSlice(e)
{
    $(e).parent().parent().remove();
}

function removeStrip(e)
{
    $(e).parent().parent().parent().parent().remove();
}

function createEmptySlice()
{
    return ["new slice", "#00ff00", 0, 8];
}

function createEmptyStrip()
{
    return [0, "new strip", "#ff0000", 0, 16, []];
}

function renderStripsTable(strips)
{
    for (n=0; n<strips.length; n++)
    {
        var helper =
		{
			n: n,
			getSliceIndex: function(index) { return index; },
			esppins: ESPPINS,
			neotypes: NEOTYPES
		};

        $("#lightsource-config-form").append(stripTemplate.render({strip: strips[n]}, helper));
    }

    lastStripIndex = n;
}

function lightsourceError(message)
{
	console.log(message);
	$("#lightsource-notifier").attr("class", "label label-danger");
	$("#lightsource-notifier").html(message);
	$("#lightsource-notifier").show();
	$("#lightsource-notifier").fadeOut(2200);
}

function lightsourceNotify(message)
{
	console.log(message);
	$("#lightsource-notifier").attr("class", "label label-success");
	$("#lightsource-notifier").html(message);
	$("#lightsource-notifier").show();
	$("#lightsource-notifier").fadeOut(2200);
}

function existingProgramSelected(o)
{
	console.log($(o).val());
	$.jsonRPC.request("init",
	{
		params: [$(o).val()],
		success: programReceived,
		error: programReceptionError
	});
}

function programReceived(o)
{
    lightsourceNotify("Received remote program");
    console.log(o);

    if (o.result.length == 2)
    {
        renderStripsTable(o.result[1]);
		$("#currentProgramName").val(o.result[0]);
    }
	else
	{
		renderStripsTable(o.result[0]);
		$("#currentProgramName").val("default");
	}
}

function programReceptionError(o)
{
    lightsourceError("Error receiving remote program: \"" + o.error.message + "\"");
    console.log(o);
}

function writeProgramSuccess(o)
{
    lightsourceNotify("Write program success");
    console.log(o);
}

function writeProgramError(o)
{
    lightsourceError("Write program failed: \"" + o.error.message + "\"");
    console.log(o);
}

function testProgramSuccess(o)
{
    lightsourceNotify("Test program success");
    console.log(o);
}

function testProgramError(o)
{
    lightsourceError("Test program failed: \"" + o.error.message + "\"");
    console.log(o);
}

function programListReceived(o)
{
	var programList = $("#lightsource-program-list");
	$.each(o.result, function()
	{
    	programList.append($("<option />").val(this[0]).text(this[0]));
	});
	console.log(o);
}

function programListReceptionError(o)
{
	lightsourceError("Failed to receive program list: \"" + o.error.message + "\"");
    console.log(o);
}
