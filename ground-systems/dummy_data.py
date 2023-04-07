import time
import socket
import random

# not used in serial version since arduino is what sends the data
def getTime():
    return time.time_ns()

# Initialize stuff
measurement = 'sensorvals'
field_keys = ["pt1", "tank", "pneumatics", "fill", "tc1", "tc2", "tc3", "tc4", "lc1", "lc2"]
num_fields = len(field_keys)
field_values = [100 for i in range(num_fields)]

UDPClientSocket = socket.socket(family=socket.AF_INET, type=socket.SOCK_DGRAM)
serverAddressPort = ('127.0.0.1', 4000)
N = 3
while True:
    lc_avgs = []
    for i in range(N):
        # Update data (we'd read from serial here)
        timestamp = getTime()
        for i in range(num_fields):
            field_values[i] = random.gauss(100, 50)
        
        # update field string
        fields = ''
        for i in range(num_fields):
            fields += f'{field_keys[i]}={field_values[i]}'
            if i != num_fields-1:
                fields += ','
        
        # influx string to send to telegraf
        data = measurement + ' ' + fields + ' ' + str(timestamp)
        UDPClientSocket.sendto(data.encode(), serverAddressPort)

        # sending load cell on a different stream
        lc_value = random.gauss(100, 50)
        lc_avgs.append(lc_value)
        


        # to simulate arduino delay
        time.sleep(0.0001)
    lc_fields = f'lc={sum(lc_avgs)/N + (int(str(time.time_ns())[-10:-7])+400)}'
    lc_data = measurement + ' ' + lc_fields + ' ' + str(timestamp)
    UDPClientSocket.sendto(lc_data.encode(), ('127.0.0.1', 4001))