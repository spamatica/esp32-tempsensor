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

def graph_data_3sensors(sensor1str, sensor2str, sensor3str):
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
    gotTemp1 = False
    gotTemp2 = False
    gotTemp3 = False

    for row in data:
        if row[0] == sensor1str:
            currentDate1Str = row[2] 
            currentDate = datetime.datetime.strptime(row[2], '%Y-%m-%d %H:%M:%S%z')
            sensor1Temp = row[1]
            gotTemp1 = True
        elif row[0] == sensor2str:
            currentDate2Str = row[2] 
            sensor2Temp = row[1]
            gotTemp2 = True
        elif row[0] == sensor3str:
            currentDate3Str = row[2]
            sensor3Temp = row[1]
            gotTemp3 = True

        if gotTemp1 and gotTemp2 and gotTemp3:
            # when we got samples from all sensors we start populating the plot
            dates.append(currentDate) # date from sensor1 (the granularity is high enough that it does not matter)
            temperatures1.append(sensor1Temp)
            temperatures2.append(sensor2Temp)
            temperatures3.append(sensor3Temp)

    label1Str = "{0:0.1f}".format(sensor1Temp)
    plt.plot(dates,temperatures1, label=label1Str, linewidth=3)

    if sensor2str != "":
        label2Str = "{0:0.1f}".format(sensor2Temp)
        plt.plot(dates,temperatures2, label=label2Str, linewidth=3)

    if sensor3str != "":
        label3Str = "{0:0.1f}".format(sensor3Temp)
        plt.plot(dates,temperatures3, label=label3Str, linewidth=3)

    plt.legend(loc="center left")
    plt.savefig(webPath + '/temperature_readings_ladugard.png')

    # f = open(webPath + "/temperature_dates.inc", "w")
    # f.write(currentDate1Str)
    # f.write("<br>")
    # f.write(currentDate2Str)
    # f.write("<br>")
    # f.write(currentDate3Str)

#########
sensor1str = "vallen ute"
sensor2str = "vallen kok"
sensor3str = "vallen ladugard"
graph_data_3sensors(sensor1str, sensor2str, sensor3str)

