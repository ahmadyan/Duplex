% Duplex optimization
%
% Vectorized version of the Generalized Multi-variate Rosenbrock function 
%   https://en.wikipedia.org/wiki/Rosenbrock_function
% f(x, y) = sum_{i=1}^{N-1} 100 (x_{i+1} - x_i^2)^2 + (1-x_i)^2
% This function has exactly one minimum for N=3 at (1, 1, 1) and 
% two minimums for N=4-7 at (1, 1, ..., 1) and (-1, 1, ... 1).
function [y] = rosen(x)
   y = sum(100*(x(2:end) - x(1:end-1).^2).^2 + (x(1:end-1)-1).^2);
end
