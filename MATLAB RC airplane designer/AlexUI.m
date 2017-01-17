function []=AlexUI(wing,l,speed_km_range,lowest_drag_grams,best,solution_length)
clf
close all
l=0;

l=l+1; %new window
figure(l)
title('Lowest drag vs air speed');
legend_string=[];
for j=1:length(speed_km_range)
    hold on
    if lowest_drag_grams(j)==min(lowest_drag_grams)
        plot(speed_km_range(j),lowest_drag_grams(j),'x');
        overall_best=best(j);
    else
        plot(speed_km_range(j),lowest_drag_grams(j),'o');
    end
    xlabel('speed [km/hr]')
    ylabel('drag [grams]')
    axis([min(speed_km_range) max(speed_km_range) 0 1]);
    axis vis3d;
    hold on
    legend_string{end+1}=sprintf('#%2u for %4.1f kph',best(j),speed_km_range(j));
end
Lhandle=legend(legend_string);
title(Lhandle, 'Best wings')


while(1)
    disp('[1] clear plots')
    disp('[2] plot drag vs air speed (shows optimal indices)')
    disp('[3] view the wing with overall lowest drag')
    disp('[''#'' + wing index] to view a specific wing')
    disp('[Q or q] to exit')
    answer=input('select an option: ','s');
    fprintf('______________________________________________________________\n\n');
    
    switch(answer)
        case '1'
            %user wants to clear the figures
            clf
            close all
            l=0;
            
        case '2'
            %user wants to re-plot the lowest drag vs air speed graph
            l=l+1; %new window
            figure(l)
            title('Lowest drag vs air speed');
            legend_string=[];
            for j=1:length(speed_km_range)
                hold on
                if lowest_drag_grams(j)==min(lowest_drag_grams)
                    plot(speed_km_range(j),lowest_drag_grams(j),'x');
                else
                    plot(speed_km_range(j),lowest_drag_grams(j),'o');
                end
                xlabel('speed [km/hr]')
                ylabel('drag [grams]')
                axis([min(speed_km_range) max(speed_km_range) 0 1]);
                axis vis3d;
                %wing.view(best);
                %best_wing=wing(best);
                %save best_wing best_wing %save best_wing as best_wing.mat
                hold on
                legend_string{end+1}=sprintf('#%2u for %4.1f kph',best(j),speed_km_range(j));
            end
            Lhandle=legend(legend_string);
            title(Lhandle, 'Best wings')
            
        case '3'
            %user wants to view the overall best wing
            l=l+1; %new window
            wing(overall_best).show;
            
        otherwise
            %user wants to quit
            if(answer(1)=='Q' || answer(1)=='q')
                break;
            end
            if answer(1)=='#'
                %user wants to view a specific wing
                try
                    for k=2:length(answer)
                        cut_answer(k-1)=answer(k);
                    end
                    wing(str2double(cut_answer)).show;
                catch
                    %user did not use good syntax
                    disp('ERROR. for example, type "#5" to view wing #5.')
                    fprintf('select a wing number between 1 and %u.\n',solution_length);
                end
            end
    end %end of switch(answer)
end %end of while(loop==1)
end %end of function