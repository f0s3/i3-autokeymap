# i3-autokeymap
Don't think about switching between layouts when switching to another workspace. Let this app do it for you.
## Prerequisites
Make sure that 

    node -v && npm -v
gives you something like

    v10.3.0
    6.3.0
## Dependencies used
This project by now uses these dependencies:
* [xkblayout-state](https://github.com/nonpop/xkblayout-state) (for checking current layout and switching to the next one)
* [node.js](https://nodejs.org/) (for running this project) and npm (for downloading module globally on your pc)
* [child_process](https://www.npmjs.com/package/child_process) npm module (for executing shell commands and getting the results of them)
## Installation
Use this oneliner in the folder you want to install i3-autokeymap.

    git clone https://github.com/f0s3/i3-autokeymap && cd i3-autokeymap/dependencies && make && cp xkblayout-state .. && cd .. && sudo npm i child_process -g && sudo rm -r dependencies test .git && sudo rm README.md package.json .gitignore
## Running and adding to i3wm config file
To run script use

    node i3-autokeymap.js
Example of using this in config:

    exec_always --no-startup-id "cd ~/.config/i3-autokeymap && node i3-autokeymap"
