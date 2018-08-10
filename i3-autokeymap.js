const config = require('./config/config')
const exec = require('child_process').exec

let focusedWorkspace = function(array) {
	array.forEach(i => {if (i.focused) workspaceObj = i})
	return workspaceObj;
}

setInterval(() => {
	exec('i3-msg -t get_workspaces', (error, stdOut, stdErr) => {
		let workspaceActivityArray = JSON.parse(stdOut).map(k => {return {num: k.num, focused: k.focused}})
		exec('xkblayout-state print %s', (error, stdOut, stdErr) => {
			if (config[focusedWorkspace(workspaceActivityArray).num] !== stdOut) exec('xkblayout-state set +1')
		})
	})
}, 400)
module.exports.focusedWorkspace = focusedWorkspace
