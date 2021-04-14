function [length] = getBridgeLength(filepath, top_leftX, top_leftY, top_rightX, top_rightY, bottomX , bottomY, t_stability)
% filepath='../experiments/delay_gauss_025_2019-02-19_13-26/angle_17.5_height_8_delay_2.25/exp_bridge.txt'
% top_rightX=10.017656;
% top_leftY=2.500000;
% top_leftX=7.140000;
% top_rightY=2.500000;
% bottomX=8.578828; 
% bottomY=10.660000;
% t_stability = 7.283334;
    %Data is organized as following: 
    %data{:,1} Timestamp; 
    %data{:,2} robot ID;
    %data{:,3} x coordinate; 
    %data{:,4} y coordinate; 
    %data{:,5} angle; 
    %data{:,6} current joint x; 
    %data{:,7} current joint y; 
    %data{:,8} previous joint x; 
    %data{:,9} previous joint y; 
    %data{:,10} it entry; 
    %data{:,11} age 
    data=dlmread(filepath, ';', 1, 0);
    bl=1.02;

    t_stability = int16((t_stability+1)*60);
    data_f = data(data(:,1) < t_stability,:);
    
    [sz1,sz2] = size(data_f);
    
    if(sz1 ==0)
        fprintf('length = %f \n', top_rightX-top_leftX);
        fprintf('bottom = %f \n', bottomY);
        fprintf('d_mean = %f \n', top_leftY-bottomY);
        fprintf('d_left = %f \n', top_leftY-bottomY);
        fprintf('d_right = %f \n', top_leftY-bottomY);
        
    else
        
        it= data_f(sz1,1);
        r = double.empty(sz2,0);
        i = sz1;   
        while (data_f(i,1) == it)
                r = [r data_f(i,:)'];
                i = i-1;
                if i == 0
                        break
                end
        end

    %     plot(r(3,:),-r(4,:), 'b-o')
    %     hold on 
    %     x = [17.340000 12.240000 7.140000]/body_l
    %     y = [2.500000 10.660000 2.500000]/body_l
    %     plot(x,-y, 'k')
    %     
    %     x1=[9.172432 14.314850]/body_l
    %     y1=[5.747175 7.335523]/body_l
    % 
    %     plot(x1, -y1, 'r-o')
        x_left = top_rightX;
        y_left = top_rightY;
        x_right = top_leftX;
        y_right = top_leftY;
        left_attach = false;
        right_attach = false;

        [s1,s2] = size(r);
        for i=1:s2
            r_X = [r(3,i) r(6,i) r(8,i) r(3,i)+bl/2*cos(r(5,i)) r(3,i)-bl/2*cos(r(5,i))];
            r_Y = [r(4,i) r(7,i) r(9,i) r(4,i)-bl/2*sin(r(5,i)) r(4,i)+bl/2*sin(r(5,i))];
            while (min(r_X) == 0)
               [min_rX, i_0]= min(r_X); 
               r_X(i_0)=[];
               r_Y(i_0)=[];
            end
            [min_rX, i_min]= min(r_X);
            [max_rX, i_max]= max(r_X);
            
            if (min_rX < x_left)
                x_left = min_rX;
                y_left = r_Y(i_min);
                if(i==4)
                    y_left
                    t=r_Y(i_min)
                end
                left_attach = true;
            end    
            if max_rX > x_right
                x_right = max_rX;
                y_right = r_Y(i_max);
                right_attach = true;
            end      
        end

    %     
    %     %line1
    %     x1  = [top_leftX bottomX];
    %     y1  = [top_leftY bottomY];
    %     %line2
    %     x2 = [x_left x_left+cos(a_left)];
    %     y2 = [y_left y_left+sin(a_left)];
    %     %calculate intersection
    %     [xi_left, yi_left] = polyxpoly(x1, y1, x2, y2, 'unique')
    %     
    %     %line1
    %     x1  = [top_rightX bottomX];
    %     y1  = [top_rightY bottomY];
    %     %line2
    %     x2 = [x_right x_left+cos(a_right)];
    %     y2 = [y_right y_left+sin(a_right)];
    %     %calculate intersection
    %     [xi_right, yi_right] = polyxpoly(x1, y1, x2, y2, 'unique')

        length = sqrt((x_left-top_leftX)^2 + (y_left-top_leftY)^2);
        length = length + sqrt((x_left-x_right)^2 + (y_left-y_right)^2);
        length = length + sqrt((x_right-top_rightX)^2 + (y_right-top_rightY)^2);

        vX=[top_leftX bottomX top_rightX];
        vY=[top_leftY bottomY top_rightY];
        bX=[top_leftX x_left x_right top_rightX];
        bY=[top_leftY y_left y_right top_rightY]; 
        fprintf('length = %f \n', length);

        if(right_attach == false)
            y_right = bottomY;
        end

        if(left_attach == false)
            y_left = bottomY;
        end

        dl = y_left - bottomY;
        dr = y_right - bottomY;
        dm = (y_right+y_left)/2 - bottomY;

        fprintf('bottom = %f \n', bottomY);
        fprintf('d_mean = %f \n', dm);
        fprintf('d_left = %f \n', dl);
        fprintf('d_right = %f \n', dr);

        fig = figure('visible','off');
        plot(vX,vY, '-o');
        axis equal
        hold on
        plot(bX,bY, 'm');

        [folder,name,ext] = fileparts(filepath);
        title = fullfile(folder, 'bridge_shape.jpg');
        saveas(fig,title);
    end
end
