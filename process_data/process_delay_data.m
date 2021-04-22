
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

filepath_00 = '../experiments/delay_025_no_gripp_area_2019-03-02_22-43/results/length.txt';
filepath_gauss = '../experiments/delay_025_no_gripp_area_2019-03-02_22-43/results/length.txt';
data=dlmread(filepath_00, ';');
data_g=dlmread(filepath_gauss, ';');
[folder,name,ext] = fileparts(filepath_00);
%folder='../experiments/global_results/'
data = unique(data,'rows');
data_g = unique(data_g,'rows');

angle = min(data(:,1)):2.5:max(data(:,1));
% delay = min(data(:,3)):1:max(data(:,3));
delay = data(:,3);
delay = unique(delay,'rows');

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
% 
% %----------- meshmap of the stability
% fig1=figure(1);
% colorMap = [0 0 0; 1 1 1];
% colormap(colorMap);
% pcolor(X,Y,s);
% %legend("No stable bridge")
% xlabel('V-Angle [Deg]')
% ylabel('Distance between robots [Body length unit]')
% legend('Stable bridge')
% set(gcf,'position',[x0,y0,width,height])
% title = fullfile(folder, 's_v2.png');
% saveas(fig1,title);
% title = fullfile(folder, 's_v2.fig');
% saveas(fig1,title);
%         
% % %----------- meshmap of the dissolution
% % figure(2)
% % colorMap = [0 0 0; 1 1 1];
% % colormap(colorMap);
% % pcolor(X,Y,d);
% % %legend("No stable bridge")
% % xlabel('V-Angle [Deg]')
% % ylabel('Delay [s]')
% % legend('Total dissolution')
% 
% %----------- meshmap of the dissolution & stability
% fig2=figure(2);
% colorMap = [0 0 0; 1 1 1];
% colormap(colorMap);
% pcolor(X,Y,sd);
% %legend("No stable bridge")
% xlabel('V-Angle [Deg]')
% ylabel('Distance between robots [Body length unit]')
% legend('Stable bridge and Total dissolution')
% set(gcf,'position',[x0,y0,width,height])
% 
% title = fullfile(folder, 's_and_d_v2.png');
% saveas(fig2,title);
% title = fullfile(folder, 's_and_d_v2.fig');
% saveas(fig2,title);

%----------- height = f(angle)
fig4 = figure(4);

cmap = colormap(parula(12)); %30
h_2bl = 1./tan(angle.*pi/180);
h_bl = 0.5./tan(angle.*pi/180);
no_diss = abs(data(data(:,12)== 0, 11));
a_diss = abs(data(data(:,12)== 0, 1));
hold on
digits(2);
for i=1:length(delay) 
    d = delay(i);
    dist = round(d*0.52/1.02, 1);
    txt = strcat("Distance  ", num2str(dist), " BL");
    plot(data(data(:,3)== d, 1),abs(data(data(:,3)== d, 11)), '-o', 'Color',cmap(2*i,:), 'DisplayName', txt);
end

plot(angle,h_2bl,'--', 'Color','k', 'DisplayName','2 BL width');
plot(angle,h_bl,'--', 'Color','k', 'DisplayName','1 BL width');
plot(a_diss,no_diss,'x', 'Color','k', 'DisplayName','Dissolution not complete');
legend show
ylabel('Mean bridge height [Body length unit]')
xlabel('V half-angle [Deg] ')

set(gcf,'position',[x0,y0,width,height])

title = fullfile(folder, 'f_angle_v2.png');
saveas(fig4,title);
title = fullfile(folder, 'f_angle_v2.fig');
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

title = fullfile(folder, 'f_dist_v2.png');
saveas(fig5,title);
title = fullfile(folder, 'f_dist_v2.fig');
saveas(fig5,title);

clear all

