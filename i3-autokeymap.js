const config = require('./config/config')
const exec = require('child_process').exec

let focusedWorkspace = (array) => {return array.find(i => i.focused)}

setInterval(() => {
	exec('i3-msg -t get_workspaces', (stdIn, workspacesArrayUnhandled, stdErr) => {
		let workspaceArray = JSON.parse(workspacesArrayUnhandled).map(k => {return {num: k.num, focused: k.focused}})
		exec('xkblayout-state print %s', (stdIn, currentLayout, stdErr) => {
			if (config[focusedWorkspace(workspaceArray).num] !== currentLayout) exec('xkblayout-state set +1')
		})
	})
}, 150)
module.exports.focusedWorkspace = focusedWorkspace
