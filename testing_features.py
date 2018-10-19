import os,json,psutil

with open("./config/config.json", "r+") as f:
    config = eval(f.read())

#i wanna print out dmenu if it is running
while(True):

    for program in config["programs"]:
        for process in psutil.process_iter():
            if process.name() == list(program.keys())[0]:
                print(process.name())
