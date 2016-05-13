
"""
Analysis of midline and optical flow data
"""

# Imports and needed packages
import itertools

import numpy as np
import matplotlib.pyplot as plt
from scipy.interpolate import UnivariateSpline 
from scipy.interpolate import RectBivariateSpline
from scipy.interpolate import interp1d


import scipy.ndimage as nd

import png # Separate package for reading PNG files - needed for the midlines
           # Supplied along with this code

from read_flo import read_flo # My code to read the deepflow .FLO output files

from scipy.ndimage import gaussian_filter

from PIL.ImageDraw import Draw
from PIL import Image


### Snippet to make sure path is present - http://stackoverflow.com/questions/273192/how-to-check-if-a-directory-exists-and-create-it-if-necessary
import os
import errno

def make_sure_path_exists(path):
    try:
        os.makedirs(path)
    except OSError as exception:
        if exception.errno != errno.EEXIST:
            raise

### Parameters and constants for the image sequence

# Location of directories containing output of Tuan's analysis
data_base0 = '/home/foz/Desktop/new_roots'
data_base = data_base0 + '/151117 Col ht timelapse II flipped plate bent right way/' 

# Location of segmented images (im==0 -> bg, im>0 -> root)
sillhouette_path = data_base + 'ver1/Thin_Origin/Image_Ske_Img_{}.png'

# Location of the midlines
mid_path = data_base + 'ver1/Thin/Image_Skeleton_{}.png'

# Location of cropped images
original_images = data_base + 'Modified/Image_{}.png'

# Number of frames in sequence 
Nf = 201

# Micros per pixel
pixel_microns = 1000.0/955.0 # Very rough estimate of microns per pixel

# Frame time
frame_time = 2.0 / 60.0 # Guess at image time spacing (in hours)

# Parameter used for spline smoothing of position of end of midline
smooth_end = None #30000.0

# Parameter used for 2D spline smoothing of midline x-position
smooth_2D = 2000000

output_directory = data_base + 'analysis/'

flow_path = data_base + 'FLO/root_{}_{}.flo'

Ns = 200 # Number of material points on the root to follow.

offset_lhs = 50 # Noise in the LH end of the midline - cut off the last bit
                # and replace by a straight line


smooth_speed2 = 0
smooth_kappa2 = 0

flow_sigma = 20

### Utility functions for importing midline images

# Alternative import method for MATLAB png files as these break PIL

def open_png(fn):
    pngReader=png.Reader(filename=fn)
    row_count, column_count, pngdata, meta = pngReader.asDirect()
    image_2d = np.vstack(itertools.imap(np.uint8, pngdata))
    return image_2d

### Analysis starts here

# Make sure output directory exists

make_sure_path_exists(output_directory)

# Load first image in sequence to extract frame size
im = open_png(mid_path.format(1))

# Arrays to store midline information
# y-position of midline for each horizontal line of the image. (Root here grows right.)
mid = np.zeros((im.shape[1], Nf), dtype=int)
# x-position of the point in Tuan's midline with maximum x
end = np.zeros((Nf,), dtype=int)


## Now calculate image offsets for stabilization

off = np.array((0.,0.))
tp = np.array((685., 243.))

offset_rhs = 80

offsets = [ off ]
tip_locations = [ tp ]
for i in range(1, Nf):
    F = read_flo(flow_path.format(i, i+1))
    v = np.mean(F[500:,:750,:], axis = (0,1))
    new_off = off - v
    
#    v_point = F[np.rint(tp[1]), np.rint(tp[0]), :]

    v_point = nd.map_coordinates(F, [[tp[1], tp[1]], [tp[0], tp[0]], [0, 1]], order=1)


    new_tp = tp + v_point 

    offsets.append(new_off)
    tip_locations.append(new_tp)

    tp = new_tp
    off = new_off

## Load in midline image sequence. Populate mid and end arrays for each image.


for i in range(Nf):
    print i
    try:
        im = open_png(mid_path.format(i+1)) # Midline .png images break other import codes
        u = np.argmax(im, axis=0) # find the position of the midline in each row of the image
                                  # - i.e. the first non-zero pixel
#        end[i] = im.shape[1] - 1 - np.argmax(np.max(im, axis=0)[::-1]) # Find y-position of last pixel 
                                                                       # *check this*
        
        shift = -offsets[i]
        print shift
        tp = tip_locations[i]

        u[:offset_lhs] = u[offset_lhs]

        end[i] = int(tp[0]-shift[0])

        x_pos = np.array(range(int(tp[0]-offset_rhs+1)) + [tp[0]]) - shift[0]
        y_pos = np.array(u[:int(tp[0]-offset_rhs+1)].tolist() + [tp[1]]) - shift[1]
        print i, y_pos[0], u[0], shift[1]
        u[:end[i]+1] = interp1d(x_pos, y_pos)(np.arange(end[i]+1))
        u[end[i]+1:] = 0
        mid[:,i] = u # Populate midline array for this image

    except IOError:
        pass


