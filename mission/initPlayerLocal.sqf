if (!hasInterface) exitWith {};

"arma3il" callExtension ["whitelist_add", [
    "i.goopics.net",
    "i.imgur.com",
    "i.ibb.co",
    "i.postimg.cc",
    "zupimages.net",
    "cdn.discordapp.com"
]];

addMissionEventHandler ["ExtensionCallback", {
    _this spawn Arma3il_fnc_OnExtensionCallback;
}];