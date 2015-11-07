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
        matlabfile = "C:\\Users\\adel\\code\\Duplex\\bin\\duplex.m"
    else:
        workingDirectory = "/Users/adel/code/Duplex/bin/"
        duplexCommand = '/Users/adel/code/Duplex/xcode/Duplex.xcode/build/Debug/Duplex.xcode'
        configfile = '/Users/adel/code/Duplex/bin/generic.cfg'
        matlabfile = '/Users/adel/code/Duplex/bin/duplex.m'

    print(workingDirectory)
    os.chdir(workingDirectory)
    total = 100
    with open(matlabfile, 'w') as f:
        f.write('error=zeros('+ str(total) +', 100);\n')

    for i in range(1, total):
        os.environ['DUPLEX_SIM_ID'] = str(i)
        duplexArgument = '--config'
        duplex = subprocess.Popen([duplexCommand, duplexArgument, configfile], stdout=subprocess.PIPE, stderr=subprocess.PIPE)
        output, error = duplex.communicate('\n')
        print(output)

        error = []
        root = xml.etree.ElementTree.parse('inv_save'+str(i)+'.xml').getroot()
        for iterations in root.findall('./stat/iteration/error'):
            error.append(float(iterations.text))

        with open(matlabfile, 'a') as f:
            f.write('error('+str(i)+',:)=' + str(error)+';\n')
        time.sleep(1)

if  __name__ =='__main__':main()