### Smooth the position of the end pixel (end[]) using a spline

s = UnivariateSpline(range(Nf), end, s=smooth_end) # Spline interpolation of end

# Plot original and smoothed endpoint to check that this is reasonable
plt.figure(figsize=(10,10))
plt.title('Smoothing of end position')
plt.plot(end, label='original')
plt.hold(True)
plt.plot(s(range(Nf)), label='smoothed')
plt.legend()
plt.xlabel('Frame number')
plt.ylabel('y position of furthest pixel (px)')
plt.savefig(output_directory + 'compare_smooth_end.png')

## Now need to deal with missing data points.

# Function to help with interpolating over missing data
def zero_helper(y):
    return y==0, lambda z: z.nonzero()[0]

for j in range(Nf): # Loop over frames
    # If the original max y-coordinate is less than the interpolated position,
    # extend the midline data (by simply copying it)
    if end[j] <=  int(s(j)):
        mid[end[j]:int(s(j))+1,j] = mid[end[j]-1,j]
    
    # If the original max y-coordinate is greater than the interpolated position,
    # zero out all data beyond the new endpoint
    if end[j] > int(s(j)):
        mid[int(s(j))+1:,j]=0
    
    # Set the endpoint to be the interpolated position
    end[j] = int(s(j))

    # Fill any gaps in the midline data by linear interpolation
    u = mid[0:end[j], j]
    zeros, x = zero_helper(u)
    mid[zeros,j] = np.interp(x(zeros), x(~zeros), u[~zeros])
    
# Interpolate missing data (between frames)


# Now transform domain over which midline data is specified, so that we can
# use 2D spline interpolation on a rectangular region to smooth it
#
# i.e.
#  ------      ------
#  |    |      |    |
#  \    |      |    |
#  0\   |  ->  |    |
#  00\  |      |    |
#  000---      ------
#
# through stretching each column of the matrix
#

i,j = np.meshgrid(range(mid.shape[0]),range(mid.shape[1]))

# For each frame, 
new_i = i/float(mid.shape[0]-1)*end[j]

# Given indices new_i, j, use linear interpolation on the original midline x-position
# array to find the x-positions for the points in these transformed coordinates
new_coordinates = np.vstack((new_i.T.flat,j.T.flat))
new_mid = nd.map_coordinates(mid, new_coordinates, order=1).reshape(mid.shape)

for i in range(1,new_mid.shape[0]):
    zeros, x = zero_helper(new_mid[i,:])
    if np.sum(zeros)>0:
	print 'missing data', i, np.sum(zeros)
        new_mid[i,zeros] = np.interp(x(zeros), x(~zeros), new_mid[i,~zeros])


plt.imshow(new_mid, aspect=0.1)


# new_mid now stores the y-positions positions of each midline.
# Here mew_mid[i,j] corresponds to the point (i/(mid.shape[0]-1)*end[j], new_mid[i,j])
# in the j-th frame.

# Use 2D spline to smooth midline x-position
u = RectBivariateSpline(range(new_mid.shape[0]), range(new_mid.shape[1]), new_mid, kx=5, ky=2, s=smooth_2D)

# Evaluate this 2D spline to give smoothed midline x-points: (smid[i,j], i/(mid.shape[0]-1)*end[j])
smid = u(range(new_mid.shape[0]), range(new_mid.shape[1]))

plt.figure(figsize=(15,15))
i = Nf/2
im = open_png(mid_path.format(i+1))
plt.imshow(im)
plt.hold(True)
plt.plot(np.arange(smid.shape[0])*end[i]/float(smid.shape[0]), smid[:,i], 'g-')
plt.plot(np.arange(smid.shape[0])*end[i]/float(smid.shape[0]), new_mid[:,i], 'k-.')
plt.xlim(0,1000)
plt.ylim(1500,0)
plt.savefig(output_directory + 'compare_frame_{}.png'.format(i))

overlay_path = output_directory + 'overlay/'

make_sure_path_exists(overlay_path)

# Superimpose smoothed midlines on the (cropped) original image sequence
for i in range(Nf):
    shift = -offsets[i]
