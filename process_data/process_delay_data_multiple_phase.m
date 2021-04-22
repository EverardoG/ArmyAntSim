filepath_00 = '../experiments/delay_00_phase_angle_lim_30_2019-02-07_16-11/results/length.txt';%flipp_dist_from_bottom
filepath_05 = '../experiments/delay_05_phase_limit_angle_2019-02-04_16-47/results/length.txt'; 
filepath_025 = '../experiments/delay_025_phase_angle_lim_30_2019-02-07_10-52/results/length.txt'; 
filepath_075 = '../experiments/delay_075_phase_angle_lim_30_2019-02-07_13-34/results/length.txt'; 

data_00=dlmread(filepath_00, ';');
data_05=dlmread(filepath_05, ';');
data_025=dlmread(filepath_025, ';');
data_075=dlmread(filepath_075, ';');

%data=data_00;
% [folder,name,ext] = fileparts(filepath_00);
folder='../experiments/global_results/'
data=[data_00; data_025; data_05; data_075];

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

%folder='../experiments/'
data = unique(data,'rows');

angle = min(data(:,1)):2.5:max(data(:,1));
% delay = min(data(:,3)):1:max(data(:,3));
delay = data(:,4);
delay = unique(delay,'rows');

delay_00 = data_00(:,3);
delay_00 = unique(delay_00,'rows');
delay_00(length(delay_00)) = [];
delay_00(length(delay_00)) = [];

delay_025 = data_025(:,3);
delay_025 = unique(delay_025,'rows');
delay_025(1) = [];
delay_025(length(delay_025)) = [];

delay_05 = data_05(:,3);
delay_05 = unique(delay_05,'rows');
delay_05(1) = [];

delay_075 = data_075(:,3);
delay_075 = unique(delay_075,'rows');
delay_075(1) = [];
delay_075(length(delay_075)) = [];

del = [delay_00 delay_025 delay_05 delay_075];

[X,Y] = meshgrid(angle,delay);
d=zeros(length(delay),length(angle));
s=zeros(length(delay),length(angle));
sd=zeros(length(delay),length(angle));
[sz1,sz2] = size(data);
[s1,s2] = size(angle);

x0=500;
y0=500;
width=750;
height=400;

%----------- height = f(angle)
fig4 = figure(4);

cmap = colormap(parula(12)); %30
h_2bl = 1./tan(angle.*pi/180);
h_bl = 0.5./tan(angle.*pi/180);
hold on
no_diss = [];
a_diss = [];
 
for i=1:4 
    d = del(3, i);
    txt = strcat("Distance  ", num2str(d));
    data_temp = data(data(:,3)== d, :);
    plot(data_temp(:,1),abs(data_temp(:,11)), '-o', 'Color',cmap(2*i,:));
    no_diss = [no_diss; abs(data_temp(data_temp(:,12)== 0, 11))];
    a_diss = [a_diss; data_temp(data_temp(:,12)== 0, 1)];
end

plot(angle,h_2bl,'--', 'Color','k', 'DisplayName','2 BL width');
plot(angle,h_bl,'--', 'Color','k', 'DisplayName','1 BL width');
plot(a_diss,no_diss,'x', 'Color','k', 'DisplayName','Dissolution not complete');
legend('Synchronized', '\pi/2 phase shift', '\pi phase shift', '3\pi/2 phase shift', '2 BL width', '1 BL width', 'Dissolution not complete')
ylabel('Mean bridge height [Body length unit]')
xlabel('V half-angle [Deg] ')

set(gcf,'position',[x0,y0,width,height])

title = fullfile(folder, 'f_angle_all_phi_d_5.png');
saveas(fig4,title);
title = fullfile(folder, 'f_angle_all_phi_d_5.fig');
saveas(fig4,title);

%----------- height = f(traffic)
fig5 = figure(5);
hold on
cmap = colormap(parula(60));
for i=1:length(angle)
    a = angle(i);
    txt = strcat("Angle  ", num2str(a));
    plot(data(data(:,1)== a, 4),abs(data(data(:,1)== a, 11)), '-o', 'DisplayName',txt, 'Color',cmap(3*i,:));
end
legend show
ylabel('Mean bridge height [Body length unit]')
xlabel('Distance between robots [Body length unit] ')

set(gcf,'position',[x0,y0,width,height])

title = fullfile(folder, 'f_dist_all_phi.png');
saveas(fig5,title);
title = fullfile(folder, 'f_dist_all_phi.fig');
saveas(fig5,title);

clear all

