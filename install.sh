#!/bin/sh

# Install script for TTYxelArt

INSTALL_DIR='/usr/local/opt'
BIN_DIR='/usr/local/bin'

# Create directories in case they do not exist
mkdir -p $INSTALL_DIR/ttyxelart/sprites || return 1
mkdir -p $BIN_DIR || return 1

# Copying files to corresponding locations
cp sprites.json $INSTALL_DIR/ttyxelart
cp ttyxel_art $INSTALL_DIR/ttyxelart/txa
cp ttyxel_art.py $INSTALL_DIR/ttyxelart

# Create symlink
rm -rf "$BIN_DIR/ttyxelart" || return 1
ln -s $INSTALL_DIR/ttyxelart/ttyxel_art.py $BIN_DIR/txa
chmod -R a+rwx $INSTALL_DIR/ttyxelart
