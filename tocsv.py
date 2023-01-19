import sqlite3
#import time
import datetime
#import random
#import dateutil
import matplotlib.pyplot as plt

conn = sqlite3.connect('/home/ddskrjo/tempsensor/esp32-tempsensor/temperatures.db')
c = conn.cursor()

def read_from_db():
    sensor1str = "vallen kok"
    sensor2str = "vallen ute"
    #oneweekago = datetime.datetime.now()- datetime.timedelta(days=7)
    #weekStr = oneweekago.strftime("%Y-%m-%d")

    c.execute('SELECT sensor_id, temperature, timestamp FROM readings where ' + 
     ' (sensor_id like \'' +
     sensor1str + '\' or sensor_id LIKE \'' + sensor2str + '\')')

    #c.execute('SELECT * FROM readings')
    data = c.fetchall()
    f = open('/home/ddskrjo/public_html/tempdata.csv', 'w')
    f.write('date,temp_ute,temp_inne\n')
    oldTempUte = 0
    for row in data:
        if row[0] == sensor2str:
            oldTempUte = row[1]
        else:
            f.write(row[2][:-6] + "," + str(oldTempUte) + "," + str(row[1]))
            f.write("\n")
    f.close()
read_from_db()

