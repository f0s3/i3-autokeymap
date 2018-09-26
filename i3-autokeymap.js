const config = require('./config/config')
const exec = require('child_process').exec

let previousWorkspace = 1;
let currentWorkspace = 1;
let layoutOrder = {}

exec('setxkbmap -query | grep layout', (stdIn, keyboardInfo, stdErr) => {
	let layoutInfo = keyboardInfo.replace('layout:', '').trim().split(/,/).forEach((k, i) => {layoutOrder[k] = i})
	setInterval(() => {
		exec('i3-msg -t get_workspaces', (error, fullWorkspaceInfo, stdErr) => {
			let workspaceActivityArray = JSON.parse(fullWorkspaceInfo).map(k => { return { num: k.num, focused: k.focused } })
			let currentWorkspace = workspaceActivityArray.find(i => i.focused).num
			if (currentWorkspace == previousWorkspace) {
				exec('./xkblayout-state print %s', (stdIn, currentLayout, stdErr) => {
					if (currentLayout != config[currentWorkspace]) config[currentWorkspace] = currentLayout
					
				})
			} else {
				exec('./xkblayout-state print %s', (stdIn, currentLayout, stdErr) => {
					if (currentLayout != config[currentWorkspace]) exec(`./xkblayout-state set ${layoutOrder[config[currentWorkspace]]}`)
				})
				previousWorkspace = currentWorkspace
			}
		})
	}, 100)
})
