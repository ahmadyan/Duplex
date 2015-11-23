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
        matlabfile = "C:\\Users\\adel\\code\\Duplex\\bin\\duplex_figure.m"
        duplexSavefile = "C:\\Users\\adel\\code\\Duplex\\bin\\opamp_result2.xml"
    else:
        workingDirectory = "/Users/adel/code/Duplex/bin/"
        duplexCommand = '/Users/adel/code/Duplex/xcode/Duplex.xcode/build/Debug/Duplex.xcode'
        configfile = '/Users/adel/code/Duplex/bin/generic.cfg'
        matlabfile = '/Users/adel/code/Duplex/bin/yield_samples.m'

    os.chdir(workingDirectory)
    root = xml.etree.ElementTree.parse(duplexSavefile).getroot()
    data = []
    pdata = []
    for p in root.findall('./data/node'):
        for parent in p.iter('parent'):
            id=-1
            for me in p.iter('id'):
                id = int( me.text )
            for obj in p.iter('objective'):
                objectives = obj.text.split()
                for i,x in enumerate(objectives):
                    objectives[i]=float(x)
            for param in p.iter('parameter'):
                parameters = param.text.split()
                for i,x in enumerate(parameters):
                    parameters[i]=float(x)
            parent_id = int(parent.text)
            if (id!=0):
                pdata.append( data[parent_id] )
            else:
                pdata.append( str(parameters[0]) + ' ' + str(parameters[1]) )
            data.append(str(parameters[0]) + ' ' + str(parameters[1]))

    print(data)
    print (pdata)

    size = len(data)

    with open(matlabfile, 'w') as f:
        f.write('duplex=zeros('+ str(size) +','+ str(4) +');\n')
        for i in range(1, size+1):
            f.write('duplex('+str(i)+',:)=[ ' + str(data[i-1]) + ' ' + str(pdata[i-1]) + ' ];\n')

if  __name__ =='__main__':main()

# Code for drawing the random tree in MATLAB
#x = unique(m(:, 1));
#y = unique(m(:,2));
#contour(y, x, bw);
#hold on
#for i=1:500,
#    x1 = duplex(i, 1);
#    y1 = duplex(i, 2);
#    x2 = duplex(i, 3);
#    y2 = duplex(i, 4);

#    line( [x1 x2], [y1 y2], 'LineWidth', 1, 'Color', 'k' );
#end
#hold off
