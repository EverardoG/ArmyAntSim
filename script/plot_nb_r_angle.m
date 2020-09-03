filepath1 = '../experiments/delay_025_no_gripp_area_2019-03-02_22-43/results/nb_robots.txt'
filepath2 = '../experiments/delay_025_no_gripp_area_2019-03-02_22-43/results/nb_robots.txt'
data1=dlmread(filepath1, ';');
data2=dlmread(filepath2, ';');
data = [data1; data2];

% filepath1 = '../experiments/general_delay_2019-01-14_09-27/results/nb_robots.txt'
% data=dlmread(filepath1, ';');
%Data is organized as following: 
%data{:,1} V-angle; 
%data{:,2} V-height;
%data{:,3} robot distance/delay; 
%data{:,4} stability; 
%data{:,5} nb_robots;

data = unique(data,'rows');

angle = min(data(:,1)):5:max(data(:,1));
height = min(data(:,2)):max(data(:,2));
dist = min(data(:,3)):0.5:max(data(:,3));
distance =3.5;

[X,Y] = meshgrid(angle,height);
n=zeros(length(height),length(angle));
s=zeros(length(height),length(angle));
[sz1,sz2] = size(data);
[s1,s2] = size(angle);
n11=zeros(s2,1);
n10=zeros(s2,1);
n20=zeros(s2,1);
n22=zeros(s2,1);
n9=zeros(s2,1);
n8=zeros(s2,1);
n7=zeros(s2,1);
n6=zeros(s2,1);
n5=zeros(s2,1);
n4=zeros(s2,1);
h22=22.0;
h20=20.0;
h11=11.0;
h10=10.0;
h9=9.0;
h8=8.0;
h7=7.0;
h6=6.0;
h5=5.0;
h4=4.0;

for i = 1:sz1
    if(data(i,3)==distance)
        x = data(i,1);
        y = data(i,2);
        k = find(angle==x);
        l = find(height==y);
        n(l,k) = data(i,5);
        s(l,k) = data(i,4);
        if data(i,2) == h11
            n11(k) = data(i,5);
        elseif data(i,2) == h10
            n10(k) = data(i,5);
        elseif data(i,2) == h9
            n9(k) = data(i,5);
        elseif data(i,2) == h8
            n8(k) = data(i,5);
        elseif data(i,2) == h7
            n7(k) = data(i,5);
        elseif data(i,2) == h6
            n6(k) = data(i,5);
        elseif data(i,2) == h5
            n5(k) = data(i,5);
        elseif data(i,2) == h4
            n4(k) = data(i,5);
        elseif data(i,2) == h20
            n20(k) = data(i,5); 
        elseif data(i,2) == h22
            n22(k) = data(i,5);          
        end
    end
end
%----------- meshmap of the nb of robots
figure(1)
colormap hot
pcolor(Y,X,n);
%legend("No stable bridge")
ylabel('V-Angle [Deg]')
xlabel('V-Height [Body length]')
hcb=colorbar
title(hcb,'nb of robots in the bridge')

%----------- meshmap of the nb of robots
figure(2)
colorMap = [0 0 0; 1 1 1]
colormap(colorMap)
pcolor(Y,X,s);
%legend("No stable bridge")
ylabel('V-Angle [Deg]')
xlabel('V-Height [Body length]')
legend('Stable bridge')

% %----------- ration=f(width) for different heights
figure(3)
plot(angle,n11)
hold on 
plot(angle,n10)
plot(angle,n9)
plot(angle,n8)
plot(angle,n7)
plot(angle,n6)
plot(angle,n5)
plot(angle,n4)
plot(angle,n20)
plot(angle,n22)
ylabel('nb of robots in the bridge')
xlabel('V-Width [Body length]')
legend('Fixed V-height of 11 BL', 'Fixed V-height of 10 BL','Fixed V-height of 9 BL', 'Fixed V-height of 8 BL','Fixed V-height of 7 BL', 'Fixed V-height of 6 BL','Fixed V-height of 5 BL', 'Fixed V-height of 4 BL', 'Fixed V-height of 20 BL', 'Fixed V-height of 22 BL')

% %----------- removed when n<2
% figure(3)
% indices = find(n11<2);
% n11(indices) = [];
% w11=width;
% w11(indices) = [];
% 
% indices = find(n10<2);
% n10(indices) = [];
% w10=width;
% w10(indices) = [];
% 
% indices = find(n8<2);
% n8(indices) = [];
% w8=width;
% w8(indices) = [];
% 
% indices = find(n6<2);
% n6(indices) = [];
% w6=width;
% w6(indices) = [];
% 
% indices = find(n4<2);
% n4(indices) = [];
% w4=width;
% w4(indices) = [];
% 
% plot(w11,n11)
% hold on 
% plot(w10,n10)
% plot(w8,n8)
% plot(w6,n6)
% plot(w4,n4)
% ylabel('nb of robots in the bridge')
% xlabel('V-Width [Body length]')
% legend('Fixed V-height of 11 BL', 'Fixed V-height of 10 BL', 'Fixed V-height of 8BL', 'Fixed V-height of 6 BL', 'Fixed V-height of 4 BL')
% 
% %----------- ration=f(angle) for different heights
% figure(3)
% a11=atan(w11./(2.*h11)).*180/pi;
% a10=atan(w10./(2.*h10)).*180/pi;
% a8=atan(w8./(2.*h8)).*180/pi;
% a6=atan(w6./(2.*h6)).*180/pi;
% a4=atan(w4./(2.*h4)).*180/pi;
% plot(a11,n11)
% hold on 
% plot(a10,n10)
% plot(a8,n8)
% plot(a6,n6)
% plot(a4,n4)
% ylabel('nb of robots in the bridge')
% xlabel('V-Angle [Deg]')
% legend('Fixed V-height of 11 BL', 'Fixed V-height of 10 BL', 'Fixed V-height of 8BL', 'Fixed V-height of 6 BL', 'Fixed V-height of 4 BL')
