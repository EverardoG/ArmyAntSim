filepath = '../experiments/general_2018-12-10_17-30/results/stable.txt'
stable=dlmread(filepath, ';');

%Generate the X and Y grid arrays using the MESHGRID function.
m_w = min(stable(:,1))
m_h = min(stable(:,2))
width = min(stable(:,1)):max(stable(:,1));
height = min(stable(:,2)):max(stable(:,2));
[X,Y] = meshgrid(width,height);
[s1,s2] = size(width)
S=zeros(s2,s2);

%Note that size(Z) is the same as size(x) and size(y)
for i = 1:121
    w = stable(i,1)-m_w+1;
    h = stable(i,2)-m_h+1;
    S(h,w) = stable(i,3);
end
%use the user defined colormap for figure.
colormap(gray(2))
%plot the figure
figure(1)
pcolor(X,Y,S);
legend("Stable bridge")
xlabel('V-Width [Body length]')
ylabel('V-Height [Body length]')

%plot the figure for angles
% figure(2)
% A = stable(:,1)./(2.*stable(:,2));
% data = unique(A,'rows');
% [X2,Y2] = meshgrid(A,height);
% pcolor(X2,Y2,S);
% legend("Stable bridge")
% xlabel('V-Angle [Deg]')
% ylabel('V-Height [Body length]')