<div align="center">
  <img src="data/app.m0rf30.cieid.svg" alt="App Logo" height="200" />
  <!-- Spacer -->
  <br /><br />

  <!-- Centered language switcher -->
  <p>
    <a href="./README.en.md">English</a> |
    <a href="./README.md">Italiano</a>
  </p>
  <h1 align="center">Software per la Carta d'Identità Elettronica</h1>
    <!-- Build status badges -->
    <p>
        <img src="https://github.com/M0Rf30/cie-middleware-linux/actions/workflows/main.yml/badge.svg" alt="Build Status">
        <img src="https://img.shields.io/badge/packaged%20with-yap-orange?url=https://left&link=github.com/packagefoundation/yap&logo=data:image/svg+xml;base64,PD94bWwgdmVyc2lvbj0iMS4wIiBlbmNvZGluZz0iVVRGLTgiPz4KPHN2ZyBjbGFzcz0iY3NzLTFqOG82OGYiIHdpZHRoPSIyOTIuNTkiIGhlaWdodD0iMjUzLjU2IiB2ZXJzaW9uPSIxLjEiIHZpZXdCb3g9IjAgMCAyNjAgMjI1LjMyIiB4bWxucz0iaHR0cDovL3d3dy53My5vcmcvMjAwMC9zdmciPgo8ZyB0cmFuc2Zvcm09Im1hdHJpeCgyLjcxNTUgMCAwIDIuMzU0OSAtNS43NiAtNS4wODU4KSIgZmlsbD0iIzRlY2NhMyI+CjxwYXRoIGQ9Im02LjcxIDU4LjQ3IDcuMTcgMS4yNmEzNy4xNSAzNy4xNSAwIDAgMCAzLjcgOC45M2wtNC4xOCA2YTUuNTEgNS41MSAwIDAgMCAwLjYgNy4wMmw0LjMgNC4zYTUuNTEgNS41MSAwIDAgMCA3LjA2IDAuNjFsNi00LjE4YTM3LjE0IDM3LjE0IDAgMCAwIDguOTMgMy43bDEuMjYgNy4xN2E1LjUxIDUuNTEgMCAwIDAgNS40NSA0LjU2aDZhNS41MSA1LjUxIDAgMCAwIDUuNDMtNC41NmwxLjI2LTcuMTdhMzcuMiAzNy4yIDAgMCAwIDguOTMtMy43MWw2IDQuMThhNS41MSA1LjUxIDAgMCAwIDcuMDYtMC41OGw0LjMtNC4zYTUuNTEgNS41MSAwIDAgMCAwLjYxLTcuMDZsLTQuMTgtNmEzNy4xIDM3LjEgMCAwIDAgMy43LTguOTNsNy4xNy0xLjI2YTUuNTEgNS41MSAwIDAgMCA0LjU3LTUuNDV2LTZhNS41MSA1LjUxIDAgMCAwLTQuNTYtNS40M2wtNy4xNy0xLjI2YTM3LjEzIDM3LjEzIDAgMCAwLTMuNy04LjkzbDQuMTgtNmE1LjUxIDUuNTEgMCAwIDAtMC42LTcuMDZsLTQuMzItNC4zMmE1LjUxIDUuNTEgMCAwIDAtNy4wNi0wLjYybC02IDQuMThhMzcuMTIgMzcuMTIgMCAwIDAtOC45My0zLjcxbC0xLjIyLTcuMTRhNS41MSA1LjUxIDAgMCAwLTUuNDctNC41NWgtNmE1LjUxIDUuNTEgMCAwIDAtNS40MyA0LjU2bC0xLjI2IDcuMTdhMzcuMDkgMzcuMDkgMCAwIDAtOC45MyAzLjcxbC02LTQuMThhNS41MSA1LjUxIDAgMCAwLTcuMDYgMC42MmwtNC4zMiA0LjI4YTUuNTEgNS41MSAwIDAgMC0wLjYxIDcuMDZsNC4xOCA2YTM3LjE3IDM3LjE3IDAgMCAwLTMuNyA4LjkzbC03LjE2IDEuMjJhNS41MSA1LjUxIDAgMCAwLTQuNTYgNS40N3Y2YTUuNTEgNS41MSAwIDAgMCA0LjU2IDUuNDd6bTI0LjY1LTI3LjFhMjYuMzUgMjYuMzUgMCAxIDEgMCAzNy4yOCAyNi4xOSAyNi4xOSAwIDAgMSAwLTM3LjI4em0xNy4wNSAxMS4yNS01LjU0LTEwLjA1YTE4Ljg0IDE4Ljg0IDAgMCAxIDI0LjU2IDI0LjU3bC0xMC01LjU0YTcuNTMgNy41MyAwIDAgMC05LTl6bTEuNTkgNWEyLjQxIDIuNDEgMCAwIDEgMS43MSA0LjEyIDIuNDcgMi40NyAwIDAgMS0zLjQyIDAgMi40MiAyLjQyIDAgMCAxIDEuNzEtNC4xNXptLTEzLjMyIDE1LjdhMTguODYgMTguODYgMCAwIDEtNC4xMS0yMC40NWwxMCA1LjU0YTcuNTUgNy41NSAwIDAgMCA5IDlsNS41NCAxMGExOC44NyAxOC44NyAwIDAgMS0yMC40My00LjA5eiIvPgo8L2c+Cjwvc3ZnPgo=)" alt="Custom Badge">
    </p>
