import sys
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
    sensor3str = "vallen rum"
    sensor4str = "vallen ladugard"
    #oneweekago = datetime.datetime.now()- datetime.timedelta(days=7)
    #weekStr = oneweekago.strftime("%Y-%m-%d")

    c.execute('SELECT sensor_id, temperature, timestamp FROM readings where ' + 
     ' (sensor_id like \'' + sensor1str + '\' or sensor_id LIKE \'' + sensor2str + '\' or sensor_id LIKE \'' + sensor3str + '\' or sensor_id LIKE \'' + sensor4str + '\')')

    #c.execute('SELECT * FROM readings')
    data = c.fetchall()
    f = open(sys.argv[1], 'w')
    f.write('date,temp_ute,temp_kok,temp_rum,temp_ladugard\n')
    oldTempUte = 0
    oldTempRum = 0
    oldTempLadugard = 0
    for row in data:
        if row[0] == sensor2str:
            oldTempUte = row[1]
        elif row[0] == sensor3str:
            oldTempRum = row[1]
        elif row[0] == sensor4str:
            oldTempLadugard = row[1]
        else:
            f.write(row[2][:-6] + f",{oldTempUte:.2f},{row[1]:.2f},{oldTempRum:.2f},{oldTempLadugard:.2f}")
            f.write("\n")
    f.close()

# /home/ddskrjo/public_html/tempdata.csv'
if len(sys.argv) > 1:
    read_from_db()
else:
    print(f"{sys.argv[0]} <path to output file.csv>")
