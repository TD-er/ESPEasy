Import("env")
import os

# access to global construction environment
#print env

# Dump construction environment (for debug purpose)
#print env.Dump()

# append extra flags to global build environment
# which later will be used to build:
# - project source code
# - frameworks
# - dependent libraries
env.Append(CPPDEFINES=[
  # ,"NO_HTTP_UPDATER"
  # ,("WEBSERVER_RULES_DEBUG", "0")
])
if os.path.isfile('src/Custom.h'):
  env.Append(CPPDEFINES=["USE_CUSTOM_H"])
else:
  env.Append(CPPDEFINES=[
    "USES_WIFI_MESH",
    "CONTROLLER_SET_NONE",
    "USES_C002",  # Domoticz MQTT
    "USES_C003",  # Nodo telnet
    "USES_C004",  # ThingSpeak
    "USES_C005",  # Home Assistant (openHAB) MQTT

    "NOTIFIER_SET_NONE",
    "PLUGIN_SET_ONLY_SWITCH",
    "USES_P001",  # Switch
    "USES_P004",  # Dallas DS18b20
    "USES_P005",  # DHT11/22
    "USES_P012",  # LCD 2004
    "USES_P026",  # SysInfo
    "USES_P028",  # BME280
    "USES_P036",  # FrameOLED
    "USES_P037",  # MQTTImport
    "USES_P052",  # SenseAir
    "USES_P087",  # Serial Proxy

    "USE_SETTINGS_ARCHIVE"
  ])


my_flags = env.ParseFlags(env['BUILD_FLAGS'])
my_defines = my_flags.get("CPPDEFINES")
#defines = {k: v for (k, v) in my_defines}

print("\u001b[32m Custom PIO configuration check \u001b[0m")
# print the defines
print("\u001b[33m CPPDEFINES: \u001b[0m  {}".format(my_defines))
print("\u001b[33m Custom CPPDEFINES: \u001b[0m  {}".format(env['CPPDEFINES']))
print("\u001b[32m ------------------------------- \u001b[0m")


if (len(my_defines) == 0):
  print("\u001b[31m No defines are set, probably configuration error. \u001b[0m")
  raise ValueError

