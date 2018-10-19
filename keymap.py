import os,time,json,psutil
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

while(True):
    currentWorkspace = [i["num"] for i in json.loads(shell("i3-msg -t get_workspaces")) if i["focused"]][0]
    for program in config["programs"]:
        for process in psutil.process_iter():
            if process.name() == list(program.keys())[0]:
                os.system("xkblayout-state set {}".format(layoutOrder[list(filter(lambda x: process.name() in x, config["programs"]))[0][process.name()]]))
    if currentWorkspace == previousWorkspace:
        currentLayout = shell("xkblayout-state print %s")
        if currentLayout != config["workspaces"][currentWorkspace]: config["workspaces"][currentWorkspace] = currentLayout
    else:
        if shell("xkblayout-state print %s") != config["workspaces"][currentWorkspace]:
            os.system("xkblayout-state set {}".format(layoutOrder[config["workspaces"][currentWorkspace][currentWorkspace]]))
        previousWorkspace = currentWorkspace
    time.sleep(100/1000)
