
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

import seaborn as sns

sns.set_context('poster', font_scale=2.5)


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
data_base = '/home/foz/Desktop/new_roots/130618 Axiostar ht Col-0 bent to left/'

# Location of segmented images (im==0 -> bg, im>0 -> root)
sillhouette_path = data_base + 'ver2/Thin_Origin/Image_Ske_Img_{}.png'

# Location of the midlines
mid_path = data_base + 'ver2/Thin/Image_Skeleton_{}.png'

# Location of cropped images
cropped_images = data_base + 'modified/Image_{}.png'

# Number of frames in sequence 
Nf = 150

# Micros per pixel
pixel_microns = 1.0 # Very rough estimate of microns per pixel

# Frame time
frame_time = 2.0 / 60.0 # Guess at image time spacing (in hours)

# Parameter used for spline smoothing of position of end of midline
smooth_end = 0#10000.0

# Parameter used for 2D spline smoothing of midline x-position
smooth_2D = 1000000 #3000000

output_directory = data_base + 'analysis/'

flow_path ='/home/foz/Desktop/new_roots/130618 Axiostar ht Col-0 bent to left/modified/Flow/root_{}_{}.flo'

Ns = 200 # Number of material points on the root to follow.

off = np.array((0.,0.))
tp = np.array((778., 560.))

offset_rhs = 80


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
# x-position of midline for each horizontal line of the image. (Root here grows downwards.)
mid = np.zeros((im.shape[0], Nf), dtype=int)
# y-position of the point in Tuan's midline with maximum y
end = np.zeros((Nf,), dtype=int)

flo_y = slice(0, 900)
flo_x = slice(0, 500)
## Now calculate image offsets for stabilization


offsets = [ off ]
tip_locations = [ tp ]
for i in range(1, Nf):
    if i>2:
        F = read_flo(flow_path.format(i, i+1))
    else:
        F = read_flo(flow_path.format(3, 4))
    v = np.mean(F[flo_y,flo_x,:], axis = (0,1))
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
        u = np.argmax(im, axis=1) # find the position of the midline in each row of the image
                                  # - i.e. the first non-zero pixel
#        end[i] = im.shape[0] - 1 - np.argmax(np.max(im, axis=1)[::-1]) # Find y-position of last pixel 
        
        shift = -offsets[i]
        print shift        
	tp = tip_locations[i]

        end[i] = int(tp[1]) # int(tp[1]) - shift[1]

        y_pos = np.array(range(int(tp[1]-offset_rhs+1)) + [tp[1]])# - shift[0]
        x_pos = np.array(u[:int(tp[1]-offset_rhs+1)].tolist() + [tp[0]])# - shift[1]
        print i, y_pos[0], u[0], shift[1]
        u[:end[i]+1] = interp1d(y_pos, x_pos)(np.arange(end[i]+1))
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


# new_mid now stores the x-positions positions of each midline.
# Here mew_mid[i,j] corresponds to the point (new_mid[i,j], i/(mid.shape[0]-1)*end[j])
# in the j-th frame.

# Use 2D spline to smooth midline x-position
u = RectBivariateSpline(range(new_mid.shape[0]), range(new_mid.shape[1]), new_mid, kx=5, ky=2, s=smooth_2D)

Ntx = 5
Nty = 10
tx = [0]*4 + np.linspace(0, new_mid.shape[0], Ntx).tolist() + [new_mid.shape[0]]*4
ty = [0]*2 + np.linspace(0, new_mid.shape[1], Nty).tolist() + [new_mid.shape[1]]*2


#u = ApproximatingSpline2D(range(new_mid.shape[0]), range(new_mid.shape[1]), new_mid, tx, 4, ty, 2)

# Evaluate this 2D spline to give smoothed midline x-points: (smid[i,j], i/(mid.shape[0]-1)*end[j])
#smid = u.eval_grid(range(new_mid.shape[0]), range(new_mid.shape[1]))
smid = u(range(new_mid.shape[0]), range(new_mid.shape[1]))

plt.figure(figsize=(15,15))
i = Nf/2
im = open_png(mid_path.format(i+1))
plt.imshow(im)
plt.hold(True)
plt.plot(smid[:,i], np.arange(smid.shape[0])*end[i]/float(smid.shape[0]), 'g-')
plt.plot(new_mid[:,i], np.arange(smid.shape[0])*end[i]/float(smid.shape[0]), 'k-.')
plt.xlim(600,1000)
plt.ylim(1500,0)
plt.savefig(output_directory + 'compare_frame_{}.png'.format(i))

overlay_path = output_directory + 'overlay/'

make_sure_path_exists(overlay_path)

