# RocketProjectatUCLA
The hub encompassing all code for Project Prometheus 2022-23.

#### Dependencies
- Git
- Python3
- Telegraf
- Grafana
- VS Code

### Step-By-Step Installation

#### Mac Instructions
1. If you don't have python3, download at https://www.python.org/downloads/
2. Install HomeBrew package manager at https://brew.sh/, this will make the other steps easier
3. Clone the git repository/download the zip and unzip it
4. Install grafana https://grafana.com/grafana/download?platform=mac
5. Login to grafana at localhost:3000 (in browser) using username and password of `admin`, then skip creating a new password (If this doesn't work, run `brew services restart grafana`. Also if you would like to stop the process run 'brew services stop grafana`.)
6. Go to Configuration → API Keys → Add API Key and give it admin perms, name it anything, and copy this
7. Go into the `.conf` files in the files you downloaded from this repository, and replace the bearer key with your API key
8. Install telegraf by running `brew install telegraf` in terminal
9. Inside of grafana, go to Dashboards → Import or look for any plus symbol that lets you upload a JSON file. Then copy or import the json labeled `dashboard_gs.json` inside of the `/ground-systems` folder you downloaded
10. `cd` into the `ground-systems/telegraf_configs` folder
11. Run `telegraf --config prometheus.conf` and `telegraf --config lc.conf` in separate terminal instances
12. Test: Run `ground-systems/dummy_data.py` from VS Code using python3. If that doesn't work, run it from terminal by cding into ground-systems and running `python3 ground-systems/dummy_data.py`. Check Grafana, it should be displaying data. 
13. When you actually need to run stuff, run `main.py` instead of dummy data.

##### IF YOU ALREADY DID THIS SETUP BEFORE AND ARE SIMPLY GETTING A NEW MAIN FILE AND DASHBOARD
1. Download the new repository, and replace **main.py**, **dummy_data.py**, and the **grafana dashboards** in your old repository with the new versions. (Do not just replace the old repo on your machine or you'll have to redo the API key which is annoying)
2. Follow step 9 to load in the new dashboard
3. Follow steps 10-11 to run telegraf
4. Follow steps 12-13 to run it

#### Windows Instructions
1. If you don't have python3, download at https://www.python.org/downloads/ or download the VScode extension
2. Clone the git repository/download the zip and unzip it
3. Install Grafana https://grafana.com/grafana/download?platform=windows
4. Download Telegraf by copying and pasting the following commands in Powershell
```
cd ~/Downloads

wget https://dl.influxdata.com/telegraf/releases/telegraf-1.22.4_windows_amd64.zip -UseBasicParsing -OutFile telegraf-1.22.4_windows_amd64.zip

Expand-Archive .\telegraf-1.22.4_windows_amd64.zip -DestinationPath 'C:\Program Files\InfluxData\telegraf'
```
**IMPORTANT:** If you have to extract the zip files manually ensure that the above file path is the destination

5. Login to grafana at localhost:3000 (in browser) using username and password of `admin`, then skip creating a new password
6. Go to Configuration → API Keys → Add API Key and give it admin perms, name it anything, and copy this
7. Go into the `.conf` files in the files you downloaded from this repository, and replace the bearer key with your API key (just lc.conf and prometheus.conf Authorization = "API KEY HERE")
8. Inside of grafana, go to Dashboards → Import or look for any plus symbol that lets you upload a JSON file. Then copy or import the json labeled `dashboard_gs.json` inside of the `/ground-systems` folder you downloaded
9. `cd` into the `ground-systems/windows` folder in two powershell instances
10. Run `.\start-telegraf.ps1` in one instance and `.\start-telegraf-2.ps1` in the other
11. Test: Run `ground-systems/dummy_data.py` from VS Code using python3 in terminal or the extension start button. If that doesn't work, run it from powershell by cding into ground-systems and running `python3 dummy_data.py`. Check Grafana, it should be displaying random data. When you actually need to run stuff, run `main.py` instead
