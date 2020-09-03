filepath = '../experiments/general_2018-12-01_12-57/results/nb_robots.txt'
data=dlmread(filepath, ';');

%Data is organized as following: 
%data{:,1} V-width; 
%data{:,2} V-height;
%data{:,3} robot distance; 
%data{:,4} stability; 
%data{:,5} nb_robots;

data = unique(data,'rows');

width = min(data(:,1)):max(data(:,1));
height = min(data(:,2)):max(data(:,2));
dist = min(data(:,3)):0.5:max(data(:,3));

[X,Y] = meshgrid(width,height);
[s1,s2] = size(width);
n=zeros(s2,s2);
[sz1,sz2] = size(data);
n11=zeros(s2,1);
n10=zeros(s2,1);
n8=zeros(s2,1);
n6=zeros(s2,1);
n4=zeros(s2,1);
h11=11.0;
h10=10.0;
h8=8.0;
h6=6.0;
h4=4.0;

for i = 1:sz1
    if(data(i,3)==2)
        x = data(i,1);
        y = data(i,2);
        n(y-1,x-1) = data(i,5);  
        if data(i,2) == h11
            n11(x-1) = data(i,5);
        elseif data(i,2) == h10
            n10(x-1) = data(i,5);
        elseif data(i,2) == h8
            n8(x-1) = data(i,5);
        elseif data(i,2) == h6
            n6(x-1) = data(i,5);
        elseif data(i,2) == h4
            n4(x-1) = data(i,5);
        end
    end
end
%----------- meshmap of the length
figure(1)
colormap default
pcolor(X,Y,n);
%legend("No stable bridge")
xlabel('V-Width [Body length]')
ylabel('V-Height [Body length]')

%----------- ration=f(width) for different heights
figure(2)
plot(width,n11)
hold on 
plot(width,n10)
plot(width,n8)
plot(width,n6)
plot(width,n4)
ylabel('nb of robots in the bridge')
xlabel('V-Width [Body length]')
legend('Fixed V-height of 11 BL', 'Fixed V-height of 10 BL', 'Fixed V-height of 8BL', 'Fixed V-height of 6 BL', 'Fixed V-height of 4 BL')

%----------- removed when n<2
figure(3)
indices = find(n11<2);
n11(indices) = [];
w11=width;
w11(indices) = [];

indices = find(n10<2);
n10(indices) = [];
w10=width;
w10(indices) = [];

indices = find(n8<2);
n8(indices) = [];
w8=width;
w8(indices) = [];

indices = find(n6<2);
n6(indices) = [];
w6=width;
w6(indices) = [];

indices = find(n4<2);
n4(indices) = [];
w4=width;
w4(indices) = [];

plot(w11,n11)
hold on 
plot(w10,n10)
plot(w8,n8)
plot(w6,n6)
plot(w4,n4)
ylabel('nb of robots in the bridge')
xlabel('V-Width [Body length]')
legend('Fixed V-height of 11 BL', 'Fixed V-height of 10 BL', 'Fixed V-height of 8BL', 'Fixed V-height of 6 BL', 'Fixed V-height of 4 BL')

%----------- ration=f(angle) for different heights
figure(3)
a11=atan(w11./(2.*h11)).*180/pi;
a10=atan(w10./(2.*h10)).*180/pi;
a8=atan(w8./(2.*h8)).*180/pi;
a6=atan(w6./(2.*h6)).*180/pi;
a4=atan(w4./(2.*h4)).*180/pi;
plot(a11,n11)
hold on 
plot(a10,n10)
plot(a8,n8)
plot(a6,n6)
plot(a4,n4)
ylabel('nb of robots in the bridge')
xlabel('V-Angle [Deg]')
legend('Fixed V-height of 11 BL', 'Fixed V-height of 10 BL', 'Fixed V-height of 8BL', 'Fixed V-height of 6 BL', 'Fixed V-height of 4 BL')
