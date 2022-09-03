#!/bin/bash

exec java \
  -Xms1G \
  -Xmx1G \
  -Djna.library.path="/usr/lib" \
  -Dawt.useSystemAAFontSettings=on \
  -classpath /usr/share/cieid/cieid.jar \
  it.ipzs.cieid.MainApplication
