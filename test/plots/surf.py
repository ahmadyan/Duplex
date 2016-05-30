from mpl_toolkits.mplot3d import Axes3D
from matplotlib import cm
from matplotlib.ticker import LinearLocator, FormatStrFormatter
import matplotlib.pyplot as plt
import numpy as np

def plot_surf():
    fig = plt.figure()
    ax = fig.gca(projection='3d')

    x = np.arange(-1, 1.00, 0.05)
    y = np.arange(-1, 1.00, 0.05)
    x, y = np.meshgrid(x, y)
    # z = x**2 - y**2 # saddle points
    # monkey saddle point function, the point (0,0) is a critical saddle point
    z = x**3 - 3*x*(y**2) # monkey saddle point

    surf = ax.plot_surface(x, y, z, rstride=1, cstride=1, cmap=cm.afmhot, linewidth=0, antialiased=False)
    #ax.set_zlim(-1.01, 1.01)
    ax.zaxis.set_major_locator(LinearLocator(10))
    ax.zaxis.set_major_formatter(FormatStrFormatter('%.02f'))
    fig.colorbar(surf, shrink=0.5, aspect=5)
    plt.show()

plot_surf()

#initial states
init = [-0.75, -0.75]
