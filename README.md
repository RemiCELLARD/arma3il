# Arma 3 Image Loader

**Arma 3 Image Loader** (`arma3il`) is a native Windows extension for [Arma 3](https://arma3.com/) that lets missions display online-hosted images directly in the game.

The repository contains:

- The C++ extension (`extension/`);
- SQF scripts, the user interface, and translations for an example mission (`mission/`).

## Features

- Asynchronous HTTPS image downloads without blocking the game.
- Support for **JPEG** (`.jpg`, `.jpeg`, `.jpe`, `.jfif`, `.jif`) and **PNG** images.
- An in-game viewer that scales images to fit the screen.
- Temporary in-memory caching as Arma 3 textures.
- Two independent allowlists for permitted hosts: one configured by the mission and one stored locally by the client.
- A 5 MB download limit and HTTP redirects disabled.
- Error messages localized in English and French.

## Requirements

### To use the extension

- Arma 3 on Windows;
- The DLL built for the game architecture: `arma3il_x64.dll` for 64-bit, or `arma3il.dll` for 32-bit;
- The mission files integrated into your mission or mod;
- A direct HTTPS URL to a JPEG or PNG image permitted by both allowlists.

### To build from source

- CMake 3.25 or later;
- `vcpkg`, with the `VCPKG_ROOT` environment variable configured;
- Visual Studio and the C++ build tools for Windows;
- Git and Internet access, required to retrieve `stb` during configuration.

`libcurl` is installed automatically through `vcpkg`. The project uses C++ 17.

## Installing and integrating the mission files

1. Build the extension (see the next section) and place the generated DLL where Arma 3 loads your extensions.
2. Copy the contents of `mission/` into your mission, or adapt its files to your project structure.
3. Keep the `CfgFunctions` declarations, dialogs, and `stringtable.xml` defined in `mission/description.ext`.
4. In a script that runs for every client, add trusted domains and register the callback handler. `mission/initPlayerLocal.sqf` provides this base configuration.
5. Call the SQF loading function with a URL and, optionally, a title:

```sqf
[
    "https://i.imgur.com/exemple.png",
    "Mission image"
] call Arma3il_fnc_LoadImage;
```

The function returns an image identifier while the image is loading. Once the download completes, the extension callback automatically opens the viewer.

### Allowing an image host

A domain must appear in **both** lists below. This double validation prevents a mission from loading an arbitrary host that the player has not permitted.

**Mission allowlist** — change the domains passed to `whitelist_add`, for example in `initPlayerLocal.sqf`:

```sqf
"arma3il" callExtension ["whitelist_add", [
    "i.imgur.com",
    "cdn.discordapp.com",
    "your-domain.example"
]];
```

**Client allowlist** — on first load, the extension creates `arma3il_whitelist.txt` next to the DLL. Add one domain per line, then reload the list if the game is already running:

```sqf
"arma3il" callExtension ["whitelist_client_reload", []];
```

The default domains are `i.goopics.net`, `i.imgur.com`, `i.ibb.co`, `i.postimg.cc`, `zupimages.net`, and `cdn.discordapp.com`.

## Building

From the `extension/` directory, configure and build the required version:

```powershell
cmake --preset windows-x64
cmake --build --preset windows-x64
```

For a 32-bit build:

```powershell
cmake --preset windows-x86
cmake --build --preset windows-x86
```

Release binaries are placed under `extension/build/…/Release/`. The output name is `arma3il_x64.dll` for 64-bit builds and `arma3il.dll` for 32-bit builds.

### Tests

The unit tests cover URL validation and allowlists, among other components. With Ninja installed:

```powershell
cmake --preset tests-ninja
cmake --build --preset tests-ninja
ctest --preset tests-ninja
```

## Extension commands

| Command | Description |
| --- | --- |
| `download` | Downloads and decodes an image, then notifies the mission when the operation finishes. |
| `whitelist_add` | Adds hosts to the mission allowlist. |
| `whitelist_clear` | Clears the mission allowlist. |
| `whitelist_client_reload` | Reloads the local `arma3il_whitelist.txt` file. |
| `texture_cache_clear` | Clears the texture cache, or removes the entry specified as an argument. |

## License

This project is distributed under the [MIT License](LICENSE). You may use, modify, and redistribute it, provided that the copyright notice and license are retained.

## Contributors

- [Rémi CELLARD](https://github.com/RemiCELLARD) — author and maintainer.

Contributions are welcome: open an issue to discuss an improvement or submit a pull request on GitHub.
