filepath_05 = '../experiments/delay_gauss_025_2019-02-22_08-37/results/length.txt'; 
filepath_05_g = '../experiments/delay_gauss_025_2019-02-22_08-37/results/length.txt'; 

data_05=dlmread(filepath_05, ';');

data_05_g=dlmread(filepath_05_g, ';');


%Data is organized as following: 
%data{:,1} V-angle; 
%data{:,2} V-height;
%data{:,3} delay between robots;
%data{:,4} distance between robots; 
%data{:,5} phase shift between robots;
%data{:,6} stability; 
%data{:,7} bridge length;
%data{:,8} V-bottom y-coord; 
%data{:,9} bridge right height; 
%data{:,10} bridge left height; 
%data{:,11} bridge mean height; 
%data{:,12} dissolution;
%data{:,13} Nb robots end of dissolution;
%(data{:,14} initial distance from the bottom of the V;) (opt)

%data=data_00;
% [folder,name,ext] = fileparts(filepath_00);
% data=[data_00; data_025; data_05; data_075];
folder='../experiments/global_results/'

delay = data_05(:,3);
delay = unique(delay,'rows');
angle = min(data_05(:,1)):2.5:max(data_05(:,1));

% ---------------------
fig1 = figure(1);
x0=500;
y0=500;
width=750;
height=400;

cmap = colormap(parula(12)); %30
h_2bl = 1./tan(angle.*pi/180);
h_bl = 0.5./tan(angle.*pi/180);
hold on
no_diss = [];
a_diss = [];

d = delay(5);
txt = "Fixed delay";
data_temp = data_05(data_05(:,3)== d, :);
data_temp=sortrows(data_temp,1);
plot(data_temp(:,1),abs(data_temp(:,11)), '-o', 'Color',cmap(2,:), 'DisplayName', txt);
no_diss = [no_diss; abs(data_temp(data_temp(:,12)== 0, 11))];
a_diss = [a_diss; data_temp(data_temp(:,12)== 0, 1)];

txt = "Gaussian delay";
data_temp = data_05_g(data_05_g(:,3)== d, :);
data_temp=sortrows(data_temp,1);
plot(data_temp(:,1),abs(data_temp(:,11)), '-o', 'Color',cmap(6,:), 'DisplayName', txt);
no_diss = [no_diss; abs(data_temp(data_temp(:,12)== 0, 11))];
a_diss = [a_diss; data_temp(data_temp(:,12)== 0, 1)];

plot(angle,h_2bl,'--', 'Color','k', 'DisplayName','2 BL width');
plot(angle,h_bl,'--', 'Color','k', 'DisplayName','1 BL width');
plot(a_diss,no_diss,'x', 'Color','k', 'DisplayName','Dissolution not complete');
legend show
ylabel('Mean bridge height [Body length unit]')
xlabel('V half-angle [Deg] ')

set(gcf,'position',[x0,y0,width,height])

title = fullfile(folder, 'f_angle_gauss_comp_d65.png');
saveas(fig1,title);
title = fullfile(folder, 'f_angle_gauss_comp_d65.fig');
saveas(fig1,title);