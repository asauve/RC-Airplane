function []=Alex_modified_tornado(wing)
%emulates the result of a text based Q&A session with tornado.
%this function allows the tornado engine to be used by other pieces of
%code, such that no user intervention is required
%finds lift induced drag for an arbitrary flying wing
global last_wing

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% INITIALIZE VARIABLES
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
wing.drag_i=zeros(size(wing.speed));
wing.drag_profile=zeros(size(wing.speed));
wing.drag=zeros(size(wing.speed));
wing.alpha_rad=zeros(size(wing.speed));

settings=config('startup'); %setting directories

geo.fnx=0;				    %number of panels on flap chords (2d array)
geo.ny=1;					%number of panels in span (2d array)
geo.nx=1;					%number of panels on chord (2d array)
geo.fsym=0;				    %flap deflection symmetry boolean bit  (2d array)
geo.fc=0;					%flap chord in percent of wingchord (2d array)
geo.flapped=[0];			    %flapped partition(wing part) boolean bit (2d array)
geo.TW(:,:,1)=0;	    	%partition twist (3d array)<1 inboard, 2 outboard>
geo.TW(:,:,2)=0;
geo.foil(:,:,1)={'0'};		    %Partition airfoils (3d array)
geo.foil(:,:,2)={'0'};		    %1 inboard, 2 outboard
geo.T=1;					%Taper ratio (2d array)
geo.SW=0;					%Sweep (2d array)
geo.c=1;					%Root chord (2d array)
geo.dihed=0;				%Dihedral (2d array)
geo.b=1;					%span(distance root->tip chord) (2d array)
geo.symetric=1;			    %Wing symmetry boolean bit (2d array)
geo.startx=0;		    	%Partition starting coordinate (2d array)
geo.starty=0;		    	% ---"----
geo.startz=0;		    	% ---"----
geo.nwing=1;				%number of wings (scalar)
geo.nelem=1;				%number of partitions on each wing (1d array)
geo.flap_vector=0;          %Flap deflection vector
geo.ref_point=[0 0 0];      %System reference point
geo.CG=[0 0 0];             %System center of gravity (around which all rotations occur)

state.AS=0;					%airspeed
state.alpha=0;				%angle of attack
state.betha=0;				%angle of sideslip
state.P=0;					%roll angluar rate
state.Q=0;					%pitch angular rate
state.R=0;					%Yaw angular rate
state.alphadot=0;           %Angle of attack time derivative
state.bethadot=0;           %Angle of sidesliptime derivative
state.ALT=0;                %Altitude, meters.
state.rho=0;                %Air density, kg/m^3.
state.pgcorr=0;             %Prandtl-Glauert compressibillity correction.

lattice.XYZ=0;				%panel corner matrix (2d array)
lattice.COLLOC=0;           %collocation point matrix
lattice.VORTEX=0;           %Vortex sling cornerpoint position matrix
lattice.N=0;                %Airfoil collocation point normal direction matrix

ref.S_ref=0;                %reference area;
ref.C_mac=0;                %mean aerodynamic choord
ref.mac_pos=0;              %start position of mac
ref.C_mgc=0;                %mean geometric chord
ref.b_ref=0;                %reference span

results.dwcond=0;           %computation result memory structure.

loop=1;                     %program run bit

stat=0;                     %Status variable, 0 if geometry and lattice is inconsistent


%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% SPECIFY WING GEOMETRY
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
geo=[];
s=1; %wing number. usually, s=[1:geo.nwing]
s2=1; %semispanwise partition number. usually s2=[1:geo.lem(s)]
t=1; %partition number. usualy increases partition # & wing #
geo.flapped=0;
geo.nwing=1; %1 wing
geo.nelem(s)=1; %1 semispanwise partition
geo.CG=[0 0 0];%CG-xyz
geo.ref_point=[0 0 0];%ref-xyz
geo.symetric(s)=1; %wing is mirrored in xy plane
geo.TW(s,1,1)=0; %twist of root
geo.meshtype(s,t)=3;%mesh distribution type (Spanwise half-cosine, chordwise cosine)
geo.flapped(s,t)=0;%is partitions flapped [1 0]
geo.fc(s,t)=0; %no flap
geo.fnx(s,t)=0; %no flap
geo.fsym(s,t)=0; %no flap
geo.flap_vector=zeros(size(geo.flapped));
geo.startx(s)=0;
geo.starty(s)=0;
geo.startz(s)=0;
geo.foil(s,t,1)={'hs522.dat'};%setting root airfoil
geo.foil(s,t,2)={'mh60.dat'};%tip

for j=1:length(wing.speed)
    try
        
