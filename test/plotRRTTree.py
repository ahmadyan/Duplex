from matplotlib import pyplot as ppl
import os
import numpy as np

MAX=10000
fig = ppl.figure(1)

ax = fig.add_subplot(1, 1, 1)
ax.axis([0, MAX, 0, MAX])
img = np.ones((MAX, MAX,3))

ax.imshow(img)




fo = open("C:\\MyGit\\RRTSearch\\bin\\RRTtreedata.txt")
for line in fo.readlines():
    p = line.split()
    ax.plot([p[0],p[2]],[p[1],p[3]], linestyle = '-', color = 'g', linewidth = 0.5)

ppl.savefig("uniformsampling.png", bbox_inches='tight', dpi = 200)
fo.close()