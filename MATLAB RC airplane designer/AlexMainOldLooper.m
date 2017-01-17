%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% PROGRAM DESCRIPTION
% A program to aid the design of a flying wing RC airplane.
% the program reports the most efficient design for a variety of wind
% speeds.
%
% this program finds the most efficient wing, given a known payload weight.
% Wing span, root chord, taper ratio, sweep angle and geometric twist
% are varied so that the lowest drag design may be found.
% program assumes a flying wing geometry, and uses geometric twist in order
% to ensure stability. the minumum geo twist required is calculated
% as per Dr, Walter Panknin's equations, http://www.b2streamlines.com/Panknin.html
% wing loading is determined using the vortex lattice method, the numerical solution
% is carried out via the "tornado" matlab code.
% the required lift coefficient to maintain level flight is found for
% each air speed, the angle of attack necessary to reach this lift
% coefficient is found by tornado, and the lift distribution and induced
% drag are calculated.
% Alexandre Sauve, 2016
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

clear
clc
clf
close all

tip_min=0.10; %rule out designs where the tip is too small
payload_mass=234; %grams. everything except the wing
% ie motor, servos, electronics, battery

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% LOOPING RANGES & INDICIES
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
i=0; %wing index
%j speed index - must simulate each wing at a number of speeds
k=0; %subplot index
l=5; %figure window index
span_range= [1:0.25:1.25]; %m, guitar wire can cut up to 36 inch wings - 72 inch wingspan max.
root_range= [0.2:0.05:0.3]; %m
taper_range=[0.3:0.1:0.8];
sweep_range =[27.5:2.5:30]; %degress, quarter chord sweep angle
extra_twist_range=[0]; %degrees, investigates additional geometric twist
speed_km_range=[20:2.5:35]; % variety of airspeeds to investigate

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%]]]]]88%
% INITIALIZE VARIABLES
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
lowest_drag_grams=zeros(size(speed_km_range))+1000;
best=zeros(size(speed_km_range));
solution_length=length(span_range)*length(root_range)*length(taper_range)*length(sweep_range)*length(extra_twist_range);
wing(solution_length)=Alex_wing;

fprintf('PROGRAM START \nAnalyzing %u wings\n\n',solution_length);
for span=span_range
    for root=root_range
        for taper=taper_range
            for sweep=sweep_range
                for extra_twist=extra_twist_range
                    i=i+1; %wing index
                    wing(i)=Alex_wing; %using object oriented programming to organize variables nicely
                    %see Alex_wing class definition
                    
                    wing(i).speed_km = speed_km_range; %km/hr
                    wing(i).speed = wing(i).speed_km*0.277778; %m/s
                    wing(i).span=span; %m
                    wing(i).root_chord=root; %m
                    wing(i).taper=taper;
                    wing(i).dihedral=0; %degrees
                    wing(i).sweep=sweep; %degrees
                    wing(i).index=i;
                    wing(i).tip_chord=root*taper;%m
                    
                     if (wing(i).tip_chord<tip_min) %rule out really small tip - too hard to manufacture
                         fprintf('Wing %u/%u, tip chord length is below specified minimum\n',i,solution_length)
                         break;
                     end
                     
                    
                    %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
                    % WING MASS ESTIMATION
                    %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
                    foam_volume_m3 =  0.05892*wing(i).root_chord^2*wing(i).span*(wing(i).taper^2-wing(i).taper+1)*(1/3); %m^3. assumes HS 522 root, MH60 tip
                    foam_volume_in3 = foam_volume_m3*61023.7; %convert from m^3 to in^3
                    foam_density = 0.2763; %g/in^3, experimental
                    foam_mass=foam_volume_in3*foam_density; %grams
                    wing(i).mass=foam_mass+payload_mass; %g
                    wing(i).weight= wing(i).mass*9.81/1000; %total weight, N
                    
                    %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
                    % STABILITY CALCULATIONS %
                    %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
                    %finding required twist & cg location
                    % equations from TWIST3 excel sheet made by Dr Walter Panknin
                    rho_stability=1.2238; %kg/m^3. value used inside tornado =1.2238
                    stability_factor=0.075;
                    Cm_c4_root=-0.005; %www.aerodesign.de/english/profile/profile_s.htm#hs522
                    alpha_L0_root=-0.84; %deg, HS 522 airfoil
                    Cm_c4_tip=-0.003;
                    alpha_L0_tip=-0.31; %deg, MH 60 airfoil
                    tip=root*taper;
                    area=tip*span+0.5*(root-tip)*span;
                    root_in=root*39.3701; %converting meters->inches
                    tip_in=tip*39.3701;
                    span_in=span*39.3701;
                    area_in=area*(39.3701)^2;
                    MGC_in=(tip_in+root_in)/2; %mean geometric chord
                    AR=span_in^2/area_in; %aspect ratio
                    wing(i).Cl_required=zeros(size(wing(i).speed));
                    wing(i).Cl_required=wing(i).weight./(0.5*area*rho_stability*wing(i).speed.*wing(i).speed);
                    wing(i).twist_required=...
                        ((0.25*(3+2*taper+taper^2)/(1+taper+taper^2)*Cm_c4_root...
                        +(1-0.25*(3+2*taper+taper^2)/(1+taper+taper^2))*Cm_c4_tip...
                        -wing(i).Cl_required*stability_factor)/(0.000014*...
                        ((AR)^1.43)*wing(i).sweep))...
                        -(alpha_L0_root-alpha_L0_tip); %degrees
                    wing(i).twist=wing(i).twist_required-extra_twist;
                    B24=root_in;
                    B25=tip_in;
                    B26=span_in/2;
                    B21=B24*0.25;
                    B22=B25*0.25;
                    B23=tan(wing(i).sweep*pi/180)*B26;
                    B20=(B21-B22)+B23;
                    B12=stability_factor;
                    F8=MGC_in;
                    F11=(B24^2+(B24*B25)+B25^2)/(6*(B24+B25))+(((2*B25)+B24)*B20)/(3*(B24+B25));
                    wing(i).cg_inches=F11-F8*B12;
                    
                    
                    %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
                    % USE TORNADO ENGINE
                    %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
                    Alex_modified_tornado(wing(i));
                    
                    %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
                    % PLOTTING & STORAGE OF BEST RESULTS
                    %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
                    fprintf('Wing %u/%u finished!\n\n',wing(i).index,solution_length)
                    
                    k=k+1; %subplot index
                    if(k==17)
                        %current figure is full!
                        k=1; %reset
                        l=l+1; %go to next figure window
                    end
                    
                    try
                        figure(l)
                        subplot(4,4,k) %plot the mini drag vs speed plot
                        wing(i).plot;
                    catch
                        fprintf('could not plot wing %u/%u!\n\n',wing(i).index,solution_length);
                    end
                    
                    %update the "best" matrix, if appropriate
                    for j=1:length(speed_km_range)
                        if wing(i).drag(j)<lowest_drag_grams(j)                     
                            lowest_drag_grams(j)=wing(i).drag(j);
                            best(j)=i;
                        end
                    end
                    
                end %extra twist loop
            end %sweep loop
        end %taper loop
    end %root loop
end %span loop

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% USER INTERFACE : SHOW RESULTS
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%one optimized wing will be presented for every speed
AlexUI(wing,l,speed_km_range,lowest_drag_grams,best,solution_length);
