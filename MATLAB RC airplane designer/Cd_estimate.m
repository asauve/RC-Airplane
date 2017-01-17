function [Cd]=Cd_estimate(alpha_deg)
%using re=100,000 mach=0 data from xfoil.
%uses hs_522 airfoil

xfoil_alpha=[-20 -18 -16 -14 -12 -10 -8 -6 -4 -2 0 2 4 6 8 14 16 18 20 22 24 26];
xfoil_cd=[0.26607 0.23295 0.21205 0.17799 0.15683 0.12668 0.09276 0.06194 0.0334 0.025 0.025 0.025 0.025 0.025 0.025 ...
0.15618 0.206 0.2294 0.26486 0.31911 0.34597 0.35279];

Cd=zeros(size(alpha_deg));

for i=1:length(alpha_deg)
    if (alpha_deg(i)>=min(xfoil_alpha) && alpha_deg(i)<=max(xfoil_alpha))
        %within bounds for interpolating data
        Cd(i)=spline(xfoil_alpha,xfoil_cd,alpha_deg(i));
    else
        if alpha_deg(i)<=min(xfoil_alpha)
            Cd(i)=xfoil_cd(1);
        end
        if alpha_deg(i)>=max(xfoil_alpha)
            Cd(i)=xfoil_cd(end);
        end
    end
end


