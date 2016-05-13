function [distance, path] = FindPath(skelImage, startPoint, endPoint)
    
    %% starting calculating:
    %startPoint and endPoint are the 1x2 matrix which contains the y and x of the point
    %1. Compute the distance transform for just the upper left block of foreground pixels.
    %2. Compute the distance transform for just the lower right block of foreground pixels.
    %3. Add the two distance transforms together.
    %4. Round to lower precision.
    %5. The pixels in the regional minimum of the result lie along one or more of the shortest paths from one block to the other.
    
    D1 = bwdistgeodesic(skelImage, startPoint(2), startPoint(1), 'quasi-euclidean');
    D2 = bwdistgeodesic(skelImage, endPoint(2), endPoint(1), 'quasi-euclidean');
    
    D = D1 + D2;
    D = round(D * 8) / 8;
    D(isnan(D)) = inf;
    skeleton_path = imregionalmin(D);
    
    %return result
    path_length = D(skeleton_path);
    %path_length = path_length(1)
    distance = path_length(1);
    path = skeleton_path;
    
    %% testing: 
    %plot the startPoint and endPoint on the skeleton image and 
    %show the path
%     P = imoverlay(skelImage, imdilate(skeleton_path, ones(3,3)), [1 0 0]);
%     figure;
%     imshow(P, 'InitialMagnification', 200)
%     hold on
%     plot(startPoint(2), startPoint(1), 'g*', 'MarkerSize', 15)
%     plot(endPoint(2), endPoint(1), 'g*', 'MarkerSize', 15)
%     hold off
    
    
    
end