import os
import subprocess
import sys
import numpy as np
import matplotlib.pyplot as plt
from mpl_toolkits.axes_grid1.axes_divider import make_axes_locatable

# Constants
frameRate = 1000 #frames/s

def readExpansionFile():
    with open(sys.argv[2] + 'Alturas.txt', 'r') as file:
        frameArray = []
        experimentalHeightArray = []
        cont = 0
        for line in file:
            frame, height = [float(x) for x in line.split()] #read line by line
            frameArray.append(cont)
            experimentalHeightArray.append(height)
            cont += 1

    experimentalHeightArray = np.array(experimentalHeightArray)
    frameArray = np.array(frameArray)
    frameArray = frameArray/frameRate

    plt.plot(frameArray, experimentalHeightArray)
    plt.title('Bed expansion')
    plt.xlabel('Time [s]')
    plt.ylabel('Maximum height [mm]')
    plt.savefig(sys.argv[2] + 'Expansion.png', bbox_inches='tight')
    #plt.show(block=False)
    plt.show()
    plt.clf()

def readPlugsFile():
    with open(sys.argv[2] + 'Plugs.txt', 'r') as file:
        frameArray = []
        experimentalPlugsArray = []
        cont = 0
        for line in file:
            plug = int(line.split()[0])
            frameArray.append(cont)
            experimentalPlugsArray.append(plug)
            cont += 1

    frameArray = np.array(frameArray)/frameRate
    experimentalPlugsArray = np.array(experimentalPlugsArray)

    plt.plot(frameArray, experimentalPlugsArray)
    plt.title('Number of plugs')
    plt.xlabel('Time [s]')
    plt.ylabel('Plugs []')
    plt.savefig(sys.argv[2] + 'Plugs.png', bbox_inches='tight')
    plt.show()
    #plt.show(block=False)
    plt.clf()

def readTemperatureFile():
    with open(sys.argv[2] + 'GranularTemperature.txt', 'r') as file:
        frameArray = []
        experimentalTemperatureArray = []
        if file.readline() != '---Geometry---\n':
            print('Wrong file format')
            return

        heights = [x for x in file.readline().split()]
        heights = [float(x) for x in heights[1:]]
        widths = [x for x in file.readline().split()]
        widths = [float(x) for x in widths[1:]]

        for line in file:
            values = [x for x in line.split()]
            
            try:
                frameArray.append(int(values[0]))
            except:
                continue
            
            values = [float(x) for x in values[2:]]

            media = 0
            cont = 0
            for x in values:
                if (x != 0):
                    media += x
                    cont += 1

            media /= cont
            experimentalTemperatureArray.append(media)

    frameArray = np.array(frameArray)/frameRate
    experimentalTemperatureArray = np.array(experimentalTemperatureArray)

    plt.plot(frameArray, experimentalTemperatureArray)
    plt.title('Average granular temperature')
    plt.xlabel('Time [s]')
    plt.ylabel('Granular temperature [m^2/s^2]')
    plt.savefig(sys.argv[2] + 'Temperature.png', bbox_inches='tight')
    plt.show()
    #plt.show(block=False)
    plt.clf()

def readTemperatureField():

    fig = plt.figure()
    ax = plt.axes()
    cax = make_axes_locatable(ax).append_axes("right", size="5%", pad="2%")
    ax.set_title('Granular temperature')
    ax.set_xlabel('X [mm]')
    ax.set_ylabel('Y [mm]')

    with open(sys.argv[2] + 'GranularTemperature.txt', 'r') as file:
        frameArray = []
        experimentalTemperatureArray = []
        if file.readline() != '---Geometry---\n':
            print('Wrong file format')
            return

        heights = [x for x in file.readline().split()]
        heights = [float(x) for x in heights[1:]]
        widths = [x for x in file.readline().split()]
        widths = [float(x) for x in widths[1:]]

        heights.append(2*heights[-1] - heights[-2])
        widths.append(2*widths[-1] - widths[-2])
        ax.set_xlim([widths[0],widths[-1]])
        ax.set_ylim([heights[0],heights[-1]])

        Xaxis = [(widths[i]+widths[i+1])/2 for i in range(len(widths)-1)]
        Yaxis = [(heights[i]+heights[i+1])/2 for i in range(len(heights)-1)]
        X, Y = np.meshgrid(Xaxis, Yaxis)

        valuesList = []
        for line in file:
            values = [x for x in line.split()]
           
            try:
                frameArray.append(int(values[0]))
            except:
                continue
           
            values = [float(x) for x in values[2:]]
            Z = np.reshape(values,[len(Yaxis),len(Xaxis)])
            valuesList.append(Z)

        for i in range(10):
            plot = ax.contourf(X,Y,valuesList[i])
            fig.colorbar(plot, cax=cax)
            plt.pause(0.001)

    plt.show(block=True)
    #anim.save('Files/animation.gif', writer='imagemagick')

    #frameArray = np.array(frameArray)/frameRate
    #experimentalTemperatureArray = np.array(experimentalTemperatureArray)/(frameRate**2)

    plt.clf()

def readVelocityField():

    fig = plt.figure()
    ax = plt.axes()
    #cax = make_axes_locatable(ax).append_axes("right", size="5%", pad="2%")
    ax.set_title('Velocity field')
    ax.set_xlabel('X [mm]')
    ax.set_ylabel('Y [mm]')

    with open(sys.argv[2] + 'VelocityX.txt', 'r') as fileX:
        with open(sys.argv[2] + 'VelocityY.txt', 'r') as fileY:
            frameArray = []
            experimentalTemperatureArray = []
            if fileX.readline() != '---Geometry---\n' or fileY.readline() != '---Geometry---\n':
                print('Wrong file format')
                return

            heights = [x for x in fileX.readline().split()]
            heights = [float(x) for x in heights[1:]]
            widths = [x for x in fileX.readline().split()]
            widths = [float(x) for x in widths[1:]]

            heights.append(2*heights[-1] - heights[-2])
            widths.append(2*widths[-1] - widths[-2])
            ax.set_xlim([widths[0],widths[-1]])
            ax.set_ylim([heights[0],heights[-1]])

            Xaxis = [(widths[i]+widths[i+1])/2 for i in range(len(widths)-1)]
            Yaxis = [(heights[i]+heights[i+1])/2 for i in range(len(heights)-1)]
            X, Y = np.meshgrid(Xaxis, Yaxis)

            VelocityXList = []
            VelocityYList = []
            for line in fileX:
                values = [x for x in line.split()]

                try:
                    frameArray.append(int(values[0]))
                except:
                    continue

                values = [float(x) for x in values[2:]]
                U = np.reshape(values,[len(Yaxis),len(Xaxis)])
                VelocityXList.append(U)
            for line in fileY:
                values = [x for x in line.split()]

                try:
                    test = int(values[0])
                except:
                    continue

                values = [float(x) for x in values[2:]]
                V = np.reshape(values,[len(Yaxis),len(Xaxis)])
                VelocityYList.append(V)

            plot = ax.quiver(X,Y,VelocityXList[0],VelocityYList[0], scale=10)
            for i in range(len(VelocityXList)):
                plot.set_UVC(VelocityXList[i],VelocityYList[i])
                #fig.colorbar(plot, cax=cax)
                plt.pause(0.001)

    plt.show(block=True)
    #anim.save('Files/animation.gif', writer='imagemagick')

    #frameArray = np.array(frameArray)/frameRate
    #experimentalTemperatureArray = np.array(experimentalTemperatureArray)/(frameRate**2)

    plt.clf()

#main
#print(sys.argv[1])
#print(sys.argv[2])
if sys.argv[1] == '-expansion':

    readExpansionFile()

elif sys.argv[1] == '-temperature':

    readTemperatureFile() 

elif sys.argv[1] == '-plugs':

    readPlugsFile() 

elif sys.argv[1] == '-all':

    readExpansionFile() 
    readPlugsFile()
    readTemperatureFile()
    readTemperatureField()
    readVelocityField()

else:

    print('Modo desconhecido')
