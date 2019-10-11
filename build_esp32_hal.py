# build_esp32_hal.py
# pre-build script, setting up build environment and fetch hal file for user's board
#
# Original concept by ESP32-Paxcounter.

import sys
import os
import os.path
import requests
from os.path import basename
from platformio import util
from SCons.Script import DefaultEnvironment

try:
    import configparser
except ImportError:
    import ConfigParser as configparser

# get platformio environment variables
env = DefaultEnvironment()
config = configparser.ConfigParser()
config.read("platformio.ini")

# get platformio source path
srcdir = env.get("PROJECTSRC_DIR")

# get hal path
haldir = os.path.join (srcdir, "src/hal")

# check if hal file is present in source directory
halconfig = config.get("esp32_board", "halfile")
halconfigfile = os.path.join (haldir, halconfig)
if os.path.isfile(halconfigfile) and os.access(halconfigfile, os.R_OK):
    print("Parsing hardware configuration from " + halconfigfile)
else:
    sys.exit("Missing file " + halconfigfile + ", please create it! Aborting.")

# check if lmic config file is present in source directory
lmicconfig = config.get("lmic_lora", "lmicconfigfile")
lmicconfigfile = os.path.join (srcdir, lmicconfig)
if os.path.isfile(lmicconfigfile) and os.access(lmicconfigfile, os.R_OK):
    print("Parsing LMIC configuration from " + lmicconfigfile)
else:
    sys.exit("Missing file " + lmicconfigfile + ", please create it! Aborting.")


# parse hal file
mykeys = {}
with open(halconfigfile) as myfile:
    for line in myfile:
        line2 = line.strip("// ")
        key, value = line2.partition(" ")[::2]
        mykeys[key.strip()] = str(value).strip()

if "upload_speed" in mykeys:
    myuploadspeed = mykeys["upload_speed"]
    env.Replace(UPLOAD_SPEED=myuploadspeed)
  
myboard = mykeys["board"]
env.Replace(BOARD=myboard)

# re-set partition table
mypartitiontable = config.get("core_esp32", "board_build.partitions")
board = env.BoardConfig(myboard)
board.manifest['build']['partitions'] = mypartitiontable

# display target
print('\033[94m' + "TARGET BOARD: " + myboard + " @ " + myuploadspeed + "bps" + '\033[0m')
print('\033[94m' + "Partition table: " + mypartitiontable + '\033[0m')


# get runtime credentials and put them to compiler directive
env['BUILD_FLAGS'].extend([
    u'-DARDUINO_LMIC_PROJECT_CONFIG_H=' + lmicconfig,
    u'-I \"' + srcdir + '\"'
    ])
