import os
import subprocess
import xml.etree.ElementTree
import time

def main():
    total = 10
    with open('matlab.txt', 'w') as f:
        f.write('error=zeros('+ str(total) +', 100);\n')

    for i in range(1, total):
        os.environ['DUPLEX_SIM_ID'] = str(i)
        duplexCommand = '/Users/adel/code/Duplex/xcode/Duplex.xcode/build/Debug/Duplex.xcode'
        duplexArgument = '--config'
        configfile = '/Users/adel/code/Duplex/bin/generic.cfg'
        duplex = subprocess.Popen([duplexCommand, duplexArgument, configfile], stdout=subprocess.PIPE, stderr=subprocess.PIPE)
        output, error = duplex.communicate('\n')
        print output

        error = []
        root = xml.etree.ElementTree.parse('save'+str(i)+'.xml').getroot()
        for iterations in root.findall('./stat/iteration/error'):
            error.append(float(iterations.text))

        with open('matlab.txt', 'a') as f:
            f.write('error('+str(i)+',:)=' + str(error)+';\n')
        time.sleep(1)

if  __name__ =='__main__':main()