geo.c(s)=wing.root_chord; %root chord length
geo.nx(s,1)=6;%number of chordwise panels
geo.ny(s,t)=10;%number of semi-spanwise panels
geo.dihed(s,t)=wing.dihedral*(pi/180); %dihedral angle
geo.b(s,t)=0.5*wing.span;%span of partition
geo.T(s,t)=wing.taper;%taper ratio
geo.SW(s,t)=wing.sweep*(pi/180);%quarter chord sweep degs
geo.TW(s,t,2)=wing.twist(j)*(pi/180);%outboard twist of tip

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% SPECIFY FLYING CONDITIONS
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

        settings=config('startup');
        state.alpha=1*pi/180;%AOA
        state.betha=0*pi/180; %sideslip angle. guy spells beta wrong (betha?)
        state.P=0;% roll angular velocity
        state.Q=0;%pitch angular velocity
        state.R=0;%yaw angular velocity
        state.adot=0; %angle of attack rate of change
        state.bdot=0; %angle of sideslip ROC
        state.alphadot=0;
        state.bethadot=0;
        state.AS=wing.speed(j); %True airspeed [m/s]
        state.ALT=10;  %Altitude [m]
        state.rho=ISAtmosphere(state.ALT); %density @ altitude
        state.pgcorr=0; %Apply Prandtl-Glauert Correction (compressibility)
        
        
        %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        % GENERATE LATTICE
        %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        quest=0;
        %[0] = Freestream following wake, Tornado method
        %[1] = Fixed wake, standard VLM method
        [lattice,ref]=fLattice_setup2(geo,state,quest);
        wing.area=ref.S_ref;
        wing.mean_aero_chord=ref.C_mac;
        
        %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        % FIND SOLUTION FOR SPECIFIED Cl
        %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        CL_target=wing.weight/(0.5*wing.area*state.rho*state.AS^2);
        
        %find alpha for prescribed Cl
        resetstate=state;
        rudder_index=find(geo.flapped);
        flap_setting=geo.flap_vector(rudder_index);
        solvertype=1;  %Hardcoded fixed wake so far
        [results,alpha_rad]=fFindAlphaAtCL(geo,state,solvertype,CL_target);
        
        
        %re-generate the lattice at alpha
        state.alpha=alpha_rad;
        [lattice,ref]=fLattice_setup2(geo,state,quest);
        
        % run static solver
        rudder_index=find(geo.flapped);
        flap_setting=geo.flap_vector(rudder_index);
        [results]=solver9(results,state,geo,lattice,ref);
        [results]=coeff_create3(results,lattice,state,ref,geo);
        
        
        %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        % PROFILE DRAG ESTIMATION
        %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        wing.drag_i(j) = results.D; %induced is found by tornado
        %http://www.rcplanesguru.org/2015/08/some-basic-terms-related-to-aircraft_25.html
        
        %neglect downwash (effective angle of attack = freestream AOA)
        q=0.5*state.rho*state.AS^2; %dynamic pressure
        wing.drag_profile(j) = Alex_profile_drag(wing,q,j,alpha_rad);
        wing.drag(j) = wing.drag_i(j) + wing.drag_profile(j);        
        
        %conversion & storage
        wing.alpha_rad(j) = alpha_rad;
        wing.alpha_deg(j)=alpha_rad*180/pi;
        
        %%%%%%%%%%%%%%%%%%
        % SPECIAL CASES
        %%%%%%%%%%%%%%%%%%
        if wing.root_chord <0.25
            wing.drag(j)=99;
            %mathematically a negative root will not require any lift and produces negative drag.
            %force the root to be at least 25cm for strength
        end
        
        if wing.span>=1.3
            wing.drag(j)=99;
            %force span to be no longer than 1.3m for strength
            
        end
        
        if(wing.alpha_deg(j)>40)
            %stalling - rule out design
            wing.drag(j)=99;
        end
        
        wing.drag_grams(j)=wing.drag(j)*101.97;
        
        
        fprintf('   %5.1f        %6.3f      %5.2f      %5.2f    %5.2f     %6.2f         %5.2f             %5.2f             %6.3f        %6.3f        %6.3f \n',wing.speed_km(j),wing.drag(j),geo.b*2,geo.c,geo.T,geo.TW(1,1,2)*180/pi,wing.drag_i(j),wing.drag_profile(j),CL_target,results.CLwing,alpha_rad*180/pi);
    end
    
    if(wing.drag(j)==0) % tornado could not converge
        wing.drag(j)=1234;
    end
end
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% STORE GENERAL RESULTS
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
wing.geo=geo;
wing.lattice=lattice;
wing.ref=ref;

last_wing=wing;
end