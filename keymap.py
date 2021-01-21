import os, time, json
from subprocess import DEVNULL, STDOUT, PIPE, run, check_call

previousWorkspace = 1
currentWorkspace = 1
appLaunched = False
layoutOrder = {}

class Application:
    def __init__(self, command, layout, status=False):
        self.command = command
        self.layout = layout
        self.status = status

applications_layouts = []

# todo: unhardcode me and fill in from config!
applications_layouts.append(Application('dmenu_run', 'us'))
applications_layouts.append(Application('i3', 'us'))
applications_layouts.append(Application('keymap.py', 'us'))

def shell(cmd):
    return run(cmd, stdout=PIPE, shell=True).stdout.decode("utf8")

keyboardInfo = shell("setxkbmap -query | grep layout | cut -c 13-").strip()
for index, e in enumerate(keyboardInfo.split(",")):
    layoutOrder[e] = index

with open("./config/config.json", "r+") as f:
    config = eval(f.read())


def get_running_programs():
    user = "f0s3" # todo: unhardcode me

    first_part = "ps -u {} -o cmd ".format(user)
    last_part = " | grep -v grep"

    for app in applications_layouts:
        a = shell(first_part + " | grep '{}'".format(app.command) + last_part)
        if a.isdigit():
            app.status = True

def is_application_running(app):
    return app.status == True

get_running_programs()

while True:
    if get_running_programs() != "":
        currentLayout = shell("xkblayout-state print %s") # maybe we can move it to a global variable
        for app in list(filter(is_application_running, applications_layouts)):
            if currentLayout != app.layout:
                os.system("xkblayout-state set {}".format(app.layout))

    currentWorkspace = [i["num"] for i in json.loads(shell("i3-msg -t get_workspaces")) if i["focused"]][0]
    if currentWorkspace == previousWorkspace:
        currentLayout = shell("xkblayout-state print %s")
        if currentLayout != config["workspaces"][currentWorkspace][currentWorkspace]: config["workspaces"][currentWorkspace][currentWorkspace] = currentLayout
    else:
        if shell("xkblayout-state print %s") != config["workspaces"][currentWorkspace][currentWorkspace]:
            os.system("xkblayout-state set {}".format(layoutOrder[config["workspaces"][currentWorkspace][currentWorkspace]]))
        previousWorkspace = currentWorkspace
    time.sleep(100 / 1000)

