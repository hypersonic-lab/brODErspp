close all
clear
clc

% Load stagline data
dd = load('stagline_result.dat');

xx  = dd(end:-1:1,1);
uu  = -dd(end:-1:1,2);
TT  = dd(end:-1:1,4);
rho = dd(end:-1:1,6);

Xi = dd(end:-1:1,9:13);

% RECAST xx IN LARSEN FORM
xx = xx(1) - xx;

% Reformat the output
dd_L = [xx, TT, rho, uu, Xi];

% Write file
fid = fopen('stagline_baseline_sol.dat', 'w');

for ii = 1:size(dd_L,1)
  for jj = 1:size(dd_L,2)
    fprintf(fid, '%e  ', dd_L(ii,jj));
  end
  fprintf(fid, '\n');
end

fclose(fid);
