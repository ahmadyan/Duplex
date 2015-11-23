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
        duplexSavefile = "C:\\Users\\adel\\code\\Duplex\\bin\\opamp_result1.xml"
    else:
        workingDirectory = "/Users/adel/code/Duplex/bin/"
        duplexCommand = '/Users/adel/code/Duplex/xcode/Duplex.xcode/build/Debug/Duplex.xcode'
        configfile = '/Users/adel/code/Duplex/bin/generic.cfg'
        matlabfile = '/Users/adel/code/Duplex/bin/yield_samples.m'

    os.chdir(workingDirectory)
    root = xml.etree.ElementTree.parse(duplexSavefile).getroot()

    for p in root.findall('./data/node'):
        for obj in p.iter('objective'):
            objectives = obj.text.split()
            for i,x in enumerate(objectives):
                objectives[i]=float(x)
        for param in p.iter('parameter'):
            parameters = param.text.split()
            for i,x in enumerate(parameters):
                parameters[i]=float(x)


    size = len(pareto)
    variableSize = 2
    with open(matlabfile, 'w') as f:
        f.write('yield=zeros('+ str(size) +','+ str(variableSize) +');\n')
        for i in range(1, size+1):
            f.write('yield('+str(i)+',:)=' + str(pareto[i-1]) +';\n')
        f.write('scatter(yield(:,1), yield(:,2))\n')

if  __name__ =='__main__':main()
