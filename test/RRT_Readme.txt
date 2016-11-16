RRT.cfg is the configuration file for RRT search

To config RRT search:
1) change iteration times
2) change step size (default = 50 when maximum window size = 10000)
3) change parameter settings: init max min
4) change performance settings: goal region, max, min

To run RRT Uniform Sampling:

1)Run RRT.cfg, the tree data will be saved in "bin" folder
2)Change Working Directory of plotRRTTree.py
3)Run plotRRTTREE.py

The program will not halt when the tree enter the goal region

To automatically halt, uncomment 176-179 lines in RRT.cpp