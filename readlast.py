import sqlite3
#import time
import datetime
#import random
#import dateutil
import matplotlib.pyplot as plt

conn = sqlite3.connect('/home/ddskrjo/tempsensor/esp32-tempsensor/temperatures.db')
c = conn.cursor()

def read_from_db():
    sensor1str = "vallen%"
    sensor2str = "cirkel%"
    oneweekago = datetime.datetime.now()- datetime.timedelta(days=7)
    weekStr = oneweekago.strftime("%Y-%m-%d")
    print(weekStr)

    c.execute('SELECT sensor_id, temperature, timestamp FROM readings where timestamp > \'' + 
     weekStr + '\' and (sensor_id like \'' +
     sensor1str + '\' or sensor_id LIKE \'' + sensor2str + '\') order by timestamp desc')

    #c.execute('SELECT * FROM readings')
    data = c.fetchall()
    for i in range(0,10):
        print(f"{i}:{data[9-i]}")

read_from_db()

