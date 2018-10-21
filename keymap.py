import os, time, json
import subprocess as sp

previousWorkspace = 1
currentWorkspace = 1
layoutOrder = {}

def shell(cmd):
    return sp.run(cmd, stdout=sp.PIPE, shell=True).stdout.decode("utf8")

keyboardInfo = shell("setxkbmap -query | grep layout | cut -c 13-").strip()
for index, e in enumerate(keyboardInfo.split(",")):
    layoutOrder[e] = index

with open("./config/config.json", "r+") as f:
    config = eval(f.read())

while True:
    currentWorkspace = [i["num"] for i in json.loads(shell("i3-msg -t get_workspaces")) if i["focused"]][0]
    if currentWorkspace == previousWorkspace:
        currentLayout = shell("xkblayout-state print %s")
        if currentLayout != config["workspaces"][currentWorkspace]: config["workspaces"][currentWorkspace][currentWorkspace] = currentLayout
    else:
        if shell("xkblayout-state print %s") != config["workspaces"][currentWorkspace][currentWorkspace]:
            os.system("xkblayout-state set {}".format(layoutOrder[config["workspaces"][currentWorkspace][currentWorkspace]]))
        previousWorkspace = currentWorkspace
    time.sleep(100 / 1000)
