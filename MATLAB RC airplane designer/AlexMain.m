%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% PROGRAM DESCRIPTION
% A program to aid the design of a flying wing RC airplane.
% the program reports the most efficient design for a variety of wind
% speeds.
%
% this program finds the most efficient wing, given a known payload weight.
% Wing span, root chord, taper ratio, and sweep angle 
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
%
% drag = function (sweep angle, wing span, root chord, taper ratio, speed)
% is minimized using the nelder-mead method
% Alexandre Sauve, 2016
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

clear
clc
clf
close all

global last_wing

%constants
global payload_mass
	payload_mass=234; %grams
global dihedral_deg
    dihedral_deg=5; %degrees
global figureWindow
    figureWindow=5;

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% begin nelder-mead search 
% we need to create a simplex for a solution in 5 variables (see below)
% must propose 6 wings as a starting point
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

wing=Alex_wing; %using object oriented programming to organize variables nicely

wing(1).span = 1; %m
wing(1).root_chord = 0.25; %m
wing(1).taper = 0.6;
wing(1).sweep = 25; %degrees
wing(1).speed_km=35; %km/hr

wing(2).span = 1; %m
wing(2).root_chord = 0.27; %m
wing(2).taper = 0.4;
wing(2).sweep = 15; %degrees
wing(2).speed_km=25; %km/hr

wing(3).span = 1.2; %m
wing(3).root_chord = 0.32; %m
wing(3).taper = 0.5;
wing(3).sweep = 20; %degrees
wing(3).speed_km=40; %km/hr

wing(4).span = 1; %m
wing(4).root_chord = 0.3; %m
wing(4).taper = 0.5;
wing(4).sweep = 30; %degrees
wing(4).speed_km=40; %km/hr


wing(5).span = 1.25; %m
wing(5).root_chord = 0.3; %m
wing(5).taper = 0.4;
wing(5).sweep = 25; %degrees
wing(5).speed_km=30; %km/hr

wing(6).span = 0.75; %m
wing(6).root_chord = 0.4; %m
wing(6).taper = 0.3;
wing(6).sweep = 25; %degrees
wing(6).speed_km=35; %km/hr

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% create simplex. i:rows, j:columns
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
start_simplex=zeros(6,5);
for i=1:6
start_simplex(i,1)=wing(i).span;
start_simplex(i,2)=wing(i).root_chord;
start_simplex(i,3)=wing(i).taper;
start_simplex(i,4)=wing(i).sweep;
start_simplex(i,5)=wing(i).speed_km;
end

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% RUN NELDER-MEAD THINGY
% replaces the highest drag wing of the simplex, until the 6 wings have converged properties
% ie. we have foud the lowest drag wing
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
min_iterations=15;
max_iterations=100;
error_tolerance=0.1;
[best_wing_simplex,lowest_drag,simplex_volume,simplex_uncertainty,simplex_log,drag_log]...
    = nelder('Alex_drag_calc',start_simplex,min_iterations,max_iterations,error_tolerance,1);

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% RESULTS
% LOOK INTO Alex_drag_calc for assumptions, drag calc procedure
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

Alex_drag_calc(best_wing_simplex,1);
best_wing=last_wing;
best_wing.mass=200;
[lowest_drag,lowest_drag_index]=min(best_wing.drag);
% force the wing object to use the same twist throughout drag calculations
% as twist cannot change during flight
for i=1:length(best_wing.twist)
    best_wing.twist(i)=best_wing.twist(lowest_drag_index);
end
Alex_drag_calc(best_wing_simplex,2) %redo drag calcs with constant twist
best_wing.show;
best_wing.save('results.txt');


