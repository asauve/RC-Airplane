function [drag_grams]=Alex_drag_calc(simplex_row,flag)
%reads a simplex row, creates a wing object and and returns the drag value for level flight
%flag = 0 for general simulation
%flag = 1 for best wing re-simulation (simulates at a wide range of speeds)
%flag = 2 for best wing final simulation (does not rewrite twist vector so that constant twist is possible)
global payload_mass
global dihedral_deg

if nargin==1
    flag=0;
end

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% CONVERT SIMPLEX INTO WING OBJECT
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
wing=Alex_wing; %creates a wing object
wing.span       =simplex_row(1);
wing.root_chord =simplex_row(2);
wing.taper      =simplex_row(3);
wing.sweep      =simplex_row(4);
wing.speed_km   =simplex_row(5);

if flag==1 %used for ploting the best wing drag vs speed
    wing.speed_km=[15:2.5:35];
end


%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% CONSTANTS
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
wing.dihedral = dihedral_deg; %degrees
wing.speed = wing.speed_km*0.277778; %m/s
wing.tip_chord=wing.root_chord*wing.taper;%m

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% WING MASS ESTIMATION
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
foam_volume_m3 =  0.05892*wing.root_chord^2*wing.span*(wing.taper^2-wing.taper+1)*(1/3); %m^3. assumes HS 522 root, MH60 tip
foam_volume_in3 = foam_volume_m3*61023.7; %convert from m^3 to in^3
foam_density = 0.2763; %g/in^3, experimental
foam_mass=foam_volume_in3*foam_density; %grams
wing.mass=foam_mass+payload_mass; %g
wing.weight= wing.mass*9.81/1000; %total weight, N

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% STABILITY CALCULATIONS
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%finding required twist & cg location
% equations from TWIST3 excel sheet made by Dr Walter Panknin
rho_stability=1.2238; %kg/m^3. value used inside tornado =1.2238
stability_factor=0.075;
Cm_c4_root=0.0157; %www.aerodesign.de/english/profile/profile_s.htm#hs522
alpha_L0_root=-0.09; %deg, HS 130 airfoil
Cm_c4_tip=0.0298;
alpha_L0_tip=0.3; %deg, JWL-065 airfoil
tip=wing.root_chord*wing.taper;
area=tip*wing.span+0.5*(wing.root_chord-tip)*wing.span;
root_in=wing.root_chord*39.3701; %converting meters->inches
tip_in=tip*39.3701;
span_in=wing.span*39.3701;
area_in=area*(39.3701)^2;
MGC_in=(tip_in+root_in)/2; %mean geometric chord
AR=span_in^2/area_in; %aspect ratio
wing.Cl_required=zeros(size(wing.speed));
for j=1:length(wing.speed)
    wing.Cl_required(j)=wing.weight./(0.5*area*rho_stability*wing.speed(j).*wing.speed(j));
    if(flag~=2)
        wing.twist(j)=...
            ((0.25*(3+2*wing.taper+wing.taper^2)/(1+wing.taper+wing.taper^2)*Cm_c4_root...
            +(1-0.25*(3+2*wing.taper+wing.taper^2)/(1+wing.taper+wing.taper^2))*Cm_c4_tip...
            -wing.Cl_required(j)*stability_factor)/(0.000014*...
            ((AR)^1.43)*wing.sweep))...
            -(alpha_L0_root-alpha_L0_tip); %degrees
    end
    B24=root_in;
    B25=tip_in;
    B26=span_in/2;
    B21=B24*0.25;
    B22=B25*0.25;
    B23=tan(wing.sweep*pi/180)*B26;
    B20=(B21-B22)+B23;
    B12=stability_factor;
    F8=MGC_in;
    F11=(B24^2+(B24*B25)+B25^2)/(6*(B24+B25))+(((2*B25)+B24)*B20)/(3*(B24+B25));
    wing.cg_inches(j)=F11-F8*B12;
end


%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% USE TORNADO ENGINE (INVISCID  VORTEX PANEL METHOD)
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
Alex_modified_tornado(wing); %finds the drag, adds to wing object
drag_grams=wing.drag_grams;

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% SPECIAL CASES : look inside Alex_modified tornado
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% for example, if root chord<0.25m, drag=99
% allows you to force desired characteristics, regardless of drag

% mathematically a negative root will not require any lift and produces negative drag.
% force the root chord to be positive

%force span to be no longer than 1.3m for strength / ease of use
end