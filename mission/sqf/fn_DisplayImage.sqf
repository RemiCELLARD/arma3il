private ["_displayCfg","_displayIdd","_display","_backgroundCtrl","_titleCtrl""_closeBtn","_closeSize","_closeMargin"];
private ["_padding","_availW","_availH","_titleRowH","_rowGap","_availImageH","_nativeW","_nativeH","_scale","_finalW","_finalH","_blockH","_posX","_posY","_rowY","_texture","_pictureCtrl"];
params [
    ["_uniqueName", "", [""]],
    ["_texWidth", 0, [0]],
    ["_texHeight", 0, [0]],
    ["_origWidth", 0, [0]],
    ["_origHeight", 0, [0]],
    ["_title", "", [""]]
];

_displayCfg = missionConfigFile >> "A3IL_ImageViewer";
_displayIdd = getNumber(_displayCfg >> "idd");

if (isNull findDisplay _displayIdd) then { createDialog "A3IL_ImageViewer"; };
_display = findDisplay _displayIdd;
if (isNull _display) exitWith { 
    systemChat localize "STR_A3IL_Errors_UnableOpenDisplay";
};

// Safezone : 10% margin
_padding = 0.1;
_availW = safezoneW * (1 - 2 * _padding);
_availH = safezoneH * (1 - 2 * _padding);

// Title : max 2 lines
_titleRowH = 0.08;
_rowGap = 0.015;
_availImageH = _availH - _titleRowH - _rowGap;

_nativeW = _origWidth * pixelW;
_nativeH = _origHeight * pixelH;

_scale = 1 min (_availW / _nativeW) min (_availImageH / _nativeH);

_finalW = _nativeW * _scale;
_finalH = _nativeH * _scale;

_blockH = _finalH + _rowGap + _titleRowH;
_posX = safezoneX + ((safezoneW - _finalW) / 2);
_posY = safezoneY + ((safezoneH - _blockH) / 2);
_rowY = _posY + _finalH + _rowGap;

_texture = format ["#(argb,%1,%2,1)extension(""arma3il"",""%3"", 0, CA)", _texWidth, _texHeight, _uniqueName];

_pictureCtrl = _display displayCtrl getNumber(_displayCfg >> "controls" >> "Picture" >> "idc");
_pictureCtrl ctrlSetText _texture;
_pictureCtrl ctrlSetPosition [_posX, _posY, _finalW, _finalH];
_pictureCtrl ctrlCommit 0;

_titleCtrl = _display displayCtrl getNumber(_displayCfg >> "controls" >> "Title" >> "idc");
_titleCtrl ctrlSetPosition [safezoneX, _rowY, safezoneW, _titleRowH];
_titleCtrl ctrlCommit 0;
_titleCtrl ctrlSetStructuredText (parseText format ["<t align='center'>%1</t>", _title]);