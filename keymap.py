import os,time,json
import subprocess as sp


previousWorkspace = 1
currentWorkspace = 1
layoutOrder = {}

def shell(cmd):
    return sp.check_output(cmd.split(), universal_newlines=True) 

keyboardInfo = shell("./keyboardInfo.sh").strip()
for index, e in enumerate(keyboardInfo.split(",")):
    layoutOrder[e] = index

with open("./config/config.json", "r+") as f:
    config = eval(f.read())

while(True):
    currentWorkspace = [i["num"] for i in json.loads(shell("i3-msg -t get_workspaces")) if i["focused"]][0]
    if currentWorkspace == previousWorkspace:
        currentLayout = shell("./xkblayout-state print %s")
        if currentLayout != config[currentWorkspace]: config[currentWorkspace] = currentLayout
    else:
        if shell("./xkblayout-state print %s") != config[currentWorkspace]:
            os.system("./xkblayout-state set {}".format(layoutOrder[config[currentWorkspace]]))
        previousWorkspace = currentWorkspace
    time.sleep(100/1000)
