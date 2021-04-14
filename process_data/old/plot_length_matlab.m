filepath = '../experiments/distance_angle_2019-01-18_15-25/results/length.txt' %flipp_dist_from_bottom
data=dlmread(filepath, ';');

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
%(data{:,10} initial distance from the bottom of the V;) (opt)

data = data(data(:, 3) > 1.3, :);
data = data(data(:, 3) < 2.5, :);
data = data(data(:, 2) > 5, :);
% data = data(data(:, 1) == 35, :);

dl = abs(data(:,7));
angle = [10, 15, 20, 25, 30, 35, 40, 45, 50];
mean_dl_angle(1)=mean(data(data(:,1) == 10, 8));
mean_dl_angle(2)=mean(data(data(:,1) == 15, 8));
mean_dl_angle(3)=mean(data(data(:,1) == 20, 8));
mean_dl_angle(4)=mean(data(data(:,1) == 25, 8));
mean_dl_angle(5)=mean(data(data(:,1) == 30, 8));
mean_dl_angle(6)=mean(data(data(:,1) == 35, 8));
mean_dl_angle(7)=mean(data(data(:,1) == 40, 8));
mean_dl_angle(8)=mean(data(data(:,1) == 45, 8));
mean_dl_angle(9)=mean(data(data(:,1) == 50, 8));

mean_dl_angle_s(1)=mean(data(data(data(:,4) == 1,1) == 10, 8));
mean_dl_angle_s(2)=mean(data(data(data(:,4) == 1,1) == 15, 8));
mean_dl_angle_s(3)=mean(data(data(data(:,4) == 1,1) == 20, 8));
mean_dl_angle_s(4)=mean(data(data(data(:,4) == 1,1) == 25, 8));
mean_dl_angle_s(5)=mean(data(data(data(:,4) == 1,1) == 30, 8));
mean_dl_angle_s(6)=mean(data(data(data(:,4) == 1,1) == 35, 8));
mean_dl_angle_s(7)=mean(data(data(data(:,4) == 1,1) == 40, 8));
mean_dl_angle_s(8)=mean(data(data(data(:,4) == 1,1) == 45, 8));
mean_dl_angle_s(9)=mean(data(data(data(:,4) == 1,1) == 50, 8));

%---- Fixed delay 
data_2_75 = data(data(:,3) == 2.1, :);
for i=1:9
    a = 5+i*5;
    mean_data_2_75(i)= mean(data_2_75(data_2_75(:,1) == a, 8));
    mean_data_2_75_r(i)= mean(data_2_75(data_2_75(:,1) == a, 7));
end

data_3 = data(data(:,3) == 1.5, :);
for i=1:9
    a = 5+i*5;
    mean_data_3(i)= mean(data_3(data_3(:,1) == a, 8));
    mean_data_3_r(i)= mean(data_3(data_3(:,1) == a, 7));
end

data_3_25 = data(data(:,3) == 1.7, :);
for i=1:9
    a = 5+i*5;
    mean_data_3_25(i)= mean(data_3_25(data_3_25(:,1) == a, 8));
    mean_data_3_25_r(i)= mean(data_3_25(data_3_25(:,1) == a, 7));
end

data_3_5 = data(data(:,3) == 1.9, :);
for i=1:9
    a = 5+i*5;
    mean_data_3_5(i)= mean(data_3_5(data_3_5(:,1) == a, 8));
    mean_data_3_5_r(i)= mean(data_3_5(data_3_5(:,1) == a, 7));
end

