import os, time, json, psutil
import subprocess as sp

previousWorkspace = 1
currentWorkspace = 1
appLaunched = False
layoutOrder = {}

def shell(cmd):
    return sp.run(cmd, stdout=sp.PIPE, shell=True).stdout.decode("utf8")

keyboardInfo = shell("setxkbmap -query | grep layout | cut -c 13-").strip()
for index, e in enumerate(keyboardInfo.split(",")):
    layoutOrder[e] = index

with open("./config/config.json", "r+") as f:
    config = eval(f.read())

def programIsRunning(programName):
    for process in psutil.process_iter():
        if process.name() == programName:
            return True
    return False

while True:
    currentWorkspace = [i["num"] for i in json.loads(shell("i3-msg -t get_workspaces")) if i["focused"]][0]
    for program in config["programs"]:
        if programIsRunning(list(program.keys())[0]):
            os.system("xkblayout-state set {}".format(layoutOrder[list(filter(lambda x: list(program.keys())[0] in x, config["programs"]))[0][list(program.keys())[0]]]))
            appLaunched = True
        elif appLaunched:
            appLaunched = False
            currentWorkspace = currentWorkspace + 1
    if currentWorkspace == previousWorkspace:
        currentLayout = shell("xkblayout-state print %s")
        if currentLayout != config["workspaces"][currentWorkspace][currentWorkspace]: config["workspaces"][currentWorkspace][currentWorkspace] = currentLayout
    else:
        if shell("xkblayout-state print %s") != config["workspaces"][currentWorkspace][currentWorkspace]:
            os.system("xkblayout-state set {}".format(layoutOrder[config["workspaces"][currentWorkspace][currentWorkspace]]))
        previousWorkspace = currentWorkspace
    time.sleep(100 / 1000)
