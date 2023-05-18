## All areas marked <!> are areas that need to be verified to match the data being received
from serial import Serial
import socket
import time
import re

# CONSTANTS
# the port the arduino is attached to (i.e. COM3 on windows and '/dev/ttyUSB0' on macs)

# ser = Serial('/dev/cu.usbmodem131488301', 9600, timeout=1) is what worked for X1. Remove this comment after hotfire working
# ls -l /dev/cu*
# to find ports on mac

PORT = '/dev/cu.usbmodem1433401'
BAUDRATE = 9600

# grafana labels
measurement = 'sensorvals'

# <!> Fields must match and be in order of received data
#field_keys = ["pt1", "pt2", "pt3", "pt4", "lc1", "lc2"]
field_keys = ["pt1", "pt2", "pt3", "pt4", "pt5","lc1", "ac1", "ac2", "ac3", "ac4", "ac5"]


# not sure if we're getting time sent from arduino but i'll put this here just in case :)
def getTime():
    return time.time_ns()

# Initialize the import stuff
start_time = str(getTime())
#HOTFIRE EDIT REMOVED LOGFILE
logfile = 'logs/data' + start_time + '.csv'
with open(logfile, 'w+', newline='') as f:
    pass
    # f.write('time,' + ','.join(field_keys))
UDPClientSocket = socket.socket(family=socket.AF_INET, type=socket.SOCK_DGRAM)
serverAddressPort = ('127.0.0.1', 4000)
ser = Serial(PORT, BAUDRATE, timeout=0.1)

while True:
    try:
        print('ser\n')
        print(ser)
        ser = Serial(PORT, BAUDRATE, timeout=0.1)
        print('ser\n')

        print(ser)
        print('ser\n')
    except:
        continue
    finally:
        print('Successfully connected to Serial device')
        break


# SMOOOOTHNESSSSSS
N = 3


while True:
    lc1_avgs = []
    lc2_avgs = []
    for i in range(N):
        timestamp = str(getTime())
        # get everything except the newline character (MIGHT not need that .decode part)
        line = ser.readline().decode()

# Processing data
# Original Format: P1: 111, P2: 222, P3: 333 P4: 444 LC1: 5.55 LC2: 6.66
        #data = line.strip().split(',')
        line_processing = re.sub(r'[A-Z]+\d:', ',', line)

        # Remove the first comma
        line_processing = line_processing.replace(",", "", 1)

        # Remove all whitespace
        line_processing = "".join(line_processing.split())
        data = line_processing.split(',')

        # under the assumption that the data is sent as simply:
        # pt1,pt2,pt3,pt4,tc1,tc2,tc4,lc1,lc2

        # create a string to send over to grafana
        fields = ''
        for key,val in zip(field_keys, data):
            fields += f'{key}={val},'
        # remove that last ,
        fields = fields.strip(',')

        # create influx string
        influx_string = measurement + ' ' + fields + ' ' + timestamp
        # print(influx_string)
        UDPClientSocket.sendto(influx_string.encode(), serverAddressPort)

        # LOAD CELL STUFF pain.
        # lc1_avgs.append(data[-2]) # append load cell data to the thing
        # lc2_avgs.append(data[-1])

# save data CURRENTLY INACTIVE
# HOTFIRE EDIT DISABLED LOG
        if len(data) >5:
            try:
                with open(logfile, 'a') as f:
                    f.write(str(int(timestamp)-int(start_time)) + ' ' + ','.join(data) + '\n')
            except:
                print('Data logging failure')
                continue

    # send smooth lc values over
    # try:
    #     lc_fields = f'lc1={sum(list(map(float,lc1_avgs)))/N},lc2={sum(list(map(float,lc2_avgs)))/N}'
    #     lc_data_string = measurement + ' ' + lc_fields + str(timestamp)
    #     UDPClientSocket.sendto(lc_data_string.encode(), ('127.0.0.1', 4001))
    # except:
    #     continue