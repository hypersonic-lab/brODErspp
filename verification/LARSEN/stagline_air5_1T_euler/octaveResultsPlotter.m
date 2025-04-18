close all
clear
clc

% Load the solution
dd = load('stagline_result.dat');

xx = dd(end:-1:1,1);
uu = -dd(end:-1:1,2); % U is inverted in stagline
TT = dd(end:-1:1,4);
Xi = dd(end:-1:1,9:13);

% Load larsen result
dd_L = load('larsen_output.dat');

xx_L = dd_L(:,1);
Xi_L = dd_L(:,2:end-1);
TT_L = dd_L(:,end);

% Rescale LARSEN position to match Stagline 
xx_L = xx(1) - xx_L;

% PLOT
figure
hold on
plot(xx, TT, '+k', 'linewidth', 1)

plot(xx_L, TT_L, 'b', 'linewidth', 2)

title('SYMBOLS: Stagline - SOLID: LARSEN')
ylabel('velocity')
xlabel('x [m]')

print('Temp.png')

% ---
figure
hold on
semilogy(xx, Xi, '-+k', 'linewidth', 2)
semilogy(xx_L, Xi_L, 'b', 'linewidth', 2)
title('BLACK SYMBOLS: Stagline - BLUE: LARSEN')
ylabel('Mole fractions')
xlabel('x [m]')
ylim([1e-10, 1])

print('Xi.png')

% ----------

fprintf('=====>  Press RETURN to quit  <=====\n')
pause()
