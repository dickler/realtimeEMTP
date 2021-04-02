import matplotlib.pyplot as plt
import csv

x = []
y = []
y1= []

with open('Debug/example.csv','r') as csvfile:
    plots = csv.reader(csvfile, delimiter='\t')
    header = 0;
    for row in plots:
	if header == 0:
		xh = str(row[0])
		yh = str(row[2])
		header = 1
	else:
		x.append(float(row[0]))
		y.append(float(row[2]))

plt.plot(x,y)
plt.xlabel(xh)
plt.ylabel(yh)
plt.legend()
plt.show()
