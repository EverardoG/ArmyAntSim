% filepath_1 = '../experiments/delay_gauss_025_2019-02-20_22-17/results/nb_robots.txt'; 
filepath_1 = '../experiments/periodic_delay_T_18_m_2_5_2019-03-03_22-54/results/nb_robots.txt'; 
% filepath_2 = '../experiments/delay_gauss_025_2019-02-21_08-45/results/nb_robots.txt';
% filepath_3 = '../experiments/delay_gauss_025_2019-02-23_20-22/results/nb_robots.txt'; 
% filepath_4 = '../experiments/delay_gauss_025_2019-02-21_14-40/results/nb_robots.txt'; 
% filepath_5 = '../experiments/delay_gauss_025_2019-02-21_16-33/results/nb_robots.txt'; 
% filepath_6 = '../experiments/delay_gauss_025_2019-02-22_08-37/results/nb_robots.txt'; 
% filepath_7 = '../experiments/delay_gauss_025_2019-02-22_10-46/results/nb_robots.txt'; 
% filepath_8 = '../experiments/delay_gauss_025_2019-02-22_12-56/results/nb_robots.txt'; 
% filepath_9 = '../experiments/delay_gauss_025_2019-02-22_20-56/results/nb_robots.txt'; 
% filepath_10 = '../experiments/delay_gauss_025_2019-02-22_14-56/results/nb_robots.txt'; 


data(:,:,1)=unique(dlmread(filepath_1, ';'),'rows');
% data(:,:,2)=unique(dlmread(filepath_2, ';'),'rows');
% data(:,:,3)=unique(dlmread(filepath_3, ';'),'rows');
% data(:,:,4)=unique(dlmread(filepath_4, ';'),'rows');
% data(:,:,5)=unique(dlmread(filepath_5, ';'),'rows');
% data(:,:,6)=unique(dlmread(filepath_6, ';'),'rows');
% data(:,:,7)=unique(dlmread(filepath_7, ';'),'rows');
% data(:,:,8)=unique(dlmread(filepath_8, ';'),'rows');
% data(:,:,9)=unique(dlmread(filepath_9, ';'),'rows');
% data(:,:,10)=unique(dlmread(filepath_10, ';'),'rows');
%data=data_00;
nb_exp=1;
[folder,name,ext] = fileparts(filepath_1);
%folder='../experiments/global_results/gaussian/result_10_exp/'

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
angle = [angle 52.5];
% delay = min(data(:,3)):1:max(data(:,3));
delay = data(:,3,1);
delay = unique(delay,'rows');
delay = [delay; 8.5];
delay = (floor(delay)+0.5).*0.52./1.02;
delay = round(delay,3);

[X,Y] = meshgrid(angle,delay);
d=zeros(length(delay),length(angle));
d_bis=zeros(length(delay),length(angle));
s=zeros(length(delay),length(angle));
sd=zeros(length(delay),length(angle));
[sz1,sz2] = size(data);
[s1,s2] = size(angle);
txt_bis=zeros(sz1);
nb_bridge=ones(length(delay),length(angle)).*nb_exp;
% 
% 

% 0: no bridges; -100:stacking; 2:no stability; 1:stable
for i = 1:sz1
        x = data(i,1,1);
        y = (floor(data(i,3,1))+0.5).*0.52./1.02;
        y = round(y,3);
        k = find(angle==x);
        l = find(delay==y);
%         d(l,k) = data(i,12);
        for j=1:nb_exp
           if data(i, 6, j)==0
               s(l,k,j) = 0; 
               nb_txt(i,3,j) = 0;
               nb_txt(i,1,j) = data(i,1,j)+step_a/2;
               nb_txt(i,2,j) = y+step_d/2;
           elseif data(i, 7, j) < 200
               s(l,k,j) = -0.1; 
               nb_txt(i,3,j) = data(i, 9,j);
               nb_txt(i,1,j) = data(i,1,j)+step_a/2;
               nb_txt(i,2,j) = y+step_d/2;
            elseif data(i, 8, j)==0
               s(l,k,j) = 2;
               nb_txt(i,3,j) = data(i, 9, j);
               nb_txt(i,1,j) = data(i,1, j)+step_a/2;
               nb_txt(i,2,j) = y+step_d/2;
            elseif data(i, 8, j)==1
               s(l,k,j) = 1;  
               nb_txt(i,3,j) = data(i, 9, j);
               nb_txt(i,1,j) = data(i,1, j)+step_a/2;
               nb_txt(i,2,j) = y+step_d/2;
           end
            
           
            if data(i, 6, j)==0 
               d(l,k,j) = 0; 
               nb_d_txt(i,3,j) = 0;
               nb_d_txt(i,1,j) = data(i,1,j)+step_a/2;
               nb_d_txt(i,2,j) = y+step_d/2;
               nb_bridge(l,k)=nb_bridge(l,k)-1;
            elseif data(i, 10, j)==1
               d(l,k,j) = 100;
               d_bis(l,k,j) = 1;
               nb_d_txt(i,3,j) = 0;
               nb_d_txt(i,1,j) = data(i,1,j)+step_a/2;
               nb_d_txt(i,2,j) = y+step_d/2;
            else
               d(l,k,j) = 100 - data(i, 11,j)/data(i, 9,j)*100;
               d_bis(l,k,j) = 0;
               txt_bis(i)=txt_bis(i)+1;
               if(d(l,k,j)<0)
                   d(l,k,j)=-1;
               end

               nb_d_txt(i,3,j) = data(i, 11,j);
               nb_d_txt(i,1,j) = data(i,1,j)+step_a/2;
               nb_d_txt(i,2,j) = y+step_d/2;
    %            txt = strcat("Angle  ", num2str(a));
            end
            if(nb_bridge(l,k)==0)
                nb_bridge(l,k) = nb_exp;
            end

        end
