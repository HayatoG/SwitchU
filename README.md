<div align="center">
    <h1>SwitchU</h1>
    <p>A Wii U-style custom home menu replacement for Nintendo Switch</p>
</div>

<p align="center">
  <a rel="LICENSE" href="https://github.com/PoloNX/SwitchU/blob/master/LICENSE">
    <img src="https://img.shields.io/static/v1?label=license&message=GPLV3&labelColor=111111&color=0057da&style=for-the-badge" alt="License">
    </a>
    <a rel="VERSION" href="https://github.com/PoloNX/SwitchU/releases">
      <img src="https://img.shields.io/static/v1?label=version&message=1.0.1&labelColor=111111&color=06f&style=for-the-badge" alt="Version">
    </a>
</p>

---

- [Architecture](#architecture)
- [Screenshots](#screenshots)
- [How to build](#how-to-build)
- [Release (GitHub)](#release-github)
- [Installation](#installation)
- [SD card layout](#sd-card-layout)
- [Known issues](#known-issues)
- [Help me](#help-me)
- [Credits](#credits)
- [License](#license)


## Architecture

- `switchu-daemon` (System Applet, title ID `0x0100000000001000`)
  - Replaces qlaunch
  - Handles lifecycle, HOME/suspend-resume flow, sleep/reboot and IPC
- `switchu-menu` (Library Applet, title ID `0x010000000000100B`)
  - Renders the full UI
  - Communicates with daemon through AppletStorage + `swu:m` notifications
- `SwitchU` (Homebrew mode)
  - Monolithic `.nro` target for standalone usage/testing

## Screenshots

![](./screenshots/1.jpg)

<details>
  <summary><b>More screenshots</b></summary>

![](./screenshots/2.jpg)
![](./screenshots/3.jpg)
![](./screenshots/4.jpg)
![](./screenshots/5.jpg)

</details>

## How to build

### Requirements

- [devkitPro](https://devkitpro.org/wiki/Getting_Started)
- [Xmake](https://xmake.io/#/)

### Clone

```bash
git clone --recursive https://github.com/PoloNX/SwitchU
cd SwitchU
```

### Build with Docker (recommended)

The easiest way to build without setting up the toolchain manually:

```bash
docker build -t switchu-builder .
./docker-build.sh
```

### Build manually (production two-applet mode)

```bash
xmake f -p cross --toolchain=devkita64
xmake
```

### Build (homebrew .nro mode)

```bash
xmake f -p cross --toolchain=devkita64 --homebrew=y
xmake
```

### Build (SDL2 backend)

```bash
xmake f -p cross --toolchain=devkita64 --backend=sdl2
xmake
```

### Clean

```bash
xmake clean
```

Build outputs are generated under `build/cross/none/release/`.

## Release (GitHub)

Tag format:

```bash
v1.0.1
```

When a tag matching `v*` is pushed, GitHub Actions builds and publishes a release with:

- `SwitchU-<version>-sdmc.zip`
- `SwitchU-<version>-switchu-daemon.nsp`
- `SwitchU-<version>-switchu-menu.nsp`
- `SwitchU-<version>-sha256.txt`

`SwitchU-<version>-sdmc.zip` already includes the installation layout:

```text
sdmc:/atmosphere/contents/0100000000001000/exefs.nsp
sdmc:/atmosphere/contents/0100000000001000/flags/boot2.flag
sdmc:/atmosphere/contents/010000000000100B/exefs.nsp
sdmc:/switch/SwitchU/fonts/*
sdmc:/switch/SwitchU/i18n/*
sdmc:/switch/SwitchU/icons/*
sdmc:/switch/SwitchU/shaders/*
sdmc:/switch/SwitchU/sounds/*
```

Manual packaging (local):

```bash
./docker-build.sh
./scripts/package-release.sh 1.0.1
```

## Installation

> **Requirements:** Nintendo Switch with [Atmosphère](https://github.com/Atmosphere-NX/Atmosphere) CFW installed.

> ⚠️ **Warning:** This replaces the Switch HOME menu entirely. If something goes wrong, boot into Hekate and delete the files via its file manager before booting normally.

### 1. Copy the NSPs

After building, copy the output files to your SD card, renaming them as shown:

| Built file | SD card destination |
|---|---|
| `build/cross/none/release/switchu-daemon.nsp` | `sdmc:/atmosphere/contents/0100000000001000/exefs.nsp` |
| `build/cross/none/release/switchu-menu.nsp` | `sdmc:/atmosphere/contents/010000000000100B/exefs.nsp` |

### 2. Create the boot2 flag

Create an empty file at the following path so Atmosphère loads the daemon on boot:

```
sdmc:/atmosphere/contents/0100000000001000/flags/boot2.flag
```

### 3. Reboot

Insert the SD card, boot with CFW active, and SwitchU will replace the HOME menu.

### Uninstalling

Delete the two content folders and reboot:

```
sdmc:/atmosphere/contents/0100000000001000/
sdmc:/atmosphere/contents/010000000000100B/
```

## SD card layout

```
sdmc:/
├── atmosphere/
│   └── contents/
│       ├── 0100000000001000/        ← Daemon (replaces qlaunch)
│       │   ├── exefs.nsp
│       │   └── flags/
│       │       └── boot2.flag
│       └── 010000000000100B/        ← Menu (library applet)
│           └── exefs.nsp
├── config/
│   └── SwitchU/                     ← created automatically at runtime
│       ├── config.ini               ← user settings
│       ├── applist.bin              ← app metadata cache
│       ├── daemon.log               ← daemon runtime log
│       └── menu.log                 ← menu runtime log
└── switch/
    └── SwitchU/                     ← optional custom assets
```

## Known issues

- Some settings are not implemented yet
- Current icons are very ugly, feel free to replace them with better ones
- SDL2 backend is very buggy and incomplete, use it for testing only
- You may experience some crashes when using overlays

## Help me

If you want to help, open an issue when you find a bug and open a pull request if you have a fix.

## Credits

- Thanks to [Xortroll](https://github.com/Xortroll) for the help and for [uLaunch](https://github.com/Xortroll/uLaunch) which inspired this project a lot

## License

This project is licensed under the GNU General Public License v3.0. See the [LICENSE](https://github.com/PoloNX/SwitchU/blob/master/LICENSE) file for details.