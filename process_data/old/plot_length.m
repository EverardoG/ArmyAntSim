filepath = '../experiments/general_2018-12-01_12-57/results/length_dist_1.5.txt'
data=dlmread(filepath, ';');

%Data is organized as following: 
%data{:,1} V-width; 
%data{:,2} V-height;
%data{:,3} initial length; 
%data{:,4} bridge length; 

r=data(:,4)./data(:,1);
A= data(:,1).*data(:,2);
data = unique(data,'rows');

width = min(data(:,1)):max(data(:,1));
height = min(data(:,2)):max(data(:,2));
[X,Y] = meshgrid(width,height);
[s1,s2] = size(width);
l=zeros(s2,s2);
[sz1,sz2] = size(data);
l11=zeros(s2,1);
l10=zeros(s2,1);
l8=zeros(s2,1);
l6=zeros(s2,1);
l4=zeros(s2,1);
h11=11.0;
h10=10.0;
h8=8.0;
h6=6.0;
h4=4.0;

for i = 1:sz1
    x = data(i,1);
    y = data(i,2);
    l(y-1,x-1) = r(i);  
    if data(i,2) == h11
        l11(x-1) = r(i);
    elseif data(i,2) == h10
        l10(x-1) = r(i);
    elseif data(i,2) == h8
        l8(x-1) = r(i);
    elseif data(i,2) == h6
        l6(x-1) = r(i);
    elseif data(i,2) == h4
        l4(x-1) = r(i);
    end
end

%----------- meshmap of the length
figure(1)
colormap default
pcolor(X,Y,l);
%legend("No stable bridge")
xlabel('V-Width [Body length]')
ylabel('V-Height [Body length]')

%----------- ration=f(width) for different heights
figure(2)
plot(width,l11)
hold on 
plot(width,l10)
plot(width,l8)
plot(width,l6)
plot(width,l4)
xlabel('ratio l/l0')
ylabel('V-Width [Body length]')
legend('Fixed V-height of 11 BL', 'Fixed V-height of 10 BL', 'Fixed V-height of 8BL', 'Fixed V-height of 6 BL', 'Fixed V-height of 4 BL')

%----------- ration=f(angle) for different heights
figure(3)
a11=atan(width./(2.*h11)).*180/pi;
a10=atan(width./(2.*h10)).*180/pi;
a8=atan(width./(2.*h8)).*180/pi;
a6=atan(width./(2.*h6)).*180/pi;
a4=atan(width./(2.*h4)).*180/pi;
plot(a11,l11)
hold on 
plot(a10,l10)
plot(a8,l8)
plot(a6,l6)
plot(a4,l4)
ylabel('ratio l/l0')
xlabel('V-Angle [Deg]')
legend('Fixed V-height of 11 BL', 'Fixed V-height of 10 BL', 'Fixed V-height of 8BL', 'Fixed V-height of 6 BL', 'Fixed V-height of 4 BL')