# Superimpose smoothed midlines on the (cropped) original image sequence
for i in range(Nf):
    im = open_png(cropped_images.format(i+1))
    im = Image.fromarray(im.astype(np.uint8)).convert('RGB')
    d = Draw(im)
    x = smid[:,i].astype(int)
    y = (np.arange(smid.shape[0])*end[i]/float(smid.shape[0]-1)).astype(int)
    d.line(zip(x,y), fill=(255,0,0), width=3 ) 
    tp = tip_locations[i]
    d.ellipse((tp[0]-1, tp[1]-1, tp[0]+1, tp[1]+1), fill=(0,0,255))
    im.crop((400, 0, 1000, 1800)).save(overlay_path+'orig_{}.png'.format(i))


# Calculate scaling between transformed and y-coordinate
# in pixels - dy/di
sc = end / float(smid.shape[0]-1)

# Evaluate derivatives of the smoothed midline - dx/dy and d^2 x / dy^2
# Use scaling factor to give in terms of pixel coordinates

#quit()
#du = u.deriv_x()
#ddu = du.deriv_x()

#d = du.eval_grid(range(new_mid.shape[0]), range(new_mid.shape[1]))/sc
#dd = ddu.eval_grid(range(new_mid.shape[0]), range(new_mid.shape[1]))/sc/sc

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

mid_Exx = nd.map_coordinates(Exx, (np.arange(smid.shape[0])*end[i]/float(smid.shape[0]-1), smid[:,i]))
mid_Exy = nd.map_coordinates(Exy, (np.arange(smid.shape[0])*end[i]/float(smid.shape[0]-1), smid[:,i]))
mid_Eyx = nd.map_coordinates(Eyx, (np.arange(smid.shape[0])*end[i]/float(smid.shape[0]-1), smid[:,i]))
mid_Eyy = nd.map_coordinates(Eyy, (np.arange(smid.shape[0])*end[i]/float(smid.shape[0]-1), smid[:,i]))

x_all = smid[:,i]
y_all = (np.arange(smid.shape[0])*end[i]/float(smid.shape[0]-1))

# Slope of midline (dx / dy)

# x and y coordinates of tracked points
y_tracked = s0
x_tracked = UnivariateSpline(y_all, x_all, k=1, s=0)(s0)
                
paths.append(s)
# Find distances along spline at tracked points; only defined on curve
d2 = UnivariateSpline(y_all, distances[:,i], k=1, s=0)(s)
path_distances.append(d2)
# Find curvature along spline at tracked points
kappa = UnivariateSpline(y_all, c[:,i], s=smooth_kappa2)(s)
path_kappa.append(kappa)

# Find y-direction stretch along midline
stretch_yy = nd.map_coordinates(Eyy, (y_all, x_all))
all_stretch_yy.append(stretch_yy)

# Find actual stretch along the midline!

tx = d[:,i] / np.sqrt(1 + d[:,i]**2)
ty = 1.0 / np.sqrt(1 + d[:, i]**2)
    
stretch = tx*tx*mid_Exx + tx*ty*(mid_Exy + mid_Eyx) + ty*ty*mid_Eyy
all_stretch.append(stretch)

for i in range(1, Nf):
    print i
    # (Issues with first flow data file
    if i<3:
        F = read_flo(flow_path.format(3, 4))
    else:
        F = read_flo(flow_path.format(i, i+1))

    Exx, Exy, Eyx, Eyy = calc_strainrate(F, flow_sigma)
    
    # x and y (px) coordinates of *all* midline points.
    x_all = smid[:,i]
    y_all = (np.arange(smid.shape[0])*end[i]/float(smid.shape[0]-1))

    # Tangent to midline
    tx = d[:,i] / np.sqrt(1 + d[:,i]**2)
    ty = 1.0 / np.sqrt(1 + d[:, i]**2)

    # x and y coordinates of tracked points
    y_tracked = s0
    x_tracked = UnivariateSpline(y_all, x_all, k=1, s=0)(s0)
    
    # Find y-component of velocities of material points    
    v = nd.map_coordinates(F[:,:,1], (y_all, x_all))
    # Interpolate these to find y-velocities for all tracked points
    v2 = UnivariateSpline(y_all, v, s=smooth_speed2)(s0)
    # Alternatively we could write v2 = nd.map_coordinates(F[:,:,1], (y_tracked, x_tracked))

    # Move material points to new position for the frame
    s = s0 + v2
    paths.append(s)
    # Find distances along spline at tracked points; only defined on curve
    d2 = UnivariateSpline(y_all, distances[:,i], k=1, s=0)(s)
    path_distances.append(d2)
    # Find curvature along spline at tracked points
    kappa = UnivariateSpline(y_all, c[:,i], s=smooth_kappa2)(s)
    path_kappa.append(kappa)

    # Find y-direction stretch along midline
    stretch_yy = nd.map_coordinates(Eyy, (y_all, x_all))
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

plt.figure(figsize=(25,25))
plt.contourf(np.outer(np.ones(smid.shape[0],), np.arange(Nf))*frame_time, (total_distances - distances)*pixel_microns, np.clip(-c/pixel_microns, -0.0015, 0.0015), 30, cmap=plt.cm.seismic, vmin=-0.001, vmax=0.001)


