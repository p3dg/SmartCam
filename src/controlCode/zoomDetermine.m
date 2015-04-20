% "zoomDetermine.m"
clear all
close all

% all the data we collected:
x = 620; % # pixels in x direction
y = 480; % # pixels in y direction
radius = 2.64;
zoom = [0 3005 10366 13998];
width = [2.44 1.74 0.60 0.28];
height = width*(y/x); % (no distortion on frame)

% derived data prepared:
widthD2 = width/2;
heightD2 = height/2;
phi1 = zeros(1, length(widthD2));
phi2 = zeros(1, length(heightD2));
for i = 1 : length(widthD2)
   phi1(i) = atan(widthD2(i)/radius);
   phi1(i) = radtodeg(phi1(i));
end
for j = 1 : length(heightD2)
   phi2(j) = atan(heightD2(j)/radius);
   phi2(j) = radtodeg(phi2(j));
end

% plot phi1 so I can see it:
figure(1)
plot(zoom, width, 'o')
xlabel('Zoom (arbitrary units in [0, 13996]')
ylabel('Width (m)')
title('width of frame as a function of zoom')

figure(2)
plot(zoom, phi1, 'o')
xlabel('Zoom (arbitrary units in [0, 13996])')
ylabel('phi1 (degrees)')
title('phi1 as a function of zoom')

% pause before plotting phi2...
fprintf('enter any value, then press enter for y direction stuff;\n');
pause = input('enter a 1 to close all windows:   ');
fprintf('\n');
if pause == 1
    close all
end

% plotting for phi2:
figure(3)
plot(zoom, height, 'o')
xlabel('Zoom (arbitrary units in [0, 13996]')
ylabel('Height (m)')
title('height of frame as a function of zoom')

figure(4)
plot(zoom, phi2, 'o')
xlabel('Zoom (arbitrary units in [0, 13996])')
ylabel('phi2 (degrees)')
title('phi2 as a function of zoom')