end
   
s_mean = mean(s,3)*100;
d_mean = mean(d,3)./nb_bridge*nb_exp;
d_bis_mean = mean(d_bis,3)./nb_bridge*nb_exp;
nb_txt_mean = mean(nb_txt,3);
nb_txt_mean(:,3) = round(nb_txt_mean(:,3),2);
nb_txt_mean(nb_txt_mean(:,3)==0,:)=[];
nb_d_txt_mean = mean(nb_d_txt,3);
nb_d_txt_mean(:,3) = round(nb_d_txt_mean(:,3),2);
nb_d_txt_mean(nb_d_txt_mean(:,3)==0,:)=[];
x0=500;
y0=500;
width=750;
height=400;

%----------- meshmap of the stability
fig1=figure(1);
colorMap = [ 0.9 0.4 0.4; linspace(0,1,100)', linspace(0,1,100)', linspace(0,1,100)'];%0 0.7 0.7;
colormap(colorMap);
pcolor(X,Y,s_mean);
%legend("No stable bridge")
xlabel('V-Angle/2 [Deg]')
ylabel('Delay between robots [Body length unit]')
set(gcf,'position',[x0,y0,width,height])

for i=1:length(nb_txt_mean)
    x = nb_txt_mean(i,1)-step_a/2;
    y = nb_txt_mean(i,2)-step_d/2;
    y = round(y,3);
    k = find(angle==x);
    l = find(delay==y);
    if(s_mean(l,k)<20)
        text(nb_txt_mean(i,1),nb_txt_mean(i,2),num2str(nb_txt_mean(i,3)/nb_bridge(l,k)*nb_exp,2),'Color',[1 1 1],'FontSize',10);
    else
        text(nb_txt_mean(i,1),nb_txt_mean(i,2),num2str(nb_txt_mean(i,3)/nb_bridge(l,k)*nb_exp,2),'Color',[0 0 0],'FontSize',10);
    end
end
colorbar
title = fullfile(folder, 'gaussian_s.png');
saveas(fig1,title);
title = fullfile(folder, 'gaussian_s.fig');
saveas(fig1,title);

% %----------- meshmap of the dissolution & stability/ average % of
% dissolution and number of robots in the bridge
fig2=figure(2);
colorMap = [ linspace(0,1,100)', linspace(0,1,100)', linspace(0,1,100)'];%0 0.7 0.7;
colormap(colorMap);
pcolor(X,Y,d_mean);
%legend("No stable bridge")
xlabel('V-Angle/2 [Deg]')
ylabel('Mean distance between robots [Body length unit]')
set(gcf,'position',[x0,y0,width,height])

for i=1:length(nb_d_txt_mean)
    x = nb_d_txt_mean(i,1)-step_a/2;
    y = nb_d_txt_mean(i,2)-step_d/2;
    y = round(y,3);
    k = find(angle==x);
    l = find(delay==y);
    if(d_mean(l,k)>80)
         text(nb_d_txt_mean(i,1),nb_d_txt_mean(i,2),num2str(nb_d_txt_mean(i,3)/nb_bridge(l,k)*nb_exp,2),'Color',[0 0 0],'FontSize',10);
    else
        text(nb_d_txt_mean(i,1),nb_d_txt_mean(i,2),num2str(nb_d_txt_mean(i,3)/nb_bridge(l,k)*nb_exp,2),'Color',[1 1 1],'FontSize',10);
    end
end

str = 'dy/dx = 0';

colorbar
title = fullfile(folder, 'gaussian_s_and_d.png');
saveas(fig2,title);
title = fullfile(folder, 'gaussian_s_and_d.fig');
saveas(fig2,title);

% %----------- meshmap of the dissolution & stability
fig3=figure(3);
colorMap = [ linspace(0,1,nb_exp+1)', linspace(0,1,nb_exp+1)', linspace(0,1,nb_exp+1)'];%0 0.7 0.7;
colormap(colorMap);
pcolor(X,Y,d_bis_mean*100);
%legend("No stable bridge"3
xlabel('V-Angle/2 [Deg]')
ylabel('Distance between robots [Body length unit]')
set(gcf,'position',[x0,y0,width,height])

for i=1:length(nb_d_txt)
    if(txt_bis(i)==0)
        continue
    end
    if(txt_bis(i)<3)
        text(nb_d_txt(i,1,1),nb_d_txt(i,2,1),num2str(txt_bis(i)),'Color',[0 0 0],'FontSize',10);
    else
        text(nb_d_txt(i,1,1),nb_d_txt(i,2,1),num2str(txt_bis(i)),'Color',[1 1 1],'FontSize',10);
    end
end

% str = 'dy/dx = 0';

colorbar
title = fullfile(folder, 'gaussian_s_and_d_v2.png');
saveas(fig3,title);
title = fullfile(folder, 'gaussian_s_and_d_v2.fig');
saveas(fig3,title);
% 
clear all


%========================================================================
%
%                  HEIGHT
%
%=========================================================================
% 
% folder='../experiments/global_results/gaussian/result_10_exp/'
fig4=figure(4)
x0=500;
y0=500;
width=750;
height=400;
set(gcf,'position',[x0,y0,width,height])

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

filepath(1) = "../experiments/periodic_delay_T_18_m_2_5_2019-03-03_22-54/results/mid_height.txt"; 

% filepath(1) = "../experiments/delay_gauss_025_2019-02-20_22-17/results/mid_height.txt"; 
% filepath(2) = "../experiments/delay_gauss_025_2019-02-21_08-45/results/mid_height.txt"; 
% filepath(3) = "../experiments/delay_gauss_025_2019-02-23_20-22/results/mid_height.txt"; %delay_gauss_025_2019-02-23_16-25
% filepath(4) = '../experiments/delay_gauss_025_2019-02-21_14-40/results/mid_height.txt'; 
% filepath(5) = '../experiments/delay_gauss_025_2019-02-21_16-33/results/mid_height.txt'; 
% filepath(6) = '../experiments/delay_gauss_025_2019-02-22_08-37/results/mid_height.txt'; 
% filepath(7) = '../experiments/delay_gauss_025_2019-02-22_10-46/results/mid_height.txt'; 
% filepath(8) = '../experiments/delay_gauss_025_2019-02-22_12-56/results/mid_height.txt'; 
% filepath(9) = '../experiments/delay_gauss_025_2019-02-22_20-56/results/mid_height.txt'; 
% filepath(10) = '../experiments/delay_gauss_025_2019-02-22_14-56/results/mid_height.txt'; 

[folder,name,ext] = fileparts(filepath(1));
cmap = colormap(parula(15)); %30

for i=1:length(filepath)
    data_l(:,:,i)=unique(dlmread(filepath(i), ';'),'rows');
    data_l(:,:,i)=sortrows(data_l(:,:,i),1);
%     data_2_25(:,:,i)=data_l(data_l(:,3,i)==2.25,:,i);
%     data_3_25(:,:,i)=data_l(data_l(:,3,i)==3.25,:,i);
%     data_4_25(:,:,i)=data_l(data_l(:,3,i)==4.25,:,i);
%     data_5_25(:,:,i)=data_l(data_l(:,3,i)==5.25,:,i);
end
% h(:,:)=data_2_25(:,11,:);

delay_l=unique(data_l(:,3,1),'rows');
angle=unique(data_l(:,1,1),'rows');
h_2bl = 1./tan(angle.*pi/180);
h_bl = 0.5./tan(angle.*pi/180);
cmap = colormap(parula(12));

hold on 
for i=1:length(delay_l)
    d = delay_l(i);
    for j=1:length(filepath)
        temp(:,j)=data_l(data_l(:,3,j)==d,11,j);
    end
    data_mean = mean(temp,2);
    
    dist=(floor(d)+0.5)*0.52/1.02;
    txt = strcat("Mean distance  ", num2str(dist,2), " BL");
    plot(angle,abs(data_mean), '-', 'Color',cmap(2*i,:), 'DisplayName',txt);
end

% plot(angle,abs(h_mean));
plot(angle,h_2bl,'--', 'Color','k', 'DisplayName','2 BL width');
plot(angle,h_bl,'--', 'Color','k', 'DisplayName','1 BL width');
legend show
ylabel('Mean bridge height [Body length unit]')
xlabel('V-Angle/2 [Deg] ')

title = fullfile(folder, 'gaussian_middle_height.png');
saveas(fig4,title);
title = fullfile(folder, 'gaussian_middle_height.fig');
saveas(fig4,title);

clear all

% 
