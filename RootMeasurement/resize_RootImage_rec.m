clear all
clc
close all

%%
TEMPLATE_TYPE = 'Equal'; % 'Equal' 'Origin'

imageTypeLoad = 'bmp';
imageTypeSave = 'png';

VIDEO_PATH= 'E:\Users\txn\Documents\MyProject\Data\151117 Col ht timelapse I flipped plate bent right way';

VIDEO_FULLPATH = [VIDEO_PATH  '\'];

PATH_TO_SAVE = [VIDEO_PATH  '\' 'Modified2/'];

%create folder if not existed
if exist(PATH_TO_SAVE, 'dir')
   %warning(....)
else
  mkdir(PATH_TO_SAVE)
end

%%
START = 1;
END = 210;
offset = 1;

prefix = 'Image_';

%150930 Col ht timelapse I straight
%croppedArea = [1 650 2451 (1350-650)] ; % x y w h
%151027 Col ht timelapse I flipped plate bent right way
%croppedArea = [1 540 2451 (1510-540)] ; % x y w h 
%151110 Col no sorbitol timelapse I flipped plate straight
%croppedArea = [1 690 2200 (1210-690)] ; % x y w h
%130814 Axiostar Col-0 ht bent to left
%croppedArea = [520 1 (1790-520) (1939-1)] ; % x y w h
%151117 Col ht timelapse I flipped plate bent right way
croppedArea = [1 681 (2260-1) (1500-681)] ; % x y w h
%151117 Col ht timelapse II flipped plate bent right way
%croppedArea = [1 711 (1820-1) (2040-711)] ; % x y w h

disp('Cropping images...');

for index = START:offset:END
    
    im = imread(strcat(VIDEO_FULLPATH, prefix, num2str(index), '.', imageTypeLoad));
    %imagesc(im);
    %pause;
    
    croppedImage = im(croppedArea(2):croppedArea(2) + croppedArea(4), croppedArea(1):croppedArea(1) + croppedArea(3));
    
    %colormap('gray');
    %imagesc(croppedImage);
    
    %save cropped image
    imwrite(croppedImage, strcat(PATH_TO_SAVE, prefix, num2str(index), '.', imageTypeSave));
    
end
disp('Finished!');