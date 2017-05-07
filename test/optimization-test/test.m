% Duplex optimization
% Test for the rosenbrock function

% Plot the log-scaled rosenbrock function. Use log-scale to pronounce the
% valley.
f = @rosen;
[x, y] = meshgrid(-2:0.05:2, -2:0.05:2);
z = zeros(size(x, 1), size(x, 2));
for i = 1:size(x, 1),
    for j = 1:size(x, 2),
        z(i, j) = log(1+rosen([x(i, j) y(i, j)]));
    end
end
surfc(x,y,z);