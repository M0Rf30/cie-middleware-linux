# Software per la Carta d'Identità Elettronica

![Build Status](https://github.com/M0Rf30/cie-middleware-linux/actions/workflows/main.yml/badge.svg)
![Packages](https://img.shields.io/badge/packaged%20with-yap-orange?url=https://left&link=github.com/packagefoundation/yap&logo=data:image/svg+xml;base64,PD94bWwgdmVyc2lvbj0iMS4wIiBlbmNvZGluZz0iVVRGLTgiPz4KPHN2ZyBjbGFzcz0iY3NzLTFqOG82OGYiIHdpZHRoPSIyOTIuNTkiIGhlaWdodD0iMjUzLjU2IiB2ZXJzaW9uPSIxLjEiIHZpZXdCb3g9IjAgMCAyNjAgMjI1LjMyIiB4bWxucz0iaHR0cDovL3d3dy53My5vcmcvMjAwMC9zdmciPgo8ZyB0cmFuc2Zvcm09Im1hdHJpeCgyLjcxNTUgMCAwIDIuMzU0OSAtNS43NiAtNS4wODU4KSIgZmlsbD0iIzRlY2NhMyI+CjxwYXRoIGQ9Im02LjcxIDU4LjQ3IDcuMTcgMS4yNmEzNy4xNSAzNy4xNSAwIDAgMCAzLjcgOC45M2wtNC4xOCA2YTUuNTEgNS41MSAwIDAgMCAwLjYgNy4wMmw0LjMgNC4zYTUuNTEgNS41MSAwIDAgMCA3LjA2IDAuNjFsNi00LjE4YTM3LjE0IDM3LjE0IDAgMCAwIDguOTMgMy43bDEuMjYgNy4xN2E1LjUxIDUuNTEgMCAwIDAgNS40NSA0LjU2aDZhNS41MSA1LjUxIDAgMCAwIDUuNDMtNC41NmwxLjI2LTcuMTdhMzcuMiAzNy4yIDAgMCAwIDguOTMtMy43MWw2IDQuMThhNS41MSA1LjUxIDAgMCAwIDcuMDYtMC41OGw0LjMtNC4zYTUuNTEgNS41MSAwIDAgMCAwLjYxLTcuMDZsLTQuMTgtNmEzNy4xIDM3LjEgMCAwIDAgMy43LTguOTNsNy4xNy0xLjI2YTUuNTEgNS41MSAwIDAgMCA0LjU3LTUuNDV2LTZhNS41MSA1LjUxIDAgMCAwLTQuNTYtNS40M2wtNy4xNy0xLjI2YTM3LjEzIDM3LjEzIDAgMCAwLTMuNy04LjkzbDQuMTgtNmE1LjUxIDUuNTEgMCAwIDAtMC42LTcuMDZsLTQuMzItNC4zMmE1LjUxIDUuNTEgMCAwIDAtNy4wNi0wLjYybC02IDQuMThhMzcuMTIgMzcuMTIgMCAwIDAtOC45My0zLjcxbC0xLjIyLTcuMTRhNS41MSA1LjUxIDAgMCAwLTUuNDctNC41NWgtNmE1LjUxIDUuNTEgMCAwIDAtNS40MyA0LjU2bC0xLjI2IDcuMTdhMzcuMDkgMzcuMDkgMCAwIDAtOC45MyAzLjcxbC02LTQuMThhNS41MSA1LjUxIDAgMCAwLTcuMDYgMC42MmwtNC4zMiA0LjI4YTUuNTEgNS41MSAwIDAgMC0wLjYxIDcuMDZsNC4xOCA2YTM3LjE3IDM3LjE3IDAgMCAwLTMuNyA4LjkzbC03LjE2IDEuMjJhNS41MSA1LjUxIDAgMCAwLTQuNTYgNS40N3Y2YTUuNTEgNS41MSAwIDAgMCA0LjU2IDUuNDd6bTI0LjY1LTI3LjFhMjYuMzUgMjYuMzUgMCAxIDEgMCAzNy4yOCAyNi4xOSAyNi4xOSAwIDAgMSAwLTM3LjI4em0xNy4wNSAxMS4yNS01LjU0LTEwLjA1YTE4Ljg0IDE4Ljg0IDAgMCAxIDI0LjU2IDI0LjU3bC0xMC01LjU0YTcuNTMgNy41MyAwIDAgMC05LTl6bTEuNTkgNWEyLjQxIDIuNDEgMCAwIDEgMS43MSA0LjEyIDIuNDcgMi40NyAwIDAgMS0zLjQyIDAgMi40MiAyLjQyIDAgMCAxIDEuNzEtNC4xNXptLTEzLjMyIDE1LjdhMTguODYgMTguODYgMCAwIDEtNC4xMS0yMC40NWwxMCA1LjU0YTcuNTUgNy41NSAwIDAgMCA5IDlsNS41NCAxMGExOC44NyAxOC44NyAwIDAgMS0yMC40My00LjA5eiIvPgo8L2c+Cjwvc3ZnPgo=)

## Descrizione

Questa libreria software (middleware) implementa le interfacce crittografiche
standard **PKCS#11** e **CSP**, consentendo l'utilizzo del certificato di
autenticazione e della relativa chiave privata memorizzati sul chip della CIE.

## Componenti

La [libreria](libcie-pkcs11) è sviluppata in C++ e supporta OS GNU/Linux.

La parte di [interfaccia grafica](cie-java) è sviluppata in Java.

## Installazione

[Qui](https://github.com/M0Rf30/cie-middleware-linux/releases) puoi trovare i
binari generati da questo repo, per le seguenti distro GNU/Linux:

* Arch Linux [qui](https://aur.archlinux.org/packages/cie-middleware-git)
* Rocky 8
* Ubuntu 20.04

## Compilazione

### Requisiti

Sono necessarie le seguenti librerie:

* crypto++
* curl
* fontconfig
* freetype2
* gradle (make)
* java (>= 11)
* jdk (>= 11) (make)
* libpng
* libxml2
* meson (make)
* ninja (make)
* openssl
* pcsclite
* pkgconf (make)

### Istruzioni

Da terminale, spostarsi nella root del presente repo e digitare:

```sh
gradle -b cie-java/build.gradle standalone

curl -sL "https://github.com/M0Rf30/cie-middleware-linux/releases/download/podofo-1.4.2/libpodofo-1.4.2.tar.gz" -o libpodofo.tar.gz

tar xf libpodofo.tar.gz --directory=libs/lib

meson builddir libs
meson configure -Dprefix=/usr builddir
meson compile -C builddir
```

alla fine della build saranno presenti i file:

* builddir/libcie-pkcs11.so
* builddir/libcie-sign-sdk.a
* cie-java/bin/libs/CIEID-standalone.jar (gli eseguibili di test e sblocco/cambio PIN)
