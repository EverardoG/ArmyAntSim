filepath = '../experiments/general_2018-12-13_16-35/results/stable.txt'
stable = dlmread(filepath, ';');
stable = unique(stable,'rows');

%Generate the X and Y grid arrays using the MESHGRID function.
m_a = min(stable(:,1));
m_h = min(stable(:,2));
angle = min(stable(:,1)):5:max(stable(:,1));
height = min(stable(:,2)):max(stable(:,2));
[X,Y] = meshgrid(angle,height);
[s1,s2] = size(stable);
S=zeros(length(height),length(angle));

distance=2.0;
%Note that size(Z) is the same as size(x) and size(y)
for i = 1:s1
    if(stable(i,3)==distance)
        x = stable(i,1);
        y = stable(i,2);
        k = find(angle==x);
        l = find(height==y);
        S(l,k) = stable(i,4);  
    end
end
%use the user defined colormap for figure.
colormap(gray(2))
%plot the figure
figure(1)
pcolor(Y,X,S);
legend("Stable bridge")
ylabel('V-Angle [Deg]')
xlabel('V-Height [Body length]')

%plot the figure for angles
% figure(2)
% A = stable(:,1)./(2.*stable(:,2));
% data = unique(A,'rows');
% [X2,Y2] = meshgrid(A,height);
% pcolor(X2,Y2,S);
% legend("Stable bridge")
% xlabel('V-Angle [Deg]')
% ylabel('V-Height [Body length]')