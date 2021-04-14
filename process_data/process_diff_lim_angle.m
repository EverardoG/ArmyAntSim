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
% 
% 
% 
% filepath(1) = "../experiments/limi_angle/delay_05_angle_lim_0_2019-02-27_11-38/results/length.txt"; 
% filepath(2) = "../experiments/limi_angle/delay_025_angle_lim_078_2019-03-01_08-40/results/length.txt";
% filepath(3) = '../experiments/limi_angle/delay_05_phase_limit_angle_2019-02-04_16-47/results/length.txt'; 
% filepath(4) = '../experiments/limi_angle/delay_05_angle_lim_2_2019-02-27_14-29/results/length.txt'; 
% filepath(5) = '../experiments/limi_angle/delay_05_angle_lim_3_2019-02-27_08-43/results/length.txt'; 
% % filepath(1) = "../experiments/limi_angle/delay_025_angle_lim_0_2019-02-27_23-24/results/length.txt"; 
% % filepath(2) = '../experiments/limi_angle/delay_025_phase_angle_lim_30_abs_2019-02-08_10-45/results/length.txt'; 
% % filepath(3) = '../experiments/limi_angle/delay_025_angle_lim_2_25_2019-02-27_16-38/results/length.txt'; 
% % filepath(4) = '../experiments/limi_angle/delay_025_angle_lim_3_2019-02-27_20-57/results/length.txt'; 
% % filepath(5) = '../experiments/limi_angle/delay_025_angle_lim_0_no_min_2019-02-28_11-57/results/length.txt';
% % filepath(6) = '../experiments/limi_angle/delay_025_angle_lim_05pi_no_min_2019-02-28_14-50/results/length.txt';
% 
% 
% for i=1:length(filepath)
%     data(:,:,i)=unique(dlmread(filepath(i), ';'),'rows');
%     data(:,:,i)=sortrows(data(:,:,i),1);
% end
% 
% folder='../experiments/global_results/lim_angle/'
% 
% 
% delay=unique(data(:,3,1),'rows');
% angle=unique(data(:,1,1),'rows');
% h_2bl = 1./tan(angle.*pi/180);
% h_bl = 0.5./tan(angle.*pi/180);
% 
% x0=500;
% y0=500;
% width=750;
% height=400;
% 
% %========================================================================
% %
% %                  HEIGHT
% %
% %=========================================================================
% cmap = colormap(parula(15)); %30
% lim_angle=[0 0.78*180/pi 0.5 2.25/pi 3/pi 0 0.5];
% for i=1:length(delay)
%     fig= figure(i);
%     hold on
%     for j=1:length(filepath)
%         d = delay(i);
%         dist = (floor(d)+0.5)*0.52/1.02;
%         a = lim_angle(j);
%         txt = strcat("Min angle to avoid pushing: ", num2str(a,2), "\pi Rad");
%         if j>4
%             txt = strcat("Min angle to avoid pushing: ", num2str(a,2), "\pi Rad, no min angle");
%         end
%         tmp = data(data(:,3,j)==d,11,j);
%         plot(angle,abs(data(data(:,3,j)==d,11,j)), '-', 'Color',cmap(2*j,:), 'DisplayName',txt);
%     end
%     % plot(angle,abs(h_mean));
%     plot(angle,h_2bl,'--', 'Color','k', 'DisplayName','2 BL width');
%     plot(angle,h_bl,'--', 'Color','k', 'DisplayName','1 BL width');
%     legend show
%     ylabel('Mean bridge height [Body length unit]')
%     xlabel('V-Angle/2 [Deg] ')
% 
%     set(gcf,'position',[x0,y0,width,height])
%     
%     txt_title = strcat("height_delay_", num2str(dist,2), ".png");
%     title = fullfile(folder, txt_title);
%     saveas(fig,title);
%     txt_title = strcat("height_delay_", num2str(dist,2), ".png");
%     title = fullfile(folder, txt_title);
%     saveas(fig,title);
% end

clear all

%==============================================================
%----------- percent of dissolution

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

filepath(1) = "../experiments/limi_angle/delay_025_angle_lim_0_2019-02-27_23-24/results/nb_robots.txt"; 
filepath(2) = "../experiments/limi_angle/delay_025_angle_lim_078_2019-03-01_08-40/results/nb_robots.txt";
filepath(3) = '../experiments/limi_angle/delay_025_phase_angle_lim_30_abs_2019-02-08_10-45/results/nb_robots.txt'; 
filepath(4) = '../experiments/limi_angle/delay_025_angle_lim_2_25_2019-02-27_16-38/results/nb_robots.txt'; 
filepath(5) = '../experiments/limi_angle/delay_025_angle_lim_3_2019-02-27_20-57/results/nb_robots.txt'; 

% filepath(5) = '../experiments/limi_angle/delay_025_angle_lim_0_no_min_2019-02-28_11-57/results/nb_robots.txt';
% filepath(6) = '../experiments/limi_angle/delay_025_angle_lim_05pi_no_min_2019-02-28_14-50/results/nb_robots.txt';

fig = figure(7)
x0=500;
y0=500;
width=750;
height=400;

for i=1:length(filepath)
%     data(:,:,i)=unique(dlmread(filepath(i), ';'),'rows');
    tmp=unique(dlmread(filepath(i), ';'),'rows');
    data(:,:,i)=tmp(tmp(:,3)<9.5,:); %7.5
    data(:,:,i)=sortrows(data(:,:,i),1);
    percent_diss(i) = sum(data(data(:,6,i)==1,10,i))/sum(data(:,6,i))*100;
    percent_stack(i) = sum(data(data(:,7,i)<200,6,i))/sum(data(:,6,i))*100;
end

folder='../experiments/global_results/lim_angle/'
lim_angle=[0 0.78*180/pi 0.5*180 2.25*180/pi 3*180/pi];
plot(lim_angle,percent_diss, 'r+','MarkerSize',10);
hold on
% plot(lim_angle,percent_stack);
ylabel('Percentage of successful dissolution [%]')
xlabel('Min angle to avoid pushing [Deg] ')
ylim([0 100])

set(gcf,'position',[x0,y0,width,height])

txt_title = strcat("percent_diss.fig");
title = fullfile(folder, txt_title);
saveas(fig,title);
txt_title = strcat("percent_diss.png");
title = fullfile(folder, txt_title);
saveas(fig,title);
clear all

%-------------------------
%    critical angle


% for i=1:length(filepath)
%     data(:,:,i)=unique(dlmread(filepath(i), ';'),'rows');
%     tmp=unique(dlmread(filepath(i), ';'),'rows');
%     data(:,:,i)=tmp(tmp(:,3)<9.5,:); %7.5
%     data_a(:,:,i)=sortrows(data(:,:,i),1);
%     delay = unique(data_a(:,3,i), 'rows')
%     for j=1:length(delay)
%         temp = data_a(data_a(:,3,i) == delay(j),:,i)
%         s = temp(temp(:,8) == 1,:,i)
%         tf_c(i,j) = max(); 
%     end
%     percent_diss(i) = sum(data(data(:,6,i)==1,10,i))/sum(data(:,6,i))*100;
%     percent_stack(i) = sum(data(data(:,7,i)<200,6,i))/sum(data(:,6,i))*100;
% end
