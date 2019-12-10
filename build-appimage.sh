#!/usr/bin/env bash

mkdir -p build
cd build
cmake -DCMAKE_BUILD_TYPE:STRING=Release ..
# call make twice to fix qt auto-generated #include line
make || true
make fix-ui
make
export PATH=/opt/qt512/bin:$PATH
cp ../open-esc-qt.desktop .
wget -c -nv "https://github.com/probonopd/linuxdeployqt/releases/download/continuous/linuxdeployqt-continuous-x86_64.AppImage"
chmod a+x linuxdeployqt-continuous-x86_64.AppImage
./linuxdeployqt-continuous-x86_64.AppImage open-esc-qt -appimage -unsupported-allow-new-glibc -verbose=0
mv open-esc-qt*.AppImage open-esc-qt.AppImage
cd ..
