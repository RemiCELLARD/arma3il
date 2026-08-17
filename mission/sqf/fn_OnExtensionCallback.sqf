private ["_parts","_titleVar","_title","_reason","_msg"];
params [
	["_name", "", [""]],
	["_function", "", [""]],
	["_data", "", [""]]
];

if (_name isNotEqualTo "arma3il") exitWith {};
if (_function isNotEqualTo "download") exitWith {};

_parts = parseSimpleArray _data;
_parts params [
    ["_status", "", [""]],
    ["_uniqueName", "", [""]]
];

switch (_status) do {
    case "done": {
        _parts params [
            "",
            "",
            ["_texWidth", 0, [0]],
            ["_texHeight", 0, [0]],
            ["_origWidth", 0, [0]],
            ["_origHeight", 0, [0]]
        ];

        _titleVar = format ["arma3il_title_%1", _uniqueName];
        _title = missionNamespace getVariable [_titleVar, ""];
        missionNamespace setVariable [_titleVar, nil];

        [_uniqueName, _texWidth, _texHeight, _origWidth, _origHeight, _title] call Arma3il_fnc_DisplayImage;
    };
    case "error": {
        _reason = _parts select 2;
        _msg = switch (true) do {
            case (_reason isEqualTo "decode_failed"): { localize "STR_A3IL_Errors_Decode_Failed" };
            case (_reason isEqualTo "http_404"): { localize "STR_A3IL_Errors_HTTP_404" };
            case (_reason isEqualTo "http_403"): { localize "STR_A3IL_Errors_HTTP_403" };
            case (_reason find "http_" isEqualTo 0): { format [localize "STR_A3IL_Errors_HTTP_XXX", _reason select [5]] };
            case (_reason find "curl_error_" isEqualTo 0): { format [localize "STR_A3IL_Errors_CURL_Err", _reason select [11]] };
            default { _reason };
        };
        hint format [localize "STR_A3IL_Errors_Default_Download_Error", _uniqueName, _msg];
    };
};