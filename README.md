# double_lane_traffic
Simulating the motion of vehicles on a double lane road with periodic boundary conditions.
\
Resources :\
https://arxiv.org/abs/cond-mat/9512119v1
\
https://ieeexplore.ieee.org/document/7867412/
\
compile via:\
g++ serial_traffic.cpp -o serial_traffic\
g++ -fopenmp parallel_traffic.cpp -o parallel_traffic
