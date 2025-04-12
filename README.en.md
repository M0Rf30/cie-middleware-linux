<div align="center">
  <img src="data/app.m0rf30.cieid.svg" alt="App Logo" height="200" />
  <!-- Spacer -->
  <br /><br />

  <!-- Centered language switcher -->
  <p>
    <a href="./README.en.md">English</a> |
    <a href="./README.md">Italiano</a>
  </p>
  <h1 align="center">Software for the Italian Electronic Identity Card</h1>
    <!-- Build status badges -->
    <p>
        <img src="https://github.com/M0Rf30/cie-middleware-linux/actions/workflows/main.yml/badge.svg" alt="Build Status">
        <img src="https://img.shields.io/badge/packaged%20with-yap-orange?url=https://left&link=github.com/packagefoundation/yap&logo=data:image/svg+xml;base64,PD94bWwgdmVyc2lvbj0iMS4wIiBlbmNvZGluZz0iVVRGLTgiPz4KPHN2ZyBjbGFzcz0iY3NzLTFqOG82OGYiIHdpZHRoPSIyOTIuNTkiIGhlaWdodD0iMjUzLjU2IiB2ZXJzaW9uPSIxLjEiIHZpZXdCb3g9IjAgMCAyNjAgMjI1LjMyIiB4bWxucz0iaHR0cDovL3d3dy53My5vcmcvMjAwMC9zdmciPgo8ZyB0cmFuc2Zvcm09Im1hdHJpeCgyLjcxNTUgMCAwIDIuMzU0OSAtNS43NiAtNS4wODU4KSIgZmlsbD0iIzRlY2NhMyI+CjxwYXRoIGQ9Im02LjcxIDU4LjQ3IDcuMTcgMS4yNmEzNy4xNSAzNy4xNSAwIDAgMCAzLjcgOC45M2wtNC4xOCA2YTUuNTEgNS41MSAwIDAgMCAwLjYgNy4wMmw0LjMgNC4zYTUuNTEgNS41MSAwIDAgMCA3LjA2IDAuNjFsNi00LjE4YTM3LjE0IDM3LjE0IDAgMCAwIDguOTMgMy43bDEuMjYgNy4xN2E1LjUxIDUuNTEgMCAwIDAgNS40NSA0LjU2aDZhNS41MSA1LjUxIDAgMCAwIDUuNDMtNC41NmwxLjI2LTcuMTdhMzcuMiAzNy4yIDAgMCAwIDguOTMtMy43MWw2IDQuMThhNS41MSA1LjUxIDAgMCAwIDcuMDYtMC41OGw0LjMtNC4zYTUuNTEgNS41MSAwIDAgMCAwLjYxLTcuMDZsLTQuMTgtNmEzNy4xIDM3LjEgMCAwIDAgMy43LTguOTNsNy4xNy0xLjI2YTUuNTEgNS41MSAwIDAgMCA0LjU3LTUuNDV2LTZhNS41MSA1LjUxIDAgMCAwLTQuNTYtNS40M2wtNy4xNy0xLjI2YTM3LjEzIDM3LjEzIDAgMCAwLTMuNy04LjkzbDQuMTgtNmE1LjUxIDUuNTEgMCAwIDAtMC42LTcuMDZsLTQuMzItNC4zMmE1LjUxIDUuNTEgMCAwIDAtNy4wNi0wLjYybC02IDQuMThhMzcuMTIgMzcuMTIgMCAwIDAtOC45My0zLjcxbC0xLjIyLTcuMTRhNS41MSA1LjUxIDAgMCAwLTUuNDctNC41NWgtNmE1LjUxIDUuNTEgMCAwIDAtNS40MyA0LjU2bC0xLjI2IDcuMTdhMzcuMDkgMzcuMDkgMCAwIDAtOC45MyAzLjcxbC02LTQuMThhNS41MSA1LjUxIDAgMCAwLTcuMDYgMC42MmwtNC4zMiA0LjI4YTUuNTEgNS41MSAwIDAgMC0wLjYxIDcuMDZsNC4xOCA2YTM3LjE3IDM3LjE3IDAgMCAwLTMuNyA4LjkzbC03LjE2IDEuMjJhNS41MSA1LjUxIDAgMCAwLTQuNTYgNS40N3Y2YTUuNTEgNS41MSAwIDAgMCA0LjU2IDUuNDd6bTI0LjY1LTI3LjFhMjYuMzUgMjYuMzUgMCAxIDEgMCAzNy4yOCAyNi4xOSAyNi4xOSAwIDAgMSAwLTM3LjI4em0xNy4wNSAxMS4yNS01LjU0LTEwLjA1YTE4Ljg0IDE4Ljg0IDAgMCAxIDI0LjU2IDI0LjU3bC0xMC01LjU0YTcuNTMgNy41MyAwIDAgMC05LTl6bTEuNTkgNWEyLjQxIDIuNDEgMCAwIDEgMS43MSA0LjEyIDIuNDcgMi40NyAwIDAgMS0zLjQyIDAgMi40MiAyLjQyIDAgMCAxIDEuNzEtNC4xNXptLTEzLjMyIDE1LjdhMTguODYgMTguODYgMCAwIDEtNC4xMS0yMC40NWwxMCA1LjU0YTcuNTUgNy41NSAwIDAgMCA5IDlsNS41NCAxMGExOC44NyAxOC44NyAwIDAgMS0yMC40My00LjA5eiIvPgo8L2c+Cjwvc3ZnPgo=)" alt="Custom Badge">
    </p>
</div>

## Description

This software library (middleware) implements the standard cryptographic interfaces **PKCS#11** and **CSP**, allowing the use of the authentication certificate and its associated private key stored on the CIE chip.

## Components

The [library](libcie-pkcs11) is developed in C++ and supports GNU/Linux OS.

The [graphical interface](cie-java) is developed in Java.

## Installation

[Here](https://github.com/M0Rf30/cie-middleware-linux/releases) you can find the binaries generated from this repo for the following GNU/Linux distributions:

- Arch Linux [here](https://aur.archlinux.org/packages/cie-middleware-git)
- Rocky 9
- Ubuntu 22.04

Furthermore, you can install the Flatpak package using the provided manifest file.

## Building the Flatpak from the Manifest

This guide describes the steps required to build a Flatpak package using the provided manifest file.

### Prerequisites

Before proceeding, ensure you have the following installed on your system:

- [Flatpak](https://flatpak.org/setup/)
- [flatpak-builder](https://docs.flatpak.org/en/latest/flatpak-builder.html)

### Build Instructions

```sh
cie-java/gradlew -b cie-java/build.gradle standalone
cp cie-java/build/libs/CIEID-standalone.jar packages/flatpak

flatpak remote-add --if-not-exists --user flathub https://dl.flathub.org/repo/flathub.flatpakrepo
flatpak-builder --force-clean --user --install-deps-from=flathub --repo=repo --install build-dir packages/flatpak/app.m0rf30.cieid.yml
```

You can run the Flatpak package using the following command:

```sh
flatpak run --user app.m0rf30.cieid
```

## Building from sources

### Requirements

The following libraries are required:

- crypto++
- curl
- fontconfig
- freetype2
- java (>= 17)
- jdk (>= 17) (make)
- libpng
- libxml2
- meson (make)
- ninja (make)
- openssl
- pcsclite
- pkgconf (make)
- podofo (<= 0.10.4)

### Build Instructions

From the terminal, navigate to the root of this repo and type:

```sh
cie-java/gradlew -b cie-java/build.gradle standalone

meson setup builddir libs
meson configure -Dprefix=/usr builddir
meson compile -C builddir
```

At the end of the build, the following files will be present:

- builddir/libcie-pkcs11.so
- cie-java/build/libs/CIEID-standalone.jar

## Usage in the Browser

To use the library with the following browsers:

- Mozilla Firefox-based
- Google Chromium-based

follow the related instructions below.

**NOTE**: Only the last 4 digits of the PIN are required
in the browser.

### Mozilla Firefox-based Browsers

Follow the instructions provided in the Cie ID UI by
clicking on the `Tutorial` button.

### Google Chromium-based Browsers

Open a terminal and type:

```sh
modutil -dbdir sql:$HOME/.pki/nssdb -add CIE -libfile /usr/lib/libcie-pkcs11.so

# Verify the added library
modutil -dbdir sql:$HOME/.pki/nssdb -list
```
