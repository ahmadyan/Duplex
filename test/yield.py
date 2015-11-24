import os
import subprocess
import platform
import xml.etree.ElementTree
import time

global workingDirectory
global duplexCommand
global configfile
global matlabfile

def isWithinTheGoalRegion(objectives):
    isOptimalState = True
    if (objectives[0] < 2e9):       # bandwidth is less than 2GHz
        isOptimalState=False
    if (objectives[1] < 30):       #gain
        isOptimalState=False
    if (objectives[2] < 5):       #gm
        isOptimalState=False
    if (objectives[3] < 45):       #phase
        isOptimalState=False
    return isOptimalState

def main():
    if (platform.system() == "Windows"):
        workingDirectory = "C:\\Users\\adel\\code\\Duplex\\bin"
        duplexCommand = "C:\\Users\\adel\\code\\Duplex\\DuplexVS\\DuplexVS.exe"
        configfile = "C:\\Users\\adel\\code\\Duplex\\bin\\inverter.cfg"
        matlabfile = "C:\\Users\\adel\\code\\Duplex\\bin\\yield_samples.m"
        duplexsavefile = "C:\\Users\\adel\\code\\Duplex\\bin\\opamp-5k.xml"
    else:
        workingDirectory = "/Users/adel/code/Duplex/bin/"
        duplexCommand = '/Users/adel/code/Duplex/xcode/Duplex.xcode/build/Debug/Duplex.xcode'
        configfile = '/Users/adel/code/Duplex/bin/generic.cfg'
        matlabfile = '/Users/adel/code/Duplex/bin/yield_samples.m'

    os.chdir(workingDirectory)
    iteration = 0
    root = xml.etree.ElementTree.parse(duplexsavefile).getroot()
    pareto = []
    variableSize=7
    for p in root.findall('./data/node'):
        iteration += 1
        print(iteration)
        if(iteration<2000):
            candid = False
            for obj in p.iter('objective'):
                objectives = obj.text.split()
                for i,x in enumerate(objectives):
                    objectives[i]=float(x)
                candid = isWithinTheGoalRegion(objectives)
            if(candid):
                for param in p.iter('parameter'):
                    parameters = param.text.split()
                    for i,x in enumerate(parameters):
                        parameters[i]=float(x)
                        variableSize=i
                    pareto.append(parameters)

    size = len(pareto)
    with open(matlabfile, 'w') as f:
        f.write('yield=zeros('+ str(size) +','+ str(variableSize) +');\n')
        for i in range(1, size+1):
            f.write('yield('+str(i)+',:)=' + str(pareto[i-1]) +';\n')
        f.write('scatter(yield(:,1), yield(:,2))\n')

if  __name__ =='__main__':main()