plt.hold(True)
path_distances = np.array(path_distances)
plt.plot(np.arange(Nf)*frame_time, (total_distances[:, np.newaxis] - path_distances[:,::8])*pixel_microns, 'k-', lw=1)
plt.xlabel('time (hr)')
plt.ylabel('Distance from tip ($\mu$ m)')
cb = plt.colorbar()
cb.set_ticks([-0.001, 0.0, 0.001])
cb.ax.set_ylabel('Curvature $\mu$ m$^{-1}$)')

plt.savefig(output_directory+'curvature.png')
plt.savefig(output_directory+'curvature.pdf', format='pdf')
plt.savefig(output_directory+'curvature.svg', format='svg')


plt.figure(figsize=(25,25))
plt.contourf(np.outer(np.ones(smid.shape[0],), np.arange(Nf))*frame_time, (total_distances - distances)*pixel_microns, np.clip(-c/pixel_microns, -0.0015, 0.0015), 30, cmap=plt.cm.seismic, vmin=-0.001, vmax=0.001)
plt.hold(True)
plt.xlabel('time (hr)')
plt.ylabel('Distance from tip ($\mu$ m)')
cb = plt.colorbar()
cb.set_ticks([-0.001, 0.0, 0.001])
cb.ax.set_ylabel('Curvature $\mu$ m$^{-1}$)')

plt.savefig(output_directory+'curvature_nolines.png')
plt.savefig(output_directory+'curvature_nolines.pdf', format='pdf')
plt.savefig(output_directory+'curvature_nolines.svg', format='svg')



all_stretch = np.array(all_stretch)
#all_stretch = nd.gaussian_filter(all_stretch, [0.5, 0.5])



plt.figure(figsize=(25,25))
cs = plt.contourf(np.outer(np.ones(all_stretch.shape[1],), np.arange(Nf))*frame_time, (total_distances - distances)*pixel_microns, all_stretch.T/frame_time,  np.linspace(-0.1,1.0, 25), vmax=1.0, cmap = plt.cm.jet, rasterized=True)

for c in cs.collections:
    c.set_rasterized(True)
plt.hold(True)
path_distances = np.array(path_distances)
plt.plot(np.arange(Nf)*frame_time, (total_distances[:, np.newaxis] - path_distances[:,::8])*pixel_microns, 'k-', lw=1)
cb = plt.colorbar()
cb.set_ticks([0.0, 1.0])
cb.ax.set_ylabel('RER (hr$^{-1}$)')

plt.xlabel('time (hr)')
plt.ylabel('Distance from tip ($\mu$ m)')
plt.savefig(output_directory+'RER.png')
plt.savefig(output_directory+'RER.svg', format='svg', dpi=100)
plt.savefig(output_directory+'RER.pdf', format='pdf', dpi=100)


bending =  np.clip(-dk.T/frame_time/pixel_microns, -0.001, 0.001)


plt.figure(figsize=(25,25))
cs = plt.contourf(np.outer(np.ones(all_stretch.shape[1],), np.arange(Nf))*frame_time, (total_distances - distances)*pixel_microns, all_stretch.T/frame_time,  np.linspace(-0.1,1.0, 25), vmax=1.0, cmap = plt.cm.jet, rasterized=True)
for c in cs.collections:
    c.set_rasterized(True)

plt.hold(True)
path_distances = np.array(path_distances)
cb = plt.colorbar()
cb.set_ticks([0.0, 1.0])
cb.ax.set_ylabel('RER (hr$^{-1}$)')

plt.xlabel('time (hr)')
plt.ylabel('Distance from tip ($\mu$ m)')
plt.savefig(output_directory+'RER_nolines.png')
plt.savefig(output_directory+'RER_nolines.svg', format='svg')
plt.savefig(output_directory+'RER_nolines.pdf', format='pdf')


bending =  np.clip(dk.T/frame_time/pixel_microns, -0.001, 0.001)

plt.figure(figsize=(25,25))
plt.contourf(np.outer(np.ones((Ns,)), np.arange(Nf-1))*frame_time, (total_distances[:-1, np.newaxis] - path_distances[:-1,:]).T*pixel_microns, bending, 30, cmap=plt.cm.seismic, vmin=-0.001, vmax=0.001)
plt.plot(np.arange(Nf)*frame_time, (total_distances[:, np.newaxis] - path_distances[:,::8])*pixel_microns, 'k-', lw=1)
cb = plt.colorbar()

plt.xlabel('time (hr)')
plt.ylabel('Distance from tip (\mu m)')
plt.savefig(output_directory+'kdot.png')
plt.savefig(output_directory+'kdot.svg', format='svg')
plt.savefig(output_directory+'kdot.pdf', format='pdf')
plt.show()
