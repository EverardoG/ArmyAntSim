clear all;

filepath(1) = "../experiments/delay_gauss_025_2019-02-20_22-17/results/length.txt"; 
filepath(2) = '../experiments/delay_gauss_025_2019-02-21_08-45/results/length.txt'; 
filepath(3) = '../experiments/delay_gauss_025_2019-02-23_20-22/results/length.txt'; 
filepath(4) = '../experiments/delay_gauss_025_2019-02-21_14-40/results/length.txt'; 
filepath(5) = '../experiments/delay_gauss_025_2019-02-21_16-33/results/length.txt'; 
filepath(6) = '../experiments/delay_gauss_025_2019-02-22_08-37/results/length.txt'; 
filepath(7) = '../experiments/delay_gauss_025_2019-02-22_10-46/results/length.txt'; 
filepath(8) = '../experiments/delay_gauss_025_2019-02-22_12-56/results/length.txt'; 
filepath(9) = '../experiments/delay_gauss_025_2019-02-22_20-56/results/length.txt'; 
filepath(10) = '../experiments/delay_gauss_025_2019-02-22_14-56/results/length.txt'; 

for i=1:length(filepath)
    data(:,:,i)=unique(dlmread(filepath(i), ';'),'rows');
end

folder='../experiments/global_results/gaussian/result_10_exp/'

%Data is organized as following: 
%data{:,1} V-angle; 
%data{:,2} V-height;
%data{:,3} delay between robots;
%data{:,4} distance between robots; 
%data{:,5} phase shift between robots;
%data{:,6} bridge exist; 
%data{:,7} 1st step duration (used to check if stacking);
%data{:,8} stability; 
%data{:,9} Nb robots end of formation; 
%data{:,10} dissolution;
%data{:,11} Nb robots end of dissolution;

step_d=0.25;
step_a=2;
angle = min(data(:,1,1)):2.5:max(data(:,1,1));
% angle = [angle 52.5];
% delay = min(data(:,3)):1:max(data(:,3));
delay = data(:,3,1);
delay = unique(delay,'rows');
% delay = [delay; 8.5];
dist = (floor(delay)+0.5).*0.52./1.02;
dist = round(dist,3);

x0=500;
y0=500;
width=750;
height=400;

h_2bl = 1./tan(angle.*pi/180);
h_bl = 0.5./tan(angle.*pi/180);

for j=1:length(delay)
    fig(j)=figure(j);
    for i=1:length(filepath)
       tmp = data(data(:,3,i)==delay(j),11,i);
       h(i,:,j) = abs(tmp');
    end
    hold on
    plot(angle,h_2bl,'--', 'Color','k', 'DisplayName','2 BL width');
    plot(angle,h_bl,'--', 'Color','k', 'DisplayName','1 BL width');
    plot(angle,median(h(:,:,j)),'-', 'Color','r', 'DisplayName','Median');
    boxplot(h(:,:,j),angle, 'positions', angle)
    legend show
    ylabel('Mean bridge height [Body length unit]')
    xlabel('V-Angle/2 [Deg] ')
    ylim([0 8])
    
    txt_title = strcat("gaussian_height_boxplot_", num2str(dist(j),2), ".png");
    title = fullfile(folder, txt_title);
    saveas(fig(j),title);
    txt_title = strcat("gaussian_height_boxplot_", num2str(dist(j),2), ".fig");
    title = fullfile(folder, txt_title);
    saveas(fig(j),title);

end

%=========================
%           DISSOLUTION
%=========================

clear all;

filepath(1) = "../experiments/delay_gauss_025_2019-02-20_22-17/results/nb_robots.txt"; 
filepath(2) = '../experiments/delay_gauss_025_2019-02-21_08-45/results/nb_robots.txt'; 
filepath(3) = '../experiments/delay_gauss_025_2019-02-23_20-22/results/nb_robots.txt'; 
filepath(4) = '../experiments/delay_gauss_025_2019-02-21_14-40/results/nb_robots.txt'; 
filepath(5) = '../experiments/delay_gauss_025_2019-02-21_16-33/results/nb_robots.txt'; 
filepath(6) = '../experiments/delay_gauss_025_2019-02-22_08-37/results/nb_robots.txt'; 
filepath(7) = '../experiments/delay_gauss_025_2019-02-22_10-46/results/nb_robots.txt'; 
filepath(8) = '../experiments/delay_gauss_025_2019-02-22_12-56/results/nb_robots.txt'; 
filepath(9) = '../experiments/delay_gauss_025_2019-02-22_20-56/results/nb_robots.txt'; 
filepath(10) = '../experiments/delay_gauss_025_2019-02-22_14-56/results/nb_robots.txt'; 

for i=1:length(filepath)
    data(:,:,i)=unique(dlmread(filepath(i), ';'),'rows');
end

folder='../experiments/global_results/gaussian/result_10_exp/'

%Data is organized as following: 
%data{:,1} V-angle; 
%data{:,2} V-height;
%data{:,3} delay between robots;
%data{:,4} distance between robots; 
%data{:,5} phase shift between robots;
%data{:,6} bridge exist; 
%data{:,7} 1st step duration (used to check if stacking);
%data{:,8} stability; 
%data{:,9} Nb robots end of formation; 
%data{:,10} dissolution;
%data{:,11} Nb robots end of dissolution;

angle = min(data(:,1,1)):2.5:max(data(:,1,1));
% angle = [angle 52.5];
% delay = min(data(:,3)):1:max(data(:,3));
delay = data(:,3,1);
delay = unique(delay,'rows');
% delay = [delay; 8.5];
dist = (floor(delay)+0.5).*0.52./1.02;
dist = round(dist,3);

x0=500;
y0=500;
width=750;
height=400;

for j=1:length(delay)
    fig(j)=figure(j+6);
    for i=1:length(filepath)
       nb_diss = data(data(:,3,i)==delay(j),11,i); 
       nb_form = data(data(:,3,i)==delay(j),9,i); 
       tmp = 100 - nb_diss./nb_form*100
       h(i,:,j) = abs(tmp');
    end
    hold on
    plot(angle,median(h(:,:,j)),'-', 'Color','r', 'DisplayName','Median');
    boxplot(h(:,:,j),angle, 'positions', angle)
    ylim([0 100])
    legend show
    ylabel('Mean bridge height [Body length unit]')
    xlabel('V-Angle/2 [Deg] ')
    
    txt_title = strcat("gaussian_diss_boxplot_", num2str(dist(j),2), ".png");
    title = fullfile(folder, txt_title);
    saveas(fig(j),title);
    txt_title = strcat("gaussian_diss_boxplot_", num2str(dist(j),2), ".fig");
    title = fullfile(folder, txt_title);
    saveas(fig(j),title);

end



