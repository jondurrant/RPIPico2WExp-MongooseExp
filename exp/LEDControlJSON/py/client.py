import requests
import os
import json
import sys


hostname = "localhost"
port = 80

color = {
    "red": 0,
    "grn": 0,
    "blu": 0
    }


if len(sys.argv) < 4:
    print("Error Usage: client.py <red> <grn> <blu> [host] [port]")
    sys.exit(1)

color["red"] = int(sys.argv[1])
color["grn"] = int(sys.argv[2])
color["blu"] = int(sys.argv[3])


if len(sys.argv) > 4:
    hostname = sys.argv[4]

if len(sys.argv) > 5:
    port = int(sys.argv[5])

url = ('http://%s:%d/api/led'%(hostname, port))
newHeaders = {'Content-type': 'application/json', 'Accept': 'text/plain'}

print("URL: " + url)

x = requests.post(url, json=color, headers=newHeaders)
if (not x.ok):
    print("HTTP ERROR Code %d"%x.status_code)
else:
    t = x.json()
    if (t['status'] == "OK"):
        print("Submitted OK")
    else:
        print("Error submitting")
        print(t)