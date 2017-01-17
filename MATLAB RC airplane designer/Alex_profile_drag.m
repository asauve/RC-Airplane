function [visc_drag]=Alex_profile_drag(wing,dyn_pressure,j,alpha_rad)
b=wing.span;
delta=wing.root_chord-wing.tip_chord;
profile_drag=[0 0 0 0 0];
alpha_deg=zeros(1,5);
drag_co=zeros(1,5);
chord_avg=zeros(1,5);
area=zeros(1,5);

%break wing into 10 subsections
%analyze a half-span (5 subsections)
%each subsection has a unique angle, area, and overall drag
for i=1:5
    %find subsection angle, correlate to a drag coefficient
    alpha_deg(i)=wing.twist(j)*(1+2*(i-1))/10 + alpha_rad*180/pi;
    drag_co(i)=Cd_estimate(alpha_deg(i)); 
    
    %find subsection area
    chord_avg(i)=wing.root_chord - delta*(1+2*(i-1))/10;
    area(i)=chord_avg(i)*(b/10);
    
    %now drag
    profile_drag(i)=dyn_pressure*drag_co(i)*area(i);   
end
visc_drag=2*sum(profile_drag);
end