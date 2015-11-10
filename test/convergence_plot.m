%Assumes error(samples,iterations) holds a single convergence plot

x = 100;                            %total iterations for each simulation
samples = 99;                       %total simulations
resolution= 100;
max = -1;                           %the maximum in y-axis
for i=1:samples,
  if error(i,1)>max,
    max=error(i,1);
  end
end
y = ceil(max/resolution);

ExError = zeros(1,x);
for i=1:x,
  e = 0;
  for j=1:samples,
    e = e + error(j, i);
  end
  ExError(i) = e/samples;
end

% Normalize the y-axis (maps it to [0,1])
% Then determines the bin by normalized-y * resolution
% Finally normalize the whole matrix by dividing it by samples
watercolor = zeros(resolution, x);
for i=1:samples,
  for j=1:x,
    bin = ceil( (error(i, j) / max) * resolution );
    watercolor(bin, j) = watercolor(bin, j)+1;
  end
end
watercolor = watercolor ./ samples;


%  Low-pass filter to smooth the results
% Only use this if the resolution is high to smoothen the plot
pass=10;
for p=1:pass,
  for i=2:resolution-1,%
    for j=1:x,
      %watercolor(i, j) = (watercolor(i-2,j) + 2*watercolor(i-1,j) + 3*watercolor(i,j) + 2*watercolor(i+1, j) +  watercolor(i+2, j) )/ 9;
      watercolor(i, j) = (watercolor(i-1,j) + watercolor(i,j) + watercolor(i+1,j) )/ 3;
    end
  end
end


flood = zeros(x);
for i=1:x,
  flood(i) = -1;
  for j=1:resolution,
    if(watercolor(j, i)>flood(i)),
      flood(i) = watercolor(j,i);
    end
  end
end

scale = 1.5;
for i=1:resolution,
  for j=1:x,
    watercolor(i, j) = scale*watercolor(i,j)/flood(j);
    if(watercolor(i,j)>1),
      watercolor(i,j)=1;
    end
  end
end

% Plot the Results
figure
hold on
for i=1:resolution-1,
  for j=1:x-1,
    p = [i-1    i   i   i-1   ];
    q = [j    j     j+1   j+1 ];
    p = p./resolution;
    if(watercolor(i,j) > 0),
      color = [ 1-watercolor(i, j) 1-watercolor(i, j)  1-watercolor(i, j) ];
    else,
      color = [1 1 1];
    end
    fill(q, p, color, 'EdgeColor', 'none')
  end
end

% over draw the expected error
ExError = ExError ./ max;
plot(ExError, 'k', 'LineWidth',2 );
axis tight
 e2 = error(20,:) ./ max;
 plot(e2, '-r');
