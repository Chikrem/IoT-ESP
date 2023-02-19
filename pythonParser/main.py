import json

file_name = "log.txt"
file = open(file_name, "r")
data = []
order = ["SSID", "RSSI", "type"]

for line in file.readlines():
    details = line.split("|")
    details = [x.strip() for x in details]
    structure = {key: value for key, value in zip(order, details)}
    data.append(structure)


with open("output.json", "w") as output_file:
    json.dump(data, output_file, indent=3)

with open("output.json", "r") as json_file:
    data = json.load(json_file)

for entry in data:
    rssi = entry.get("RSSI")
    if rssi is not None and isinstance(rssi, (int, float, str)):
        print("RSSI:", rssi)

with open("rssi_values.txt", "w") as rssi_file:
    for entry in data:
        rssi = entry.get("RSSI")
        if rssi is not None and isinstance(rssi, (int, float, str)):
            rssi_file.write(str(rssi) + "\n")
