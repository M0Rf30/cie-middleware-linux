#!/bin/sh

# Use CIEID_PATH if set, otherwise default to /usr
: "${PREFIX:=/usr}"

exec java \
  -Xms1G \
  -Xmx1G \
  -Djna.library.path="${PREFIX}/lib" \
  -Dawt.useSystemAAFontSettings=on \
  -Dsun.java2d.uiScale.enabled=true \
  -Dsun.java2d.uiScale=2.0 \
  -classpath "${PREFIX}/share/cieid/cieid.jar" \
  app.m0rf30.cieid.MainApplication "$@"
