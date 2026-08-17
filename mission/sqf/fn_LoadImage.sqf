private ["_data","_parts","_reason","_msg"];
params [
    ["_url", "", [""]],
    ["_title", "", [""]]
];

if (!hasInterface) exitWith {};
if (_url isEqualTo "") exitWith { hint localize "STR_A3IL_Errors_UrlRequired"; };

_data = "arma3il" callExtension ["download", [_url]];
_parts = parseSimpleArray (_data select 0);
_parts params [
    ["_status", "", [""]],
    ["_uniqueName", "", [""]]
];

if (_status isEqualTo "loading") exitWith {
    missionNamespace setVariable [format ["arma3il_title_%1", _uniqueName], _title];
    _uniqueName
};

_reason = _parts select 2;
_msg = switch (true) do {
    case (_reason isEqualTo "missing_args"): { localize "STR_A3IL_Errors_MissingArgs_CallExtension" };
    case (_reason isEqualTo "invalid_extension"): { localize "STR_A3IL_Errors_FileExtensionNotSupported" };
    case (_reason isEqualTo "not_whitelisted_mission"): { localize "STR_A3IL_Errors_DomainName_NotAllowedByServer" };
    case (_reason isEqualTo "not_whitelisted_client"): { localize "STR_A3IL_Errors_DomainName_NotAllowedByClient" };
    case (_reason isEqualTo "http_404"): { localize "STR_A3IL_Errors_HTTP_404" };
    case (_reason isEqualTo "http_403"): { localize "STR_A3IL_Errors_HTTP_403" };
    case (_reason find "http_" isEqualTo 0): { format [localize "STR_A3IL_Errors_HTTP_XXX", _reason select [5]] };
    case (_reason find "curl_error_" isEqualTo 0): { format [localize "STR_A3IL_Errors_CURL_Err", _reason select [11]] };
    default { _reason };
};
hint format [localize "STR_A3IL_Errors_Default_VerifyLink_Error", _uniqueName, _msg];