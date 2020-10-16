import math
import numpy as np

def fmt(value):
    return "%.3f" % value

period = [4, 2, 4]
radius = 1.5
timestep = 0.02
maxtime = max(period)*3
timemult = [1, 1, 1]
phase=[0,0,0]
amp = [1,0.4,.5]
center = [0, 0, -2]

t = np.arange(0, maxtime, timestep)
x = np.sin(t * 2 * np.pi / period[0] + phase[0]) * radius * amp[0] + center[0]
y = np.sin(t * 2 * np.pi / period[1] + phase[1]) * radius * amp[1] + center[1]
z = np.sin(t * 2 * np.pi / period[2] + phase[2]) * radius * amp[2] + center[2]
vx = 2 * np.pi * amp[0] * radius * np.cos(t * 2 * np.pi / period[0] + phase[0]) / period[0]
vy = 2 * np.pi * amp[1] * radius * np.cos(t * 2 * np.pi / period[1] + phase[1]) / period[1]
vz = 2 * np.pi * amp[2] * radius * np.cos(t * 2 * np.pi / period[2] + phase[2]) / period[2]

with open('FigureEightFF.txt', 'w') as the_file:
    for i in range(t.size):
        the_file.write(fmt(t[i]) + "," + fmt(x[i]) + "," + fmt(y[i]) + "," + fmt(z[i]))
        the_file.write("," + fmt(vx[i]) + "," + fmt(vy[i]) + "," + fmt(vz[i]))
        the_file.write("\n");    
