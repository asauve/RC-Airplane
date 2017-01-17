classdef Alex_wing < handle %using a handle allows pass-by-reference without declaring global objects
    properties (SetAccess = public)
        index
        span %m
        root_chord %m
        tip_chord %m
        taper %taper ratio = root chord / tip chord
        dihedral %deg
        sweep %deg
        mass %gram
        weight %N
        area %m^2
        mean_aero_chord %m
        cg_inches %inches
        
        %tornado generated files, needed for plots
        geo
        lattice
        ref
        
        %matrixes
        speed_km %km/h
        speed %m/s
        twist_required %deg
        twist %deg
        alpha_rad %radians
        alpha_deg %deg
        Cl_required
        drag_i %lift induced drag, found using tornado
        drag_profile %viscous body & pressure drag. found using xfoil & alpha correlation
        drag %total drag, N
        drag_grams %total drag in grams (useful for comparing with thrust data)
    end
    methods
        function [obj]=show(obj)
            global figureWindow
            
            figure(figureWindow);
            clf
            figureWindow=figureWindow+1;
            geometryplot(obj.lattice,obj.geo,obj.ref); %plot geometry
            
            
            figure(figureWindow);
            clf
            figureWindow=figureWindow+1;
            clf
            plot(obj.speed_km,obj.drag_grams);
            hold on
            drag_i_grams=obj.drag_i.*101.971621298;
            plot(obj.speed_km,drag_i_grams,'g--');
            hold on
            drag_profile_grams=obj.drag_profile.*101.971621298;
            plot(obj.speed_km,drag_profile_grams,'r-.');
            name=strcat('Wing, (#',num2str(obj.index),') drag vs speed');
            title(name)
            xlabel('speed [km/hr]')
            ylabel('drag [grams]')
            axis([min(obj.speed_km) max(obj.speed_km) 0 max(obj.drag_grams)]);
            legend('total drag', 'lift-induced drag', 'profile drag')
            axis vis3d
        end
        
        function [obj]=plot(obj)
            plot(obj.speed_km,obj.drag);
            title(obj.index);
            axis([min(obj.speed_km) max(obj.speed_km) 0 1]);
            axis vis3d;
            set(gca,'xtick',[min(obj.speed_km) max(obj.speed_km)]);
            drawnow
        end
        
        function [obj]=save(obj,name)
            fid = fopen( name, 'wt' );
            fprintf( fid, '%s \n',date);
            fprintf( fid, 'span: %f [m] \n',obj.span);
            fprintf( fid, 'root chord: %f [m] \n', obj.root_chord);
            fprintf( fid, 'tip chord: %f [m] \n', obj.tip_chord);
            fprintf( fid, 'taper ratio: %f \n', obj.taper);
            fprintf( fid, 'dihedral: %f degrees \n', obj.dihedral);
            fprintf( fid, 'sweep angle: %f degrees \n', obj.sweep);
            fprintf( fid, 'airplane mass: %f grams \n', obj.mass);
            fprintf( fid, 'airplane weight: %f [N] \n', obj.weight);
            fprintf( fid, 'planform area: %f [m^2] \n',obj.area);
            fprintf( fid, 'center of gravity should be %f inches back from the nose \n',obj.cg_inches(1));
            
            fprintf(fid, '\nSIMULATION RESULTS\n');
            fprintf(fid, 'speed [km/hr]   drag [N]    lift_drag [N]   drag_profile [N]   Cl_required   alpha_required [deg] \n');
            for i=1:length(obj.speed_km)
                fprintf(fid, '   %5.1f        %6.3f         %5.2f             %5.2f             %6.3f        %6.3f \n',obj.speed_km(i),obj.drag(i),obj.drag_i(i),obj.drag_profile(i), obj.Cl_required(i),obj.alpha_deg(i));
            end
            fclose(fid);
        end
        
    end
end