function [closetPoint, distance] = FindClosestPoint(pointList, seedPoint)

    sizePoints = size(pointList);
    
    listSeed = repmat(seedPoint, sizePoints(1), 1); %duplicate rows
    
    tempDiff = (listSeed - pointList).^2;
    
    tempDistance = sqrt(tempDiff(:, 1) + tempDiff(:, 2));
    
    [distance, index] = min(tempDistance(:));
    
    closetPoint(1,1) = pointList(index, 1);
    closetPoint(1,2) = pointList(index, 2);
end