#!/usr/bin/env bash

meson setup --prefix /usr --buildtype release build .
cd build || exit 1
meson compile
meson dist --include-subprojects
meson install --destdir AppDir --no-rebuild

if [[ ! -e "linuxdeploy-x86_64.AppImage" ]]; then
  wget https://github.com/linuxdeploy/linuxdeploy/releases/download/continuous/linuxdeploy-x86_64.AppImage
  chmod +x linuxdeploy-x86_64.AppImage
fi
NO_STRIP=1 ./linuxdeploy-x86_64.AppImage --appdir AppDir --output appimage
mv EZP2023+-x86_64.AppImage meson-dist/