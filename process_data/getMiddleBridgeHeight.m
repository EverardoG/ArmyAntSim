function [length] = getMiddleBridgeHeight(filepath, top_leftX, top_leftY, top_rightX, top_rightY, bottomX , bottomY, t_stability)
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
        fprintf('bottom = %f \n', bottomY);
        fprintf('d_mean = %f \n', top_leftY-bottomY);
        
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
        
        h=bottomY;

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
            
            if (bottomX < max_rX && bottomX > min_rX)
                m = min(r_Y(r_Y>0));
                if (m < h)
                h = m;
                end    
     
            end
        end

        dm = h - bottomY;

        fprintf('bottom = %f \n', bottomY);
        fprintf('d_mean = %f \n', dm);

    end
end