#    im = Image.open(original_images.format(i+1))
#    im = im.crop((0, 680, 2260, 680+810)) 
    im = open_png(sillhouette_path.format(i+1))
    im = Image.fromarray(im.astype(np.uint8)).convert('RGB')
    d = Draw(im)
    y = smid[:,i].astype(int)+shift[1]
    x = (np.arange(smid.shape[0])*end[i]/float(smid.shape[0]-1)).astype(int)+shift[0]
    d.line(zip(x,y), fill=(255,0,0), width=3 )
    tp = tip_locations[i]
    d.ellipse((tp[0]-1, tp[1]-1, tp[0]+1, tp[1]+1), fill=(0,0,255))
    im.save(overlay_path+'orig_{}.png'.format(i))


# Calculate scaling between transformed and y-coordinate
# in pixels - dy/di
sc = end / float(smid.shape[0]-1)

# Evaluate derivatives of the smoothed midline - dx/dy and d^2 x / dy^2
# Use scaling factor to give in terms of pixel coordinates
d = u(range(new_mid.shape[0]), range(new_mid.shape[1]), dx=1)/sc
dd = u(range(new_mid.shape[0]), range(new_mid.shape[1]), dx=2)/sc/sc


# Calculate distance 
# ds = ds / di. Use d^2 d/dy^2 = 1 + (dx/dy)^2 and dy/di = sc 
ds = np.sqrt(1+d*d)*sc
# Find the cumulative distance of each point from the top of the image
# (This is pixel midpoint to the top)
dsum =  np.cumsum(ds,axis=0)
# Trapesium rule-like integration
distances = 0.5*dsum
distances[1:,:] += 0.5*dsum[:-1,:]
# Total length of the midline (to the bottom, not endpoint, of the last pixel).
total_distances = np.sum(ds, axis=0)

# Calculate curvature of the smoothed midline using standard (cartesian) coordinate
# definition
c = dd/(1+d*d)**(3.0/2.0)

# Calculate the rate of strain tensor; taking derivatives
# of the flow field using gaussian derivatives.

def calc_strainrate(A, sigma):
    Aux = gaussian_filter(A[:,:,0], sigma, (0, 1))
    Auy = gaussian_filter(A[:,:,0], sigma, (1, 0))
    Avx = gaussian_filter(A[:,:,1], sigma, (0, 1))
    Avy = gaussian_filter(A[:,:,1], sigma, (1, 0))
    Exx = Aux
    Exy = 0.5*(Auy+Avx)
    Eyx = Exy
    Eyy = Avy
    return Exx, Exy, Eyx, Eyy

# Largest eigenvalue of a two-dimensional matrix
# Can be used in a vectorized form
def largest_eigenvalue(A11, A12, A21, A22):
    T = A11 + A22
    D = A11*A22 - A21*A12
    L1 = T/2.0 + np.sqrt(T*T/4.0-D)
    return L1




# Now follow material points on the midline

# y-coordinate of each of the material points
paths = []
# distances along the midline of each of the material points
path_distances = []
# curvature of the midline (in px^{-1}) following midline points
path_kappa = []
# Just the y-component of the stretch along the midline
all_stretch_yy = []
# RER (gamma) along the midline
all_stretch = []
# Initial (pixel) y-positions of material points


s0 = np.linspace(0, end[0], Ns)

F = read_flo(flow_path.format(1, 2))

Exx, Exy, Eyx, Eyy = calc_strainrate(F, flow_sigma)


# x and y (px) coordinates of *all* midline points.
i = 0
s = s0

y_all = smid[:,i]
x_all = (np.arange(smid.shape[0])*end[i]/float(smid.shape[0]-1))

mid_Exx = nd.map_coordinates(Exx, (y_all, x_all))
mid_Exy = nd.map_coordinates(Exy, (y_all, x_all))
mid_Eyx = nd.map_coordinates(Eyx, (y_all, x_all))
mid_Eyy = nd.map_coordinates(Eyy, (y_all, x_all))



# Slope of midline (dx / dy)

# x and y coordinates of tracked points
x_tracked = s0
y_tracked = UnivariateSpline(x_all, y_all, k=1, s=0)(s0)
                
paths.append(s)
# Find distances along spline at tracked points; only defined on curve
d2 = UnivariateSpline(x_all, distances[:,i], k=1, s=0)(s)
path_distances.append(d2)
# Find curvature along spline at tracked points
kappa = UnivariateSpline(x_all, c[:,i], s=smooth_kappa2)(s)
path_kappa.append(kappa)

# Find y-direction stretch along midline
stretch_yy = nd.map_coordinates(Eyy, (y_all, x_all))
all_stretch_yy.append(stretch_yy)

# Find actual stretch along the midline!

ty = d[:,i] / np.sqrt(1 + d[:,i]**2)
tx = 1.0 / np.sqrt(1 + d[:, i]**2)
    
stretch = tx*tx*mid_Exx + tx*ty*(mid_Exy + mid_Eyx) + ty*ty*mid_Eyy
all_stretch.append(stretch)

