import sqlite3
import datetime
import matplotlib.pyplot as plt
import numpy as np
import matplotlib.ticker as ticker
from matplotlib.dates import AutoDateFormatter, AutoDateLocator, ConciseDateFormatter

webPath = '/home/ddskrjo/public_html/'

conn = sqlite3.connect('/home/ddskrjo/t/tempsensor/esp32-tempsensor/temperatures.db')
c = conn.cursor()

font = {'family' : 'normal',
        'weight' : 'bold',
        'size'   : 14}

plt.rc('font', **font)

def read_from_db():
    c.execute('SELECT * FROM readings')
    data = c.fetchall()
    print(data[-1])
    #for row in data:
    #    print(row)

def graph_data_vallen_2sensors():
    sensor1str = "vallen kok"
    sensor2str = "vallen ute"
    savedTemperatures=[]
    oneweekago = datetime.datetime.now()- datetime.timedelta(days=7)
    weekStr = oneweekago.strftime("%Y-%m-%d")
    print(weekStr)

    c.execute('SELECT sensor_id, temperature, timestamp FROM readings where timestamp > \'' + 
     weekStr + '\' and (sensor_id like \'' +
     sensor1str + '\' or sensor_id LIKE \'' + sensor2str + '\')')
    data = c.fetchall()

    sensor1Temp = 0
    sensor2Temp = 0
    dates = []
    temperatures1 = []
    temperatures2 = []
    currentDate = 0
    currentDate1Str = ""
    currentDate2Str = ""
    for row in data:
        if row[0] == sensor1str:
            currentDate1Str = row[2] 
            currentDate = datetime.datetime.strptime(row[2], '%Y-%m-%d %H:%M:%S%z')
            sensor1Temp = row[1]
            dates.append(currentDate)
            temperatures1.append(sensor1Temp)
            temperatures2.append(sensor2Temp)
        if row[0] == sensor2str:
            currentDate2Str = row[2] 
            sensor2Temp = row[1]

    label1Str = "{0:0.1f}".format(sensor1Temp)
    label2Str = "{0:0.1f}".format(sensor2Temp)

    plt.plot(dates,temperatures1, label=label1Str, linewidth=3)
    plt.plot(dates,temperatures2, label=label1Str, linewidth=3)

    plt.legend(loc="center right")
    #plt.savefig(webPath + '/temperature_readings.png')
    plt.show()
    print (currentDate1Str)
    print (currentDate2Str)
    f = open("temperature_dates.txt", "w")
    f.write(currentDate1Str)
    f.write("<br>")
    f.write(currentDate2Str)
#read_from_db()

# graph_data(["cirkelstigen1", "cirkelstigen2"])
#graph_data(["vallen ute", "vallen kok"])
graph_data_vallen_2sensors()

