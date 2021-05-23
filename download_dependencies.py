import os
import shutil
import pathlib

SCRIPT_PATH = os.path.realpath(os.path.dirname(__file__))
os.chdir(SCRIPT_PATH)

def shell_cmd(command):
    print(command)
    return_code = os.system(command)
    if return_code != 0:
        raise Exception("Command '{}' returned non zero code: {}".format(command, return_code))

try:
    if os.path.exists('./lib/cpput'):
        shutil.rmtree('./lib/cpput')
    os.makedirs("./lib/", exist_ok=True)
    shell_cmd(" && ".join([
        "cd ./lib/",
        "git clone https://github.com/WojciechSobczak/cpput.git",
        "cd ./cpput/",
        "git reset --hard ae68bed5041fa7b9f85fb2762bc430f0ac37a77a"
    ]))
except Exception as e:
    print(e)