for i in range(1, Nf):
    print i, s0[::5]
    # (Issues with first flow data file

    shift = -offsets[i]
    
    F = read_flo(flow_path.format(i, i+1))

    Exx, Exy, Eyx, Eyy = calc_strainrate(F, flow_sigma)
    
    # x and y (px) coordinates of *all* midline points.
    y_all = smid[:,i]+shift[1]
    x_all = (np.arange(smid.shape[0])*end[i]/float(smid.shape[0]-1))+shift[0]

    # Tangent to midline
    ty = d[:,i] / np.sqrt(1 + d[:,i]**2)
    tx = 1.0 / np.sqrt(1 + d[:, i]**2)

    # x and y coordinates of tracked points
    x_tracked = s0
    y_tracked = UnivariateSpline(x_all, y_all, k=1, s=0)(s0)
    
    # Find y-component of velocities of material points    
    v = nd.map_coordinates(F[:,:,0], (y_all, x_all))
    # Interpolate these to find y-velocities for all tracked points
    v2 = UnivariateSpline(x_all, v, s=smooth_speed2)(s0)
    # Alternatively we could write v2 = nd.map_coordinates(F[:,:,1], (y_tracked, x_tracked))

    # Move material points to new position for the frame
    s = s0 + v2
    paths.append(s)
    # Find distances along spline at tracked points; only defined on curve
    d2 = UnivariateSpline(x_all, distances[:,i], k=1, s=0)(s)
    path_distances.append(d2)
    # Find curvature along spline at tracked points
    kappa = UnivariateSpline(x_all, c[:,i], s=smooth_kappa2)(s)
    path_kappa.append(kappa)

    # Find y-direction stretch along midline
    stretch_yy = nd.map_coordinates(Exx, (y_all, x_all))
    all_stretch_yy.append(stretch_yy)

    # Find actual stretch along the midline!
 
    mid_Exx = nd.map_coordinates(Exx, (y_all, x_all))
    mid_Exy = nd.map_coordinates(Exy, (y_all, x_all))
    mid_Eyx = nd.map_coordinates(Eyx, (y_all, x_all))
    mid_Eyy = nd.map_coordinates(Eyy, (y_all, x_all))
    
    stretch = tx*tx*mid_Exx + tx*ty*(mid_Exy + mid_Eyx) + ty*ty*mid_Eyy

    all_stretch.append(stretch)

    s0 = s
    #print end[i]

# Differentiate the curvature wrt the frame, following material points

path_kappa = np.array(path_kappa)
dk = path_kappa[1:,:] - path_kappa[:-1,:]

plt.figure(figsize=(15,15))
plt.contourf(np.outer(np.ones(smid.shape[0],), np.arange(Nf))*frame_time, (total_distances - distances)*pixel_microns, c/pixel_microns, 30, cmap=plt.cm.seismic, vmin=-0.001, vmax=0.001)
plt.hold(True)
path_distances = np.array(path_distances)
plt.plot(np.arange(Nf)*frame_time, (total_distances[:, np.newaxis] - path_distances[:,::8])*pixel_microns, 'k-')
plt.xlabel('time (hr)')
plt.ylabel('Distance from tip (um)')
plt.colorbar()
plt.savefig(output_directory+'curvature.png')

all_stretch = np.array(all_stretch)
plt.figure(figsize=(15,15))
plt.contourf(np.outer(np.ones(all_stretch.shape[1],), np.arange(Nf))*frame_time, (total_distances - distances)*pixel_microns, all_stretch.T/frame_time,  np.linspace(-0.1,1.0, 30), vmax=1.0)
plt.hold(True)
path_distances = np.array(path_distances)
plt.plot(np.arange(Nf)*frame_time, (total_distances[:, np.newaxis] - path_distances[:,::8])*pixel_microns, 'k-')
plt.colorbar()
plt.xlabel('time (hr)')
plt.ylabel('Distance from tip (um)')
plt.savefig(output_directory+'RER.png')

bending =  np.clip(dk.T/frame_time/pixel_microns, -0.001, 0.001)

plt.figure(figsize=(15,15))
plt.contourf(np.outer(np.ones((Ns,)), np.arange(Nf-1))*frame_time, (total_distances[:-1, np.newaxis] - path_distances[:-1,:]).T*pixel_microns, bending, 30, cmap=plt.cm.seismic, vmin=-0.001, vmax=0.001)
plt.plot(np.arange(Nf)*frame_time, (total_distances[:, np.newaxis] - path_distances[:,::8])*pixel_microns, 'k-')
plt.colorbar()
plt.xlabel('time (hr)')
plt.ylabel('Distance from tip (um)')
plt.savefig(output_directory+'kdot.png')
plt.show()
