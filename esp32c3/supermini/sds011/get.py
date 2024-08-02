import urllib.request, json 

esp32c3 = "http://10.0.0.243"

with urllib.request.urlopen(esp32c3) as url:
    data = json.load(url)
    print(data)