figure(1)
cmap = colormap(parula(5));
hold on
plot(angle, abs(mean_data_3), '-x', 'Color',cmap(2,:));
plot(angle, abs(mean_data_3_25), '-x', 'Color',cmap(3,:));
plot(angle, abs(mean_data_3_5), '-x', 'Color',cmap(4,:));
plot(angle, abs(mean_data_2_75), '-x', 'Color',cmap(1,:));
plot(angle, abs(mean_data_2_75_r), '--', 'Color',cmap(1,:));
plot(angle, abs(mean_data_3_r), '--', 'Color',cmap(2,:));
plot(angle, abs(mean_data_3_25_r), '--', 'Color',cmap(3,:));
plot(angle, abs(mean_data_3_5_r), '--', 'Color',cmap(4,:));
legend('distance between robots of 1.5 BL', 'distance between robots of 1.7 BL', 'distance between robots of 1.9 BL', 'distance between robots of 2.1 BL');
xlabel('V-Angle [Deg]')
ylabel('Left/Right bridge height [Body length unit]')

%---- dl = f(angle)
figure(2)
colormap(bone);
dl_h8 = data(data(:,2) == 8, 7);
plot(data(data(:,4)==1,1), abs(data(data(:,4)==1,8)), 'o', 'Color',[87/256,172/256,195/256]);
hold on 
plot(data(data(:,4)==0,1), abs(data(data(:,4)==0,8)), 'x', 'Color',[87/256,172/256,195/256]);
plot(angle, abs(mean_dl_angle), '-', 'Color',[239/256,149/256,68/256]);
%plot(angle, abs(mean_dl_angle_s), '--');
xlabel('V-Angle [Deg]')
ylabel('Left bridge height [Body length unit]')
legend('Stable bridge', 'Unstable bridge', 'Average height');
%plot(angle, abs(dl_h8), '-x');
%plot(data(:,3), abs(data(:,8)), '-x');
%plot(data(:,3), abs(data(:,9)), '-x');
%plot(data(:,3), abs(data(:,4)), '-x');


%---- dl = f(traffic)
traffic = [2.75, 3, 3.25, 3.5];
dl_a20 = data(data(:,1) == 20, :);
a=20;
for i=1:4
    tf = traffic(i);
    mean_dl_a20(i)= mean(dl_a20(dl_a20(:,3) == tf, 7));
end

dl_a30 = data(data(:,1) == 30, :);
a=30;
for i=1:4
    tf = traffic(i);
    mean_dl_a30(i)= mean(dl_a30(dl_a30(:,3) == tf, 7));
end

dl_a25 = data(data(:,1) == 25, :);
a=25;
for i=1:4
    tf = traffic(i);
    mean_dl_a25(i)= mean(dl_a25(dl_a25(:,3) == tf, 7));
end

a=15;
dl_a15 = data(data(:,1) == a, :);
for i=1:4
    tf = traffic(i);
    mean_dl_a15(i)= mean(dl_a15(dl_a15(:,3) == tf, 7));
end

a=35;
dl_a35 = data(data(:,1) == a, :);
for i=1:4
    tf = traffic(i);
    mean_dl_a35(i)= mean(dl_a35(dl_a35(:,3) == tf, 7));
end

% figure(3)
% plot(traffic, abs(mean_dl_a20), '-x');
% hold on 
% plot(dl_a20(:,3), abs(dl_a20(:,7)), 'x');
% plot(traffic, abs(mean_dl_a30), '-x');
% plot(dl_a30(:,3), abs(dl_a30(:,7)), 'x');
% plot(traffic, abs(mean_dl_a25), '-x');
% plot(dl_a25(:,3), abs(dl_a25(:,7)), 'x');
% plot(traffic, abs(mean_dl_a15), '-x');
% plot(dl_a15(:,3), abs(dl_a15(:,7)), 'x');
% plot(traffic, abs(mean_dl_a35), '-x');
% plot(dl_a35(:,3), abs(dl_a35(:,7)), 'x');

% %---- Plot bridge height as a function of the V height
% figure(4)
% hold on
% d=sortrows(data(data(:,10) == 12.1, :),2)
% d_left = d(:, 8);
% d_right = d(:, 7);
% h = d(:, 2);
% plot(h, abs(d_left), '-x');
% plot(h, abs(d_right), '-x');
% axis([ 4 10 0 5])
% grid on
% ylabel('left/right bridge height [Body length unit]')
% xlabel('V-depth [Body length unit] ')
% legend('Left side of the bridge', 'Right side of the bridge');

