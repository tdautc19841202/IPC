#!/bin/bash
rm -Rf _install/pack
mkdir -p _install/pack
mkdir -p _install/pack/busybox-1.24.1-IPCM
mkdir -p _install/pack/busybox-1.24.1-IPCM/bin
cp -R _install/bin/* _install/pack/busybox-1.24.1-IPCM/bin
cp -R _install/sbin/* _install/pack/busybox-1.24.1-IPCM/bin
cp -R _install/usr/sbin/* _install/pack/busybox-1.24.1-IPCM/bin
cp -R _install/usr/bin/* _install/pack/busybox-1.24.1-IPCM/bin
cd _install/pack/busybox-1.24.1-IPCM/bin && find ./ | xargs -i /bin/ln -fs busybox {} ;
cd ../..
tar -Jcvf ../busybox-1.24.1-IPCM.tar.xz busybox-1.24.1-IPCM