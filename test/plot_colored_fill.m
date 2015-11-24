function [Q] = plot_colored_fill(X,Y1,Y2,C,varargin)

% -----------------------
%
% S. HSIANG
% 8.17.2012
% SHSIANG@PRINCETON.EDU
%
% -----------------------
%
% PLOT_COLORED_FILL(X,Y1,Y2,C) plots the filled space between two functions
% Y1 and Y2, both of which are dependant on X. The color of the
% fill determed by values in C.  If no color is specified, then the
% line is plotted greyscale, with the lowest C values set to [almost] white
% and the highest values set to [almost] black.
%
% PLOT_COLORED_FILL(X,Y1,Y2,C,V1) plots C values scaled between almost
% white (low values) and the color specified by the three element vector V1
% for high values.
%
% PLOT_COLORED_FILL(X,Y1,Y2,C,V1,V0) plots values scaled between V0 (low
% values) and V1 (high values). Both V1 and V0 are three element vectors
% specifying a color (with entries between zero and one).  They can be
% interchanged and the function will still plot, but the color scale will
% be reversed.
%
% Examples:
%
%       x = [1:.1:100]
%       y1 = sin(x)
%       y2 = sin(x)+1
%       c = cos(x)
%       figure
%       plot_colored_fill(x,y1,y2,y1)
%       figure
%       plot_colored_fill(x,y1,y2,c,[0 0 1], [1 0 0], 3)
%
%       r1 = randn(size(x))
%       r2 = randn(size(x))
%       figure
%       plot_colored_fill(x,r1,r2,c)
%
% See also vwregress, plot_colored_line



if length(X) ~= length(Y1)
    disp('SOL: X and Y must be same length')
    Q = false;
    return
elseif length(Y1) ~= length(Y2)
    disp('SOL: Y1 and Y2 must be same length')
    Q = false;
    return
elseif length(X) ~= length(C)
    disp('SOL: X and C must be same length')
    Q = false;
    return
end

max_color = max(C);
min_color = min(C);
color_range = (max_color - min_color);

if min_color == max_color
    C = .5*ones(size(C));
else
    C = (C - min_color)/color_range;        %rescale color range
%    C = 1-(C*(length(C)-1)+.5)/length(C);   %prevent color from hitting extreme values
    C = ones(size(C))-C;                    %because low numbers are dark colors

end



%determine if colors are assigned
S = size(varargin);
linewidthvar = 1; %line width
if S(2) == 1
    %if top color vector is specified
    colorlimit = varargin{1,1};
    %bottom color is white
    colorbottom = [1 1 1];
elseif S(2) == 2
    %if top color vector is specified
    colorlimit = varargin{1};
    %bottom color is specified
    colorbottom = varargin{2};
else
    colorlimit = [0 0 0]; %black for high values
    colorbottom = [1 1 1]; %whilte for low values
end



%plot each block with color as the average value at both end points

for i = 1:length(X)-1
    fill([X(i), X(i+1), X(i+1), X(i)],[Y1(i), Y1(i+1), Y2(i+1), Y2(i)], ...
        nanmean(C(i:i+1))*(colorbottom-colorlimit)+colorlimit,...
        'EdgeColor', nanmean(C(i:i+1))*(colorbottom-colorlimit)+colorlimit)

    if i == 1
        hold on
    end
end

Q = true;