%===============================
% %---- Plot bridge height as a function of the V angle, given traffic,
% averaged on V-height
%===============================
figure(4)
h = 2:1:12;
i=0;
data_a30 = data(data(:,1) == 30, :);
data_a20 = data(data(:,1) == 20, :);
data_a15 = data(data(:,1) == 15, :);

for i=1:length(traffic)
    tf = traffic(i);
    d = data(data(:,3) == tf, :);
    d30 = data_a30(data_a30(:,3) == tf, :);
    d20 = data_a20(data_a20(:,3) == tf, :);
    d15 = data_a15(data_a15(:,3) == tf, :);
    data_mean_a30(i)= mean(d30(:,8));
    data_mean_a20(i)= mean(d20(:,8));
    data_mean_a15(i)= mean(d15(:,8));
    data_mean(i)= mean(d(:,8));
    data_mean_r(i)= mean(d(:,7));
    data_mean_s(i)= mean(d(d(:,4) == 1, 8));
    data_mean_r_s(i)= mean(d(d(:,4) == 1, 7));

end
% for i=1:length(traffic)
%     tf = traffic(i);
%      d = data(data(:,3) == tf, :);
%     data_mean(i)= mean(d(:,8));
%     data_mean_s(i)= mean(d(d(:,4) == 1, 8));
% end


hold on
plot(1./data(data(:,4)==0,3), abs(data(data(:,4)==0,8)), 'x','Color',[219/256,160/256,94/256]);
plot(1./data(data(:,4)==1,3), abs(data(data(:,4)==1,8)), 'o','Color',[219/256,160/256,94/256]);
plot(1./traffic, abs(data_mean_s), '-','Color',[239/256,149/256,68/256]);
plot(1./traffic, abs(data_mean), '--','Color',[239/256,149/256,68/256]);

plot(1./data(data(:,4)==0,3), abs(data(data(:,4)==0,7)), 'x','Color',[87/256,172/256,195/256]);
plot(1./data(data(:,4)==1,3), abs(data(data(:,4)==1,7)), 'o','Color',[87/256,172/256,195/256]);
plot(1./traffic, abs(data_mean_r_s), '-','Color',[87/256,150/256,195/256]);
plot(1./traffic, abs(data_mean_r), '--','Color',[87/256,150/256,195/256]);
% plot(traffic, abs(data_mean_a30), '-o');
% plot(traffic, abs(data_mean_a20), '-o');
% plot(traffic, abs(data_mean_a15), '-o');

grid on
ylabel('Right/left bridge height [Body length unit]')
xlabel('Traffic [Robot/s] ')
legend('Unstable bridge', 'Stable bridge', 'Average left bridge height (stable bridges)', 'Average left bridge height (all bridges)','Unstable bridge', 'Stable bridge', 'Average right bridge height (stable bridges)', 'Average right bridge height (all bridges)');

%=================================================

%=================================================
figure(5)
h = 9:2:21;
i=0;
data_d28 = data(data(:,3) == 2.8, :);
data_d28 = data_d28(data_d28(:,1) == 15, :);
data_d3 = data(data(:,3) == 3, :);
data_d35 = data(data(:,3) == 3.5, :);

hold on
plot(data_d28(:,2), abs(data_d28(:,8))/0.52, '-o');
%plot(data_d3(:,2), abs(data_d3(:,8)), '-o');
plot(data_d35(:,2), abs(data_d35(:,8))/0.52, '-o');
plot(data_d28(:,2), abs(data_d28(:,7))/0.52, '-o');
%plot(data_d3(:,2), abs(data_d3(:,7)), '-o');
plot(data_d35(:,2), abs(data_d35(:,7))/0.52, '-o');
axis([ 13 21 3 11])
ylabel('left/right bridge height [flipping distance unit]')
xlabel('V-depth [flipping distance unit] ')
legend('Left side of the bridge', 'Right side of the bridge');


%clear all;