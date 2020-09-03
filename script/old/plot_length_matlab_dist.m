filepath_d7 = '../experiments/distance/traffic_2019-01-03_10-09/results/length.txt'
data_d7=dlmread(filepath_d7, ';');
filepath_d8 = '../experiments/distance/traffic_2019-01-10_15-02/results/length.txt'
data_d8=dlmread(filepath_d8, ';');
filepath_d20 = '../experiments/distance/traffic_2019-01-11_09-11/results/length.txt'
data_d20=dlmread(filepath_d20, ';');
filepath_d15 = '../experiments/distance/traffic_2019-01-11_11-11/results/length.txt'
data_d15=dlmread(filepath_d15, ';');
filepath_d12 = '../experiments/distance/traffic_2019-01-11_13-16/results/length.txt'
data_d12=dlmread(filepath_d12, ';');
filepath_d10 = '../experiments/distance/traffic_2019-01-11_15-09/results/length.txt'
data_d10=dlmread(filepath_d10, ';');

data=[data_d7; data_d8; data_d20; data_d15; data_d10; data_d10];
%Data is organized as following: 
%data{:,1} V-angle; 
%data{:,2} V-height;
%data{:,3} distance/delay between robots; 
%data{:,4} stability; 
%data{:,5} bridge length;
%data{:,6} V-bottom y-coord; 
%data{:,7} bridge right height; 
%data{:,8} bridge left height; 
%data{:,9} bridge mean height; 

dl = abs(data(:,7));
angle = [10, 15, 20, 25, 30, 35, 40];
mean_dl_angle(1)=mean(data(data(:,1) == 10, 8));
mean_dl_angle(2)=mean(data(data(:,1) == 15, 8));
mean_dl_angle(3)=mean(data(data(:,1) == 20, 8));
mean_dl_angle(4)=mean(data(data(:,1) == 25, 8));
mean_dl_angle(5)=mean(data(data(:,1) == 30, 8));
mean_dl_angle(6)=mean(data(data(:,1) == 35, 8));
mean_dl_angle(7)=mean(data(data(:,1) == 40, 8));

% %---- Fixed delay
% data_2_75 = data(data(:,3) == 2.75, :);
% for i=1:7
%     a = 5+i*5
%     mean_data_2_75(i)= mean(data_2_75(data_2_75(:,1) == a, 8));
% end
% 
% data_3 = data(data(:,3) == 3, :);
% for i=1:7
%     a = 5+i*5
%     mean_data_3(i)= mean(data_3(data_3(:,1) == a, 8));
% end
% 
% data_3_25 = data(data(:,3) == 3.25, :);
% for i=1:7
%     a = 5+i*5
%     mean_data_3_25(i)= mean(data_3_25(data_3_25(:,1) == a, 8));
% end
% 
% data_3_5 = data(data(:,3) == 3.5, :);
% for i=1:7
%     a = 5+i*5
%     mean_data_3_5(i)= mean(data_3_5(data_3_5(:,1) == a, 8));
% end
% 
% figure(1)
% cmap = colormap(parula(5));
% plot(angle, abs(mean_data_2_75), '-x', 'Color',cmap(1,:));
% hold on
% plot(angle, abs(mean_data_3), '-x', 'Color',cmap(2,:));
% plot(angle, abs(mean_data_3_25), '-x', 'Color',cmap(3,:));
% plot(angle, abs(mean_data_3_5), '-x', 'Color',cmap(4,:));
% legend('distance between robots of 2.75 BL', 'distance between robots of 3 BL', 'distance between robots of 3.25 BL', 'distance between robots of 3.75 BL');
% xlabel('V-Angle [Deg]')
% ylabel('Left bridge height [Body length]')
% 
% %---- dl = f(angle)
% figure(2)
% colormap(bone);
% dl_h8 = data(data(:,2) == 8, 7);
% plot(data(:,1), abs(data(:,7)), 'x');
% hold on 
% plot(angle, abs(mean_dl_angle), '-x');
% %plot(angle, abs(dl_h8), '-x'); plot(data(:,3), abs(data(:,8)), '-x');
% %plot(data(:,3), abs(data(:,9)), '-x'); plot(data(:,3), abs(data(:,4)),
% %'-x');


%---- dl = f(traffic)
%traffic = 1.1:0.1:2.6;
traffic = [1.1 1.2 1.3 1.4 1.5 1.6 1.7 1.8 1.9 2 2.1 2.2 2.3 2.4 2.5 2.6];
a=35;
dl_a35 = data(data(:,1) == a, :);
dl_a35_stable = dl_a35(dl_a35(:,4) > 0, :);
dl_a35_unstable = dl_a35(dl_a35(:,4) == 0, :);

for i=1:length(traffic)
    tf = traffic(i);
    mean_dl_a35(i)= mean(dl_a35(dl_a35(:,3) == tf, 8));
    mean_dr_a35(i)= mean(dl_a35(dl_a35(:,3) == tf, 7));
    mean_dl_a35_stable(i)= mean(dl_a35_stable(dl_a35_stable(:,3) == tf, 8));
    mean_dr_a35_stable(i)= mean(dl_a35_stable(dl_a35_stable(:,3) == tf, 7));
end


figure(1)
hold on 
plot(1./dl_a35_unstable(:,3), abs(dl_a35_unstable(:,8)), 'x','Color',[219/256,160/256,94/256]);
plot(1./dl_a35_stable(:,3), abs(dl_a35_stable(:,8)), 'o','Color',[219/256,160/256,94/256]);
plot(1./traffic, abs(mean_dl_a35_stable), '-','Color',[239/256,149/256,68/256]);
plot(1./traffic, abs(mean_dl_a35), '--','Color',[239/256,149/256,68/256]);

plot(1./dl_a35_unstable(:,3), abs(dl_a35_unstable(:,7)), 'x','Color',[87/256,172/256,195/256]);
plot(1./dl_a35_stable(:,3), abs(dl_a35_stable(:,7)), 'o','Color',[87/256,172/256,195/256]);
plot(1./traffic, abs(mean_dr_a35_stable), '-','Color',[87/256,150/256,195/256]);
plot(1./traffic, abs(mean_dr_a35), '--','Color',[87/256,150/256,195/256]);

grid on
ylabel('Right/left bridge height [Body length unit]')
xlabel('Traffic [Robot/Body length unit] ')
legend('Unstable bridge', 'Stable bridge', 'Average left bridge height (stable bridges)', 'Average left bridge height (all bridges)','Unstable bridge', 'Stable bridge', 'Average right bridge height (stable bridges)', 'Average right bridge height (all bridges)');
%---- dl-dr = f(traffic)
cmap = colormap(parula(10));
%traffic = 1.1:0.1:2.6;
or=zeros(18);
t2=[4 traffic 0.1];
figure(2)
hold on 
plot(1./traffic, abs(mean_dl_a35)-abs(mean_dr_a35), '-', 'Color',[0.9,0.4,0]);
plot(1./dl_a35(:,3), abs(dl_a35(:,8))-abs(dl_a35(:,7)), 'x', 'Color',[0,0.6,0.9]);
plot(1./t2, or, 'k');
axis([ 0.3 0.91 -4 12]);
ax = gca;
grid on
ylabel('h_{l} - h_{r} [Body length unit]')
xlabel('Traffic [1/Body length unit] ')
