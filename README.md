# Middleware CSP-PKCS11 per la CIE 3.0

![Build Status](https://github.com/M0Rf30/cie-middleware-linux/actions/workflows/main.yml/badge.svg)
![Packages](https://img.shields.io/badge/packaged%20with-yap-orange?url=https://left&link=github.com/packagefoundation/yap&logo=data:image/svg+xml;base64,PD94bWwgdmVyc2lvbj0iMS4wIiBlbmNvZGluZz0iVVRGLTgiPz4KPHN2ZyBjbGFzcz0iY3NzLTFqOG82OGYiIHdpZHRoPSIyOTIuNTkiIGhlaWdodD0iMjUzLjU2IiB2ZXJzaW9uPSIxLjEiIHZpZXdCb3g9IjAgMCAyNjAgMjI1LjMyIiB4bWxucz0iaHR0cDovL3d3dy53My5vcmcvMjAwMC9zdmciPgo8ZyB0cmFuc2Zvcm09Im1hdHJpeCgyLjcxNTUgMCAwIDIuMzU0OSAtNS43NiAtNS4wODU4KSIgZmlsbD0iIzRlY2NhMyI+CjxwYXRoIGQ9Im02LjcxIDU4LjQ3IDcuMTcgMS4yNmEzNy4xNSAzNy4xNSAwIDAgMCAzLjcgOC45M2wtNC4xOCA2YTUuNTEgNS41MSAwIDAgMCAwLjYgNy4wMmw0LjMgNC4zYTUuNTEgNS41MSAwIDAgMCA3LjA2IDAuNjFsNi00LjE4YTM3LjE0IDM3LjE0IDAgMCAwIDguOTMgMy43bDEuMjYgNy4xN2E1LjUxIDUuNTEgMCAwIDAgNS40NSA0LjU2aDZhNS41MSA1LjUxIDAgMCAwIDUuNDMtNC41NmwxLjI2LTcuMTdhMzcuMiAzNy4yIDAgMCAwIDguOTMtMy43MWw2IDQuMThhNS41MSA1LjUxIDAgMCAwIDcuMDYtMC41OGw0LjMtNC4zYTUuNTEgNS41MSAwIDAgMCAwLjYxLTcuMDZsLTQuMTgtNmEzNy4xIDM3LjEgMCAwIDAgMy43LTguOTNsNy4xNy0xLjI2YTUuNTEgNS41MSAwIDAgMCA0LjU3LTUuNDV2LTZhNS41MSA1LjUxIDAgMCAwLTQuNTYtNS40M2wtNy4xNy0xLjI2YTM3LjEzIDM3LjEzIDAgMCAwLTMuNy04LjkzbDQuMTgtNmE1LjUxIDUuNTEgMCAwIDAtMC42LTcuMDZsLTQuMzItNC4zMmE1LjUxIDUuNTEgMCAwIDAtNy4wNi0wLjYybC02IDQuMThhMzcuMTIgMzcuMTIgMCAwIDAtOC45My0zLjcxbC0xLjIyLTcuMTRhNS41MSA1LjUxIDAgMCAwLTUuNDctNC41NWgtNmE1LjUxIDUuNTEgMCAwIDAtNS40MyA0LjU2bC0xLjI2IDcuMTdhMzcuMDkgMzcuMDkgMCAwIDAtOC45MyAzLjcxbC02LTQuMThhNS41MSA1LjUxIDAgMCAwLTcuMDYgMC42MmwtNC4zMiA0LjI4YTUuNTEgNS41MSAwIDAgMC0wLjYxIDcuMDZsNC4xOCA2YTM3LjE3IDM3LjE3IDAgMCAwLTMuNyA4LjkzbC03LjE2IDEuMjJhNS41MSA1LjUxIDAgMCAwLTQuNTYgNS40N3Y2YTUuNTEgNS41MSAwIDAgMCA0LjU2IDUuNDd6bTI0LjY1LTI3LjFhMjYuMzUgMjYuMzUgMCAxIDEgMCAzNy4yOCAyNi4xOSAyNi4xOSAwIDAgMSAwLTM3LjI4em0xNy4wNSAxMS4yNS01LjU0LTEwLjA1YTE4Ljg0IDE4Ljg0IDAgMCAxIDI0LjU2IDI0LjU3bC0xMC01LjU0YTcuNTMgNy41MyAwIDAgMC05LTl6bTEuNTkgNWEyLjQxIDIuNDEgMCAwIDEgMS43MSA0LjEyIDIuNDcgMi40NyAwIDAgMS0zLjQyIDAgMi40MiAyLjQyIDAgMCAxIDEuNzEtNC4xNXptLTEzLjMyIDE1LjdhMTguODYgMTguODYgMCAwIDEtNC4xMS0yMC40NWwxMCA1LjU0YTcuNTUgNy41NSAwIDAgMCA5IDlsNS41NCAxMGExOC44NyAxOC44NyAwIDAgMS0yMC40My00LjA5eiIvPgo8L2c+Cjwvc3ZnPgo=)

Il middleware qui presente è in fase di sviluppo, ed è da considerarsi in
**versione beta**. È possibile effettuare tutti gli sviluppi e i test, ma è per
ora questa base di codice **non è consigliabile per l'uso in produzione**.

## Caso d'uso

Il middleware CIE è una libreria software che implementa le interfacce
crittografiche standard **PKCS#11** e **CSP**. Esso consente agli applicativi
integranti di utilizzare il certificato di autenticazione e la relativa chiave
privata memorizzati sul chip della CIE astraendo dalle modalità di comunicazione
di basso livello.

## Architettura

La [libreria](libcie-pkcs11) è sviluppata in C++ e supporta OS GNU/Linux.

La parte di [interfaccia grafica](cie-java) è sviluppata in Java.

## CSP

Il Minidriver CIE gestisce la carta in modalità **Read-Only**, pertanto i
comandi di creazione e cancellazione di oggetti non sono supportati. Si faccia
riferimento alla specifica Microsoft per i dettagli su quali operazioni possono
essere effettuate su una carta Read Only.

Il modulo CSP implementa anche uno store provider per i certificati, in modo
tale da non richiedere l’operazione di propagazione dei certificati nello store
di sistema.

## PKCS11

Allo stesso modo del CSP, anche il PKCS11 gestisce la carta in modalità
**read-only**. Pertanto le operazioni di creazione, modifica e distruzione di
qualsiasi oggetto restituiranno un errore.

## GNU/Linux

### Arch Linux

Da terminale esegui
`yay -S cie-middleware-git`

dove `yay` rappresenta il tuo AUR-helper.

### Binari

Puoi trovare alcuni binari generati da questo repo
[qui](https://github.com/M0Rf30/cie-middleware-linux/releases), per le seguenti
distro GNU/Linux:

* Ubuntu
* Fedora
* OpenSuse

## Build

### Prerequisiti

Fare riferimento al file [PKGBUILD](packages/cie-middleware/PKGBUILD) per
ottenere le dipendenze di build-time and run-time per la propria distribuzione.

È inoltre possibile trovare le istruzioni complete per costruire gli artefatti
necessari e specifici per la propria distro.

## Build generica

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
