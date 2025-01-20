#!/bin/sh

FOLDER='ttyxel_art'
VERSION='1.0.1'

dpkg-deb --build ./${FOLDER}
mv ./ttyxel_art.deb ./${FOLDER}_${VERSION}_amd64.deb
sudo dpkg -i ${FOLDER}_${VERSION}_amd64.deb