</div>

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

- Arch Linux [qui](https://aur.archlinux.org/packages/cie-middleware-git)
- Rocky 9
- Ubuntu 22.04

Inoltre puoi installare il pacchetto Flatpak utilizzando il file manifest fornito.

## Generazione Flatpak dal Manifest

Questa guida descrive i passaggi necessari per creare un pacchetto Flatpak utilizzando il file manifest fornito.

### Prerequisiti

Prima di procedere, assicurati di avere installato i seguenti software sul tuo sistema:

- [Flatpak](https://flatpak.org/setup/)
- [flatpak-builder](https://docs.flatpak.org/en/latest/flatpak-builder.html)

### Istruzioni

```sh
cie-java/gradlew -b cie-java/build.gradle standalone
cp cie-java/build/libs/CIEID-standalone.jar packages/flatpak

flatpak remote-add --if-not-exists --user flathub https://dl.flathub.org/repo/flathub.flatpakrepo
flatpak-builder --force-clean --user --install-deps-from=flathub --repo=repo --install build-dir packages/flatpak/app.m0rf30.cieid.yml
```

Puoi eseguire il pacchetto Flatpak installato con il seguente comando:

```sh
flatpak run --user app.m0rf30.cieid
```

## Compilazione

### Requisiti

Sono necessarie le seguenti librerie:

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

### Istruzioni

Da terminale, spostarsi nella root del presente repo e digitare:

```sh
cie-java/gradlew -b cie-java/build.gradle standalone

meson setup builddir libs
meson configure -Dprefix=/usr builddir
meson compile -C builddir
```

alla fine della build saranno presenti i file:

- builddir/libcie-pkcs11.so
- cie-java/build/libs/CIEID-standalone.jar

## Uso nel browser

Per utilizzare la libreria con i seguenti browser:

- basati su Mozilla Firefox
- basati su Google Chromium

Segui le relative istruzioni sottostanti.

**NOTA BENE**: nel browser vengono sempre richieste
le ultime 4 cifre del pin.

### Browser basati su Mozilla Firefox

Segui le istruzioni riportate nella UI di Cie ID, cliccando
sul pulsante `Tutorial`.

### Browser basati su Google Chromium

Aprire un terminale e digitare:

```sh
modutil -dbdir sql:$HOME/.pki/nssdb -add CIE -libfile /usr/lib/libcie-pkcs11.so

# Verifica della libreria aggiunta
modutil -dbdir sql:$HOME/.pki/nssdb -list
```
