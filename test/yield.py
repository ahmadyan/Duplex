import os
import subprocess
import platform
import xml.etree.ElementTree
import time

global workingDirectory
global duplexCommand
global configfile
global matlabfile

def main():
    if (platform.system() == "Windows"):
        workingDirectory = "C:\\Users\\adel\\code\\Duplex\\bin"
        duplexCommand = "C:\\Users\\adel\\code\\Duplex\\DuplexVS\\DuplexVS.exe"
        configfile = "C:\\Users\\adel\\code\\Duplex\\bin\\inverter.cfg"
        matlabfile = "C:\\Users\\adel\\code\\Duplex\\bin\\yield_samples.m"
    else:
        workingDirectory = "/Users/adel/code/Duplex/bin/"
        duplexCommand = '/Users/adel/code/Duplex/xcode/Duplex.xcode/build/Debug/Duplex.xcode'
        configfile = '/Users/adel/code/Duplex/bin/generic.cfg'
        matlabfile = '/Users/adel/code/Duplex/bin/yield_samples.m'

    os.chdir(workingDirectory)
    i = 0
    root = xml.etree.ElementTree.parse('inv_save'+str(i)+'.xml').getroot()
    pareto = []

    for p in root.findall('./data/node'):
        candid = False
        for obj in p.iter('objective'):
            objectives = obj.text.split()
            for i,x in enumerate(objectives):
                objectives[i]=float(x)
            if (objectives[0]<1.6e-11) and (objectives[1] > 0.00015):
                candid=True
        if(candid):
            for param in p.iter('parameter'):
                parameters = param.text.split()
                for i,x in enumerate(parameters):
                    parameters[i]=float(x)
                pareto.append(parameters)

    size = len(pareto)
    variableSize = 2
    with open(matlabfile, 'w') as f:
        f.write('yield=zeros('+ str(size) +','+ str(variableSize) +');\n')
        for i in range(1, size+1):
            f.write('yield('+str(i)+',:)=' + str(pareto[i-1]) +';\n')
        f.write('scatter(yield(:,1), yield(:,2))\n')
        
if  __name__ =='__main__':main()
