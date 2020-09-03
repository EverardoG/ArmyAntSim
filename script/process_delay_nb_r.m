filepath_00 = '../experiments/limi_angle/delay_025_angle_lim_05pi_no_min_2019-02-28_14-50/results/nb_robots.txt'
data=dlmread(filepath_00, ';');
[folder,name,ext] = fileparts(filepath_00);

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

data = unique(data,'rows');

step_d=0.5;
angle = min(data(:,1)):2.5:max(data(:,1));
% delay = min(data(:,3)):1:max(data(:,3));
delay = data(:,4);
delay = unique(delay,'rows');

[X,Y] = meshgrid(angle,delay);
d=zeros(length(delay),length(angle));
s=zeros(length(delay),length(angle));
sd=zeros(length(delay),length(angle));
[sz1,sz2] = size(data);
[s1,s2] = size(angle);

% no bridges: 0; stacking: 1; no stability: 2; stable: 3
for i = 1:sz1
        x = data(i,1);
        y = data(i,4);
        k = find(angle==x);
        l = find(delay==y);
%         d(l,k) = data(i,12);
        if data(i, 6)==0
           s(l,k) = 0; 
        elseif data(i, 7) < 200
           s(l,k) = 1; 
           nb_txt(i,3) = data(i, 9);
           nb_txt(i,1) = data(i,1)+2.5/2;
           nb_txt(i,2) = data(i,4)+step_d/2;
        elseif data(i, 8)==0
           s(l,k) = 2;
           nb_txt(i,3) = data(i, 9);
           nb_txt(i,1) = data(i,1)+2.5/2;
           nb_txt(i,2) = data(i,4)+step_d/2;
        elseif data(i, 8)==1
           s(l,k) = 3;  
           nb_txt(i,3) = data(i, 9);
           nb_txt(i,1) = data(i,1)+2.5/2;
           nb_txt(i,2) = data(i,4)+step_d/2;
        else
           s(l,k) = 10;
        end
        
        if data(i, 6)==0 
           d(l,k) = 0; 
        elseif data(i, 10)==1
           d(l,k) = 100;   
        else
           d(l,k) = 100 - data(i, 11)/data(i, 9)*100;
           if(d(l,k)<0)
               d(l,k)=-1
           end
           
           nb_d_txt(i,3) = data(i, 11);
           nb_d_txt(i,1) = data(i,1)+2.5/2;
           nb_d_txt(i,2) = data(i,4)+step_d/2;
%            txt = strcat("Angle  ", num2str(a));
        end
%         sd(l,k) = d(l,k) && s(l,k);
end
   


x0=500;
y0=500;
width=750;
height=400;

%----------- meshmap of the stability
fig1=figure(1);
colorMap = [0 0 0; 0.9 0.4 0.4; 0 0.7 0.7; 1 1 1];
colormap(colorMap);
pcolor(X,Y,s);
%legend("No stable bridge")
xlabel('V-Angle/2 [Deg]')
ylabel('Distance between robots [Body length unit]')
set(gcf,'position',[x0,y0,width,height])

for i=1:length(nb_txt)
    text(nb_txt(i,1),nb_txt(i,2),num2str(nb_txt(i,3)),'Color',[0 0 0],'FontSize',10);
end
colorbar
title = fullfile(folder, 's_v2.png');
saveas(fig1,title);
title = fullfile(folder, 's_v2.fig');
saveas(fig1,title);
        
% %----------- meshmap of the dissolution
% figure(2)
% colorMap = [0 0 0; 1 1 1];
% colormap(colorMap);
% pcolor(X,Y,d);
% %legend("No stable bridge")
% xlabel('V-Angle [Deg]')
% ylabel('Delay [s]')
% legend('Total dissolution')

%----------- meshmap of the dissolution & stability
fig2=figure(2);
colorMap = [ linspace(0,1,100)', linspace(0,1,100)', linspace(0,1,100)'];%0 0.7 0.7;
% colorMap = [0 0 0; 1 1 1];
colormap(colorMap);
pcolor(X,Y,d);
%legend("No stable bridge")
xlabel('V-Angle/2 [Deg]')
ylabel('Distance between robots [Body length unit]')
set(gcf,'position',[x0,y0,width,height])

for i=1:length(nb_d_txt)
    text(nb_d_txt(i,1),nb_d_txt(i,2),num2str(nb_d_txt(i,3)),'Color',[1 1 1],'FontSize',14);
end

% str = 'dy/dx = 0';

colorbar
title = fullfile(folder, 's_and_d_v2.png');
saveas(fig2,title);
title = fullfile(folder, 's_and_d_v2.fig');
saveas(fig2,title);

clear all
