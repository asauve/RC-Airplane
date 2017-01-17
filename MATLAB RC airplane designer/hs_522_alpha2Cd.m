function [Cd]=Cd_estimate(alpha_deg)
%using re=100,000 mach=0 data from xfoil.
%uses hs_522 airfoil

xfoil_alpha=[-20 -18 -16 -14 -12 -10 -8 -6 -4 -2 0 2 4 6 8 14 16 18 20 22 24 26];
xfoil_cd=[0.26607 0.23295 0.21205 0.17799 0.15683 0.12668 0.09276 0.06194 0.0334 0.01493 0.01277 0.01417 0.01564 0.01799 0.02395 ...
0.15618 0.206 0.2294 0.26486 0.31911 0.34597 0.35279];

if (alpha>min(xfoil_alpha) && alpha<max(xfoil_alpha) 
    %within bounds for interpolating data
Cd=spline(xfoil_alpha,xfoil_cd,alpha_deg);
else
    if alpha<min(xfoil_alpha)
        Cd=xfoil_cd(1);
    end
    if alpha>max(xfoil_alpha)
        Cd=xfoil_cd(end);
    end 
end



