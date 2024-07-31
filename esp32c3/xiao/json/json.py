import json
from urllib.request import urlopen

f = urlopen("https://www.openml.org/d/40996/json")
j = json.load(f)
