#!/usr/bin/bash

pacaur -S xkblayout-state-git --noconfirm;
pip3 install --user psutil;
echo "exec_always --no-startup-id 'cd ~/.config/i3-autokeymap && python3 keymap.py'" >> ~/.config/i3/config
