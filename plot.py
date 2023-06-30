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

# locator = AutoDateLocator(minticks=3, maxticks=7)
# formatter = ConciseDateFormatter(locator)
# plt.axes().xaxis.set_major_locator(locator)
# plt.axes().xaxis.set_major_formatter(formatter)

def read_from_db():
    c.execute('SELECT * FROM readings')
    data = c.fetchall()
    print(data[-1])
    #for row in data:
    #    print(row)

def graph_data(sensor_names):

    savedTemperatures=[]
    for name in sensor_names:
        c.execute('SELECT sensor_id, temperature, timestamp FROM readings where sensor_id LIKE \'' + name + '\' and timestamp > \'2022-01-01\'')
        data = c.fetchall()

        dates = []
        temperatures = []
    
        for row in data:
            currentTemp = row[1]
            datetimeobj = datetime.datetime.strptime(row[2], '%Y-%m-%d %H:%M:%S%z')
            dates.append(datetimeobj)
            temperatures.append(currentTemp)
        savedTemperatures.append(currentTemp)
        labelStr = "{0:0.1f}".format(currentTemp)

        plt.plot(dates,temperatures,'-', label=labelStr, linewidth=3)

    plt.legend(loc="center right")
    fig = plt.figure(frameon = False)
    fig.set_size_inches(5, 8)
    plt.savefig(webPath + '/temperature_readings.png', dpi=300)
    #plt.show()

def graph_data_vallen_2sensors():
    sensor1str = "vallen kok"
    sensor2str = "vallen ute"
    savedTemperatures=[]
    startTime = datetime.datetime.now()- datetime.timedelta(days=4)
    startTimeStr = startTime.strftime("%Y-%m-%d")
    print(startTimeStr)
    c.execute('SELECT sensor_id, temperature, timestamp FROM readings where timestamp > \'' + 
     startTimeStr + '\' and (sensor_id like \'' +
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
    plt.plot(dates,temperatures2, label=label2Str, linewidth=3)

    plt.legend(loc="center right")
    plt.savefig(webPath + '/temperature_readings.png')
    #plt.show()
    #print (currentDate1Str)
    #print (currentDate2Str)
    f = open(webPath + "/temperature_dates.inc", "w")
    f.write(currentDate1Str)
    f.write("<br>")
    f.write(currentDate2Str)
#read_from_db()

# graph_data(["cirkelstigen1", "cirkelstigen2"])
#graph_data(["vallen ute", "vallen kok"])
graph_data_vallen_2sensors()

