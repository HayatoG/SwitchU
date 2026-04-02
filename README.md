<div align="center">
    <h1>SwitchU</h1>
    <p>A Wii U-style custom home menu replacement for Nintendo Switch</p>
</div>

<p align="center">
  <a rel="LICENSE" href="https://github.com/PoloNX/SwitchU/blob/master/LICENSE">
    <img src="https://img.shields.io/static/v1?label=license&message=GPLV3&labelColor=111111&color=0057da&style=for-the-badge" alt="License">
    </a>
    <a rel="VERSION" href="https://github.com/PoloNX/SwitchU/releases">
        <img src="https://img.shields.io/static/v1?label=version&message=1.0.0&labelColor=111111&color=06f&style=for-the-badge" alt="Version">
    </a>
</p>

---

- [Architecture](#architecture)
- [Screenshots](#screenshots)
- [How to build](#how-to-build)
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


Adicionar aos planos:
1 - Folders ficarem sempre em primeiro na lista
2 - Adicionar filtro para mostrar apenas folders
3 - Adicionar possibilidade de mudar o grid das folders para elas ficarem maiores e exibir menos folders na tela por página
4 - Adicionar preview fora da folder dos jogos que estao naquela pasta ou adicionar a letra inicial ou as iniciais do nome da folder
5 - Adicionar botao para editar folder (nome e excluir jogos diretamente desse menu)
6 - Retirar do menu de detalhes das informaćoes dos jogos o folder manager
7 - Adicionar um botao abaixo do miiverse para criar uma folder nova
8 - Adicionar um botao de configuracoes de folders gerais para poder excluir todas por exemplo e retirar os jogos das folders e exibir todos os jogos na tela
9 - As folders que já existem nao aparecem ao ligar o console, isso precisa ser exibido desde o comeco
10 - Adicionar o efeito de glass/blur nas folders (botao de on/off nas configuraćoes gerais das folders)
11 - Adicionar botao de selecionar preview de icones dos jogos ou iniciais dos nomes da pasta
12 - Ao clicar na pasta, abrir uma janela com os jogos que estao nela para poder iniciar ou ver detalhes
13 - FUTURAMENTE (nao enviar) um modo de grid onde os jogos ficam em baixo em uma linha só e em cima fica a preview da pasta com os icones dos jogos passando em carroussel ao passar por cima da pasta
