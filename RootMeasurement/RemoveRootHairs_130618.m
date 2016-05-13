clear all
close all
clc

% This code means to remove remain root hairs after segmented by level set
% 
%%
TEMPLATE_TYPE = 'Test'; % 'Equal' 'Origin' 'Test'
VIDEO_SEQ = 'Antoine 8';
saveContour = true;
imageTypeLoad = 'png';
imageTypeSave = 'png';
contourFileType = 'txt';

START = 176;
END = 176;
offset = 1;
visualised = 0;

prefixLoad = 'Image_';
suffixLoad = '_seg';
prefixNode = 'Image_Node_';
prefixNode_Txt_EndPoint = 'Image_Node_Txt_ep_';
prefixNode_Txt_BranchPoint = 'Image_Node_Txt_bp_';
prefixSkeleton = 'Image_Skeleton_';
prefixSkeletonOrigin = 'Image_Ske_Img_';
prefixSkeletonOriginMiddle = 'Image_Ske_Middle_Img_';
prefixRefinedSeg = 'Image_Refined_Seg_';
prefixRootSeg = 'Image_Root_Noise';

BASE_PATH = 'E:\Users\txn\Documents\MyProject\Data\130618 Axiostar ht Col-0 bent to left\ver1_3'; %ver1_3 copied from ver1_2
VIDEO_PATH = [BASE_PATH '\Seg\']; %stored in hard drive
%VIDEO_FULLPATH = [VIDEO_PATH VIDEO_SEQ '\'];

PATH_TO_SAVE_SEG = [BASE_PATH '\'];
PATH_TO_SAVE_CONTOUR = [BASE_PATH '\Contour\'];
PATH_TO_SAVE_THIN_IMG = [BASE_PATH '\Thin\'];
PATH_TO_SAVE_NODE_IMG = [BASE_PATH '\Node\'];
PATH_TO_SAVE_NODE_TXT = [BASE_PATH '\Node_TXT\'];
PATH_TO_SAVE_THIN_ORIGIN = [BASE_PATH '\Thin_Origin\'];
PATH_TO_SAVE_THIN_ORIGIN_MIDDLE = [BASE_PATH '\Thin_Origin_Middle\'];
PATH_TO_SAVE_REFINED_SEG = [BASE_PATH '\RefinedSeg\'];
PATH_TO_SAVE_ROOT_NOISE = [BASE_PATH '\RootNoise\'];

%create folder if not existed
if exist(PATH_TO_SAVE_SEG, 'dir')
    %warning(....)
else
    mkdir(PATH_TO_SAVE_SEG)
end
if exist(PATH_TO_SAVE_CONTOUR, 'dir')
    %warning(....)
else
    mkdir(PATH_TO_SAVE_CONTOUR)
end
if exist(PATH_TO_SAVE_NODE_TXT, 'dir')
    %warning(....)
else
    mkdir(PATH_TO_SAVE_NODE_TXT)
end
if exist(PATH_TO_SAVE_THIN_IMG, 'dir')
    %warning(....)
else
    mkdir(PATH_TO_SAVE_THIN_IMG)
end
if exist(PATH_TO_SAVE_NODE_IMG, 'dir')
    %warning(....)
else
    mkdir(PATH_TO_SAVE_NODE_IMG)
end
if exist(PATH_TO_SAVE_THIN_ORIGIN, 'dir')
    %warning(....)
else
    mkdir(PATH_TO_SAVE_THIN_ORIGIN)
end
if exist(PATH_TO_SAVE_THIN_ORIGIN_MIDDLE, 'dir')
    %warning(....)
else
    mkdir(PATH_TO_SAVE_THIN_ORIGIN_MIDDLE)
end
if exist(PATH_TO_SAVE_REFINED_SEG, 'dir')
    %warning(....)
else
    mkdir(PATH_TO_SAVE_REFINED_SEG)
end
if exist(PATH_TO_SAVE_ROOT_NOISE, 'dir')
    %warning(....)
else
    mkdir(PATH_TO_SAVE_ROOT_NOISE)
end
% http://www.mathworks.co.uk/help/images/morphological-filtering.html

%%
%load all images
img_dir = dir([VIDEO_PATH '*.' imageTypeLoad]);
%limit = size(img_dir);
totalSize = size(img_dir);
limit = END - START + 1;
disp(['Total images for testing ' num2str(limit)]);

%% 

red = uint8([255 0 0]); 
green = uint8([0 255 0]);
blue = uint8([0 0 255]);
yellow = uint8([0 255 255]);

seedPoint = [ 40 750]; %y and x 

smoothEdge = 1;
refineBoundary = 1;

% For each image (segmented),
for indexImage = START:offset:END
    % load image
    fullname = strcat(VIDEO_PATH, prefixLoad, num2str(indexImage), suffixLoad, '.', imageTypeLoad);
    
    if (~exist(fullname, 'file'))
        disp(['  Not exist: ' fullname]);
        continue;
    else
        disp(['Processing: ' fullname]); 
    end
    
    imageSource = imread(fullname);
    binaryImageSource = im2bw(imageSource, 0);
    if (visualised == 1)
        imshow(binaryImageSource);
    end
        
    % Extract the largest area using our custom function ExtractNLargestBlobs().
    numberToExtract = 1;
    largestBlob = ExtractNLargestBlobs(binaryImageSource, numberToExtract);
    
    %figure;
    %imshow(largestBlob);
    %keep the largest blob
    imageSourceRoot = imageSource;
    imageSourceRoot(largestBlob == 0) = 0;
    imageSource = imageSourceRoot;
    if (visualised == 1)
        figure;
        imshow(imageSourceRoot);
    end
    
    %save the root
     fullnameRootNoise = strcat(PATH_TO_SAVE_ROOT_NOISE, prefixRootSeg, num2str(indexImage), '.', imageTypeSave);
     imwrite(imageSource, fullnameRootNoise);
    
    %smooth the boundary of the root
    %SmoothSavitzkyGolaySlidingPoly(imageSource);
    
    %binaryImageSource = imfill(binaryImageSource, 'holes');
    %figure;
    %imshow(binaryImageSource);
    
    if (smoothEdge == 1)

        se = strel('disk',20);
        erodedBW = imerode(imageSource,se);

        se = strel('disk',10);
        erodedBW = imerode(erodedBW,se);

        se = strel('disk',5);
        erodedBW = imerode(erodedBW,se);
        erodedBW = imerode(erodedBW,se);
        erodedBW = imerode(erodedBW,se);
        se = strel('disk',3);
        erodedBW = imerode(erodedBW,se);
        erodedBW = imerode(erodedBW,se);
        erodedBW = imerode(erodedBW,se);
        
        if (visualised == 1)
            figure;
            imshow(erodedBW);
        end
               
        binaryImageSource = im2bw(erodedBW, 0);
        
        if (refineBoundary == 1)
            % Segment the object:
            Object=~binaryImageSource;
            %Object=binaryImageSource;
            if (visualised == 1)
                figure;
                imshow(Object);
            end

            BW = bwconvhull(Object, 'objects');
            Mask=repmat(BW,[1,1,1]);
            
            % Iterate opening operation:
            Interp=binaryImageSource;
            for k=1:5
                %Interp=imopen(Interp, strel('disk', 5));
                Interp=imdilate(Interp, strel('disk', 5));
            end
            if (visualised == 1)
                figure;
                imshow(Interp);
            end
            % Keep original pixels, add the newly generated ones and smooth the output:
            Interpolated(:,:,1)=medfilt2(imadd(binaryImageSource(:,:,1).*Object, Interp(:,:,1).*~(BW==Object)), [4 4]);
            
            % Display the results:
            Masked=imadd(Interpolated.*im2double(Mask), im2double(~Mask));
             if (visualised == 1)
            figure;
            imshow(Masked);
             end
             
            binaryImageSource = Masked;
            
            %save refined boundary
            fullnameRefinedBoundary = strcat(PATH_TO_SAVE_REFINED_SEG, prefixRefinedSeg, num2str(indexImage), '.', imageTypeSave);
            imwrite(binaryImageSource, fullnameRefinedBoundary);
            
        end
    end
      
    
    skelImg   = bwmorph(binaryImageSource, 'thin', 'inf');
    skelImg   = bwmorph(skelImg, 'skel', 'inf');

    fullnameSkeleton = strcat(PATH_TO_SAVE_THIN_IMG, prefixSkeleton, num2str(indexImage), '.', imageTypeSave);
    imwrite(skelImg, fullnameSkeleton);
    
    if (visualised == 1)
        h = figure;
        imshow(skelImg);
    end

    % find all branches
    branchImg = bwmorph(skelImg, 'branchpoints');
    endImg    = bwmorph(skelImg, 'endpoints');

    [row, column] = find(endImg);
    endPts        = [row column];
        
    sizePoints = size(endPts);
    
    %circlesEndPoint(:,1,:) = endPts(:, 2); % colum x
    %circlesEndPoint(:,2,:) = endPts(:, 1); % colum y
    temp = ones(sizePoints(1), 1) * 3;
    circlesEndPoint = [endPts(:, 2) endPts(:, 1) temp];
    
    fullnameNode_endpoints = strcat(PATH_TO_SAVE_NODE_TXT, prefixNode_Txt_EndPoint, num2str(indexImage), '.txt');
    dlmwrite(fullnameNode_endpoints, circlesEndPoint);
    
    [row, column] = find(branchImg);
    branchPts     = [row column];
    
    sizePoints = size(branchPts);
    circlesBranchPoint = ones(sizePoints(1), 3) * 2;
    circlesBranchPoint(:,1,:) = branchPts(:, 2); % colum x
    circlesBranchPoint(:,2,:) = branchPts(:, 1); % colum y
    %temp = ones(xy(1), 1) * 3;
    %circlesBranchPoint = [branchPts(:, 2) branchPts(:, 1) temp]; % colum x
    
    fullnameNode_branchpoints = strcat(PATH_TO_SAVE_NODE_TXT, prefixNode_Txt_BranchPoint, num2str(indexImage), '.txt');
    dlmwrite(fullnameNode_branchpoints, circlesBranchPoint);
    
    if (visualised == 1)
        hold on
        plot(endPts(:,2),endPts(:,1),'go')
        plot(branchPts(:,2),branchPts(:,1),'ro')
        hold off
    end
     
    %save to image
    grayImage = 255 * uint8(skelImg);
    RGB = repmat(grayImage,[1,1,3]);
    %circles = int32([304, 1, 2]);
    shapeInserter = vision.ShapeInserter('Shape','Circles','BorderColor','Custom','CustomBorderColor',green);
    J = step(shapeInserter, RGB, int32(circlesEndPoint));
    
    shapeInserter = vision.ShapeInserter('Shape','Circles','BorderColor','Custom','CustomBorderColor',red);
    J = step(shapeInserter, J, int32(circlesBranchPoint));
    %rectangle('Position',circlesEndPoint,'Curvature',[1,1]); % could be
    %used to draw an circle/ellipse
    
    fullnameNode = strcat(PATH_TO_SAVE_NODE_IMG, prefixNode, num2str(indexImage), '.', imageTypeSave);
    imwrite(J, fullnameNode);

     if (visualised == 1)
    figure;
    imshow(J); 
     end
    %overlap skeleton with the orginal image
    combinedBM = imadd(imageSource,grayImage,'uint8');
    if (visualised == 1)
        figure;
        imshow(combinedBM,[]);
    end
     
    fullnameSkeImg = strcat(PATH_TO_SAVE_THIN_ORIGIN, prefixSkeletonOrigin, num2str(indexImage), '.', imageTypeSave);
    imwrite(combinedBM, fullnameSkeImg);
    
    %%%%%%%%%%%%%%%%%%%%%%
    %Or using below, C code with matlab functions
    %[dmap,exy,jxy] = anaskel(skelImg);
    %hold on
    %plot(exy(1,:),exy(2,:),'go')
    %plot(jxy(1,:),jxy(2,:),'ro')
    %%%%%%%%%%%%%%%%%%%%%%
    
    % find the longest branch
    % 1. Find the start point from the seed point
    % 2. For each branch point, find the longest distance from the start
    % point
    %[startPoint, distClosest] = FindClosestPoint(branchPts, seedPoint);
    [startPoint, distClosest] = FindClosestPoint(endPts, seedPoint);
    
    %testing display seed points and its closest point
    %circlesStartEndPoint(1, :) = [seedPoint(:, 2) seedPoint(:, 1) temp(1,1)];
    %circlesStartEndPoint(2, :) = [startPoint(:, 2) startPoint(:, 1) temp(1,1)];
    %shapeInserter = vision.ShapeInserter('Shape','Circles','BorderColor','Custom','CustomBorderColor',yellow);
    %startEndImg = step(shapeInserter, RGB, int32(circlesStartEndPoint));
    %figure;
    %imshow(startEndImg); 
    
    sizePoints = size(endPts);
    %sizePoints = size(branchPts);
    currentPath = 0;
    currentDistance = 0;
    
    for indexPoint = 1:sizePoints(1)
        [dist, path] = FindPath(skelImg, startPoint, endPts(indexPoint, :));
        %[dist, path] = FindPath(skelImg, startPoint, branchPts(indexPoint, :));
        
        if (dist > currentDistance && dist ~= inf)
            currentDistance = dist;
            currentPath = path;
        end
    end
            
    %testing: display the longest path
    P = imoverlay(skelImg, imdilate(currentPath, ones(3,3)), [1 0 0]);
    %P = imoverlay(skelImg, currentPath);
    if (visualised == 1)
        figure;
        imshow(P, 'InitialMagnification', 200);
    end
     
    combinedBM = imadd(imageSource, P(:,:,1), 'uint8');
     if (visualised == 1)
    figure;
    imshow(combinedBM,[]);
     end
    fullnameSkeImgMiddle = strcat(PATH_TO_SAVE_THIN_ORIGIN_MIDDLE, prefixSkeletonOriginMiddle, num2str(indexImage), '.', imageTypeSave);
    imwrite(combinedBM, fullnameSkeImgMiddle);
    
end

disp('All processed!');
%%



