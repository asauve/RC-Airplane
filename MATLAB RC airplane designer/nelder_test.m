function [result]=nelder_test(V)
%bowl shaped thing, the minimum value (=2) should be found at V(1)=V(2)=v(3)=0
result = V(1)^2 +V(2)^2 +V(3)^2 +2;
end