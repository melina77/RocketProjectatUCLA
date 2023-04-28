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
1. Install HomeBrew package manager at https://brew.sh/, this will make the other steps easier
2. Clone the git repository/download the zip and unzip it
3. Install grafana https://grafana.com/grafana/download?platform=mac
4. Login to grafana at localhost:3000 (in browser) using username and password of `admin`, then skip creating a new password (If this doesn't work, run `brew services restart grafana`
5. Go to Configuration → API Keys → Add API Key and give it admin perms, name it anything, and copy this
6. Go into the `.conf` files in the files you downloaded from this repository, and replace the bearer key with your API key
7. Install telegraf by running `brew install telegraf` in terminal
8. Inside of grafana, go to Dashboards → Import or look for any plus symbol that lets you upload a JSON file. Then copy or import the json labeled `dashboard_gs.json` inside of the `/ground-systems` folder you downloaded
9. `cd` into the `ground-systems/telegraf_configs` folder
10. Run `telegraf --config prometheus.conf` and `telegraf --config lc_influx.conf` in separate terminal instances
11. Run `ground-systems/dummy_data.py` from VS Code using python3. If that doesn't work, run it from terminal by cding into ground-systems and running `python3 dummy_data.py`
