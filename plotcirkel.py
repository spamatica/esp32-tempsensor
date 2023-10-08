import sqlite3
import datetime
import matplotlib.pyplot as plt
import numpy as np
import matplotlib.ticker as ticker
from matplotlib.dates import AutoDateFormatter, AutoDateLocator, ConciseDateFormatter

webPath = '/home/ddskrjo/public_html/'

conn = sqlite3.connect('/home/ddskrjo/tempsensor/esp32-tempsensor/temperatures.db')
c = conn.cursor()

font = {'family' : 'normal',
        'weight' : 'bold',
        'size'   : 12}

plt.rc('font', **font)

def graph_data_sensors(sensor1str, sensor2str, sensor3str):
    savedTemperatures=[]
    startTime = datetime.datetime.now()- datetime.timedelta(days=4)
    startTimeStr = startTime.strftime("%Y-%m-%d")
    print(startTimeStr)

    # build sql string with 1-3 search strings
    sqlStr = 'SELECT sensor_id, temperature, timestamp FROM readings where timestamp > \'' + startTimeStr + '\' and (sensor_id like \'' + sensor1str 
    if sensor2str != "":
         sqlStr += '\' or sensor_id LIKE \'' + sensor2str
    if sensor3str != "":
         sqlStr += '\' or sensor_id LIKE \'' + sensor3str
    sqlStr += '\')'

    c.execute(sqlStr)
    data = c.fetchall()

    sensor1Temp = 0
    sensor2Temp = 0
    sensor3Temp = 0
    dates = []
    temperatures1 = []
    temperatures2 = []
    temperatures3 = []
    currentDate = 0
    currentDate1Str = ""
    currentDate2Str = ""
    currentData3Str = ""
    for row in data:
        if row[0] == sensor1str:
            currentDate1Str = row[2] 
            currentDate = datetime.datetime.strptime(row[2], '%Y-%m-%d %H:%M:%S%z')
            sensor1Temp = row[1]
            dates.append(currentDate)

            # each time sensor1 is collected we store all three to their arrays
            temperatures1.append(sensor1Temp)
            temperatures2.append(sensor2Temp)
            temperatures3.append(sensor3Temp)

        if row[0] == sensor2str:
            currentDate2Str = row[2] 
            sensor2Temp = row[1]
        if row[0] == sensor3str:
            currentDate3Str = row[2]
            sensor3Temp = row[1]

    label1Str = "{0:0.1f}".format(sensor1Temp)
    plt.plot(dates,temperatures1, label=label1Str, linewidth=3)

    if sensor2str != "":
        label2Str = "{0:0.1f}".format(sensor2Temp)
        plt.plot(dates,temperatures2, label=label2Str, linewidth=3)

    if sensor3str != "":
        label3Str = "{0:0.1f}".format(sensor3Temp)
        plt.plot(dates,temperatures3, label=label3Str, linewidth=3)

    plt.legend(loc="center right")
    plt.savefig(webPath + '/temperature_readings_cirkel.png')

#########
sensor1str = "cirkel inne"
sensor2str = "cirkel ute"
sensor3str = ""
graph_data_sensors(sensor1str, sensor2str, sensor3str)

