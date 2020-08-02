import urllib.request, json
import matplotlib.pyplot as plt
import matplotlib.dates as md
import datetime as dt
from datetime import datetime

#variable definitions
eCO2 = []
tvoc = []
temp = []
hum = []
prs = []
lux = []
timestamp = []

# main loop starts here

#get user input
print("Insert plot start date: ")
date_entry = input('Enter a date in YYYY-MM-DD-HH-MM format: ')
year, month, day , hour, minute = map(int, date_entry.split('-'))
date1 = dt.datetime(year,month,day,hour,minute)
print(" ")
print("Insert plot end date: ")
date_entry = input('Enter a date in YYYY-MM-DD-HH-MM format: ')
year, month, day , hour, minute = map(int, date_entry.split('-'))
date2 = dt.datetime(year,month,day,hour,minute)

print(" ")
print("Which parameter do you want to plot?")
print("1.Temperature")
print("2.Humidity")
print("3.Pressure")
print("4.Light Intensitiy")
print("5.eCO2")
print("6.TVOC")

shownParameter=int(input())

print(" ")
yMinValue=int(input("Insert Y Axis minimal value: "))
yMaxValue=int(input("Insert Y Axis maximal value: "))

timestampStart =date1.timestamp() *1000
timestampEnd =date2.timestamp() *1000



#retrieve json from noSQL db
from array import array 
with urllib.request.urlopen("https://c1bdf122-9382-42be-9cc1-4c50dfa0610c-bluemix.cloudant.com/iotdata/_design/Sort/_view/By_Date") as url:
    data = json.loads(url.read().decode())

#retrieve useful data from json
rows= data['rows']
total_rows = data['total_rows']
for i  in range(total_rows):
	value= rows[i]
	actualTimestamp =float(value['value']['payload']['Sensor1']['ts'])
	if (actualTimestamp>timestampStart and actualTimestamp<timestampEnd):
		temp.append(value['value']['payload']['Sensor1']['t'])
		hum.append(value['value']['payload']['Sensor1']['t'])
		prs.append(value['value']['payload']['Sensor1']['p'])
		lux.append(value['value']['payload']['Sensor1']['l'])
		eCO2.append(value['value']['payload']['Sensor1']['e'])
		tvoc.append(value['value']['payload']['Sensor1']['T'])
		# timestamp.append(float(value['value']['payload']['Sensor1']['ts']))
		temporal = datetime.fromtimestamp(int(actualTimestamp)/1000)
		timestamp.append(temporal)



#plot the data
ax=plt.gca()
xfmt = md.DateFormatter('%d-%m-%Y-%H:%M')
ax.xaxis.set_major_formatter(xfmt)
plt.gcf().autofmt_xdate()
ax.set_ylim([yMinValue,yMaxValue])


if shownParameter==1:
	ax.plot(timestamp,temp)
	plt.title('Evoluția Temperaturii')
	plt.ylabel('Temperatura')
elif shownParameter==2:	
	ax.plot(timestamp,hum)
	plt.title('Evoluția Umiditații')
	plt.ylabel('Umiditate')
elif shownParameter==3:	
	ax.plot(timestamp,prs)
	plt.title('Evoluția Presiunii')
	plt.ylabel('Presiune')
elif shownParameter==4:	
	ax.plot(timestamp,lux)
	plt.title('Evoluția Intensității Luminoase')
	plt.ylabel('Intensitatea Luminoasa')
elif shownParameter==5:	
	ax.plot(timestamp,eCO2)
	plt.title('Evoluția eCO2')
	plt.ylabel('eCO2')
elif shownParameter==6:	
	ax.plot(timestamp,tvoc)
	plt.title('Evoluția TVOC')
	plt.ylabel('TVOC')

plt.xlabel('Time')
plt.show()




