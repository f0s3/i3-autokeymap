const config = require('./config/config')
const exec = require('child_process').exec

setInterval(() => {
	exec('i3-msg -t get_workspaces', (error, stdOut, stdErr) => {
		let workspaceActivityArray = JSON.parse(stdOut).map(k => {return {num: k.num, focused: k.focused}})
		function focusedWorkspace() {
			workspaceActivityArray.forEach(i => {if (i.focused === true) workspaceObj = i})
			return workspaceObj
		}
		exec('xkblayout-state print %s', (error, stdOut, stdErr) => {if (config[focusedWorkspace().num] !== stdOut) exec('xkblayout-state set +1')})
	})
}, 400)
