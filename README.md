# Middleware CSP-PKCS11 per la CIE 3.0

![Build Status](https://github.com/M0Rf30/cie-middleware-linux/actions/workflows/main.yml/badge.svg)

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

Fare riferimento al file [pacur/PKGBUILD](pacur/PKGBUILD) per ottenere le
dipendenze di build-time and run-time per la propria distribuzione.

È inoltre possibile trovare le istruzioni complete per costruire gli artefatti
necessari e specifici per la propria distro.

## Build generica

Da terminale, spostarsi nella root del presente repo e digitare:

```sh
gradle -b cie-java/build.gradle standalone
cd libcie-pkcs11/ 
meson builddir 
meson configure -Dprefix=/usr builddir
meson compile -C builddir
```

alla fine della build saranno presenti i file:

* libcie-pkcs11/builddir/libcie-pkcs11.so
* cie-java/bin/libs/CIEID-standalone.jar (gli eseguibili di test e sblocco/cambio PIN)
