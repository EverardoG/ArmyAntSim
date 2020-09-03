filepath = '../experiments/traffic_2019-01-03_10-09/results/nb_robots.txt'
data=dlmread(filepath, ';');

%Data is organized as following: 
%data{:,1} V-angle; 
%data{:,2} V-height;
%data{:,3} robot distance/delay; 
%data{:,4} stability; 
%data{:,5} nb_robots;

data = unique(data,'rows');
distance =2.5;

% %----------- ration=f(width) for different heights
figure(1)
plot(data(:,3), data(:,4))
axis([1 3 0 2])
ylabel('nb of robots in the bridge')
xlabel('V-Width [Body length]')


figure(2)
plot(data(2:end,3), data(2:end,5))
ylabel('nb of robots in the bridge')
xlabel('V-Width [Body length]')