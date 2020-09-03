filepath = '../experiments/general_2018-12-01_12-57/width_2.0_height_2.0_dist_1.5/exp_bridge.txt'
it = 1552;
function getBridgeLength(filepath, it, body_l, width, height)

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
    data=dlmread(filepath, ';', 2);
    
    [sz1,sz2] = size(data)
    r = double.empty(sz2,0);
    for i=1:sz1
        if data(i,1) == it
            r = [r data(i,:)'];
        elseif data(i,1) > it
            break
        end
    end
end
