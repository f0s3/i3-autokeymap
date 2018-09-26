const config = require('./config/config')
const exec = require('child_process').exec

let focusedWorkspace = (array) => { return array.find(i => i.focused) }

let prevWorkspace = 1;
let currentWorkspace = 1;

let layoutOrder = {}


exec('setxkbmap -query | grep layout', (stdIn, keyboardInfo, stdErr) => {
	let layoutInfo = keyboardInfo.replace('layout:', '').trim()
	layoutInfo.split(/,/).forEach((k, i) => {
		layoutOrder[k] = i;
	})

	setInterval(() => {
		exec('i3-msg -t get_workspaces', (error, curWork, stdErr) => {
			let workspaceActivityArray = JSON.parse(curWork).map(k => { return { num: k.num, focused: k.focused } })
			let currentWorkspace = focusedWorkspace(workspaceActivityArray).num
			if (currentWorkspace == prevWorkspace) {
				exec('./xkblayout-state print %s', (stdIn, currentLayout, stdErr) => {
					if (currentLayout != config[currentWorkspace]) {
						config[currentWorkspace] = currentLayout
					}
				})
			} else {
				exec('./xkblayout-state print %s', (stdIn, stdOut, stdErr) => {
					if (stdOut != config[currentWorkspace]) {
						exec(`./xkblayout-state set ${layoutOrder[config[currentWorkspace]]}`)
					}
				})
				prevWorkspace = currentWorkspace
			}

		})
	}, 150)
})

module.exports.focusedWorkspace = focusedWorkspace
