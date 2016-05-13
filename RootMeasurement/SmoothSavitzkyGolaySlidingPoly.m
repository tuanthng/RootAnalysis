function [ smoothX  smoothY] = SmoothSavitzkyGolaySlidingPoly( biggestBlob )
% Display the binary image.
%subplot(2, 3, 3);
%imshow(biggestBlob, []);
%title('Binary Image', 'FontSize', fontSize, 'Interpreter', 'None');

% Get the boundaries.
% First, display the original gray scale image.
%subplot(2, 3, 4);
%imshow(grayImage, []);
%axis image; % Make sure image is not artificially stretched because of screen's aspect ratio.
%hold on;
% Now get the boundaries.
boundaries = bwboundaries(biggestBlob);
numberOfBoundaries = size(boundaries, 1);
for k = 1 : numberOfBoundaries
	thisBoundary = boundaries{k};
	plot(thisBoundary(:,2), thisBoundary(:,1), 'g', 'LineWidth', 2);
end
visualised = 0;
if (visualised == 1)
hold off;
caption = sprintf('%d Outlines, from bwboundaries()', numberOfBoundaries);
title(caption, 'FontSize', 14); 
axis on;
end

firstBoundary = boundaries{1};
% Get the x and y coordinates.
x = firstBoundary(:, 2);
y = firstBoundary(:, 1);

% Now smooth with a Savitzky-Golay sliding polynomial filter
windowWidth = 35;%45 %35
polynomialOrder = 3;%2%3%2
smoothX = sgolayfilt(x, polynomialOrder, windowWidth);
smoothY = sgolayfilt(y, polynomialOrder, windowWidth);
if (visualised == 1)
% % First, display the original gray scale image.
figure;
% subplot(2, 3, 5);
 imshow(biggestBlob, []);
 axis image; % Make sure image is not artificially stretched because of screen's aspect ratio.
hold on;
 plot(smoothX, smoothY, 'r-', 'LineWidth', 2);
% grid on;
% title('Smoothed Signal', 'FontSize', fontSize);
end

end

