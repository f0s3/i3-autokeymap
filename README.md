# i3-autokeymap
Don't think about switching between layouts when switching to another workspace. Let this app do it for you.
## Prerequisites
Make sure that `python3 -V` gives you `Python 3.7.0`
## Dependencies used
This project by now uses these dependencies:
* [xkblayout-state](https://github.com/nonpop/xkblayout-state) (for checking current layout and switching to the next one)
## Installation
It usually depends on what linux flavor you are using.
### Debian, Ubuntu and other .deb distros
1. https://github.com/nonpop/xkblayout-state#compilation-and-installation (as official docs say...)

2. `cd` to a desired directory(usually you want this to be in `~/.config/`) and `git clone https://github.com/f0s3/i3-autokeymap`
### Arch, Manjaro, Antergos and other AUR users:
1. `yaourt -S xkblayout-state-git`

2. [Same as in Debian](https://github.com/f0s3/i3-autokeymap#)
## Running and adding to i3wm config file
To run script use `python3 keymap.py`

Example of config line: `exec_always --no-startup-id "cd ~/.config/i3-autokeymap && python3 i3-autokeymap"`
