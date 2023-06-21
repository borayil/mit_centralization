# %%
from matplotlib import cm
import numpy as np
import matplotlib.pyplot as plt
from PIL import Image


# %%
import os
path_to_data = os.path.join(os.getcwd(), '..', 'data', 'data.txt')
path_to_centralized_data = os.path.join(
    os.getcwd(), '..', 'data', 'transformed_data.txt')
output_path = os.path.join(os.getcwd(), '.', 'images')

# %%
#  Create output directory if it does not exist
if not os.path.exists(output_path):
    os.makedirs(output_path)


# %%

# Load the first data file
data_centralized = np.loadtxt(path_to_centralized_data)

# Convert the data into a 3D cylindrical coordinate system
n_sensors = data_centralized.shape[1]
n_depths = data_centralized.shape[0]
# evenly distributed angles around the pipe
theta = np.linspace(0, 2 * np.pi, n_sensors)
z = np.arange(n_depths) * 5

# Create a grid of the theta and z values
theta_grid, z_grid = np.meshgrid(theta, z)

# Convert from cylindrical to Cartesian coordinates
x_centralized = data_centralized * np.cos(theta_grid)
y_centralized = data_centralized * np.sin(theta_grid)

# Compute the deviation from the expected radius
expected_radius = 127
radius_deviation = np.abs(data_centralized - expected_radius)
# normalize deviation to [0, 1]
norm = plt.Normalize(radius_deviation.min(), radius_deviation.max())

# Create the first plot
fig = plt.figure(figsize=(20, 12))

# Plot for the centralized data
ax_centralized = fig.add_subplot(122, projection='3d')

# Create a color map
colors = cm.viridis(norm(radius_deviation))

# Plot for the centralized data
surf_centralized = ax_centralized.plot_surface(
    x_centralized, y_centralized, z_grid, facecolors=colors, shade=False
)

# Set the axis labels for the centralized data
ax_centralized.set_title('Centralized Readings')

# Do same process for non-centralized data
data_noncentralized = np.loadtxt(path_to_data)
x_noncentralized = data_noncentralized * np.cos(theta_grid)
y_noncentralized = data_noncentralized * np.sin(theta_grid)
radius_deviation = np.abs(data_noncentralized - expected_radius)
norm = plt.Normalize(radius_deviation.min(), radius_deviation.max())

ax_noncentralized = fig.add_subplot(121, projection='3d')
ax_noncentralized.set_title('Offset Readings')
colors = cm.viridis(norm(radius_deviation))
surf_noncentralized = ax_noncentralized.plot_surface(
    x_noncentralized, y_noncentralized, z_grid, facecolors=colors, shade=False
)

#  Hide the axis labels
ax_noncentralized.set_xticks([])
ax_noncentralized.set_yticks([])
ax_noncentralized.set_zticks([])
ax_centralized.set_xticks([])
ax_centralized.set_yticks([])
ax_centralized.set_zticks([])

# Remove background color
ax_noncentralized.xaxis.pane.fill = False
ax_noncentralized.yaxis.pane.fill = False
ax_noncentralized.zaxis.pane.fill = False
ax_noncentralized.xaxis.pane.set_edgecolor('white')

ax_centralized.xaxis.pane.fill = False
ax_centralized.yaxis.pane.fill = False
ax_centralized.zaxis.pane.fill = False
ax_centralized.xaxis.pane.set_edgecolor('white')

# Hide grid lines
ax_noncentralized.grid(False)
ax_centralized.grid(False)

# Hide the axes
ax_noncentralized.xaxis.line.set_color((1.0, 1.0, 1.0, 0.0))
ax_noncentralized.yaxis.line.set_color((1.0, 1.0, 1.0, 0.0))
ax_noncentralized.zaxis.line.set_color((1.0, 1.0, 1.0, 0.0))

ax_centralized.xaxis.line.set_color((1.0, 1.0, 1.0, 0.0))
ax_centralized.yaxis.line.set_color((1.0, 1.0, 1.0, 0.0))
ax_centralized.zaxis.line.set_color((1.0, 1.0, 1.0, 0.0))


# Set the rotation angle
azimuth_start = 0
azimuth_end = 360
n_frames = 100  # Total number of frames in the animation
step = (azimuth_end - azimuth_start) / n_frames  # Step size for each frame
elevation_angle = 42

# Create a directory to store the frames
frames_dir = os.path.join(output_path, "frames")
os.makedirs(frames_dir, exist_ok=True)


print("Animating... (this may take a bit)")

# Rotate the plots and save each frame
for i in range(n_frames):
    azimuth_angle = azimuth_start + i * step
    ax_centralized.view_init(elevation_angle, azimuth_angle)
    ax_noncentralized.view_init(elevation_angle, azimuth_angle)
    plt.savefig(os.path.join(frames_dir, f'frame_{i}.png'))

# Create the GIF
frames = []
for i in range(n_frames):
    new_frame = Image.open(os.path.join(frames_dir, f'frame_{i}.png'))
    frames.append(new_frame)

# Save into a GIF file
gif_save_path = os.path.join(output_path, "3d.gif")
frames[0].save(gif_save_path, format='GIF',
               append_images=frames[1:],
               save_all=True,
               duration=120, loop=0)

# Remove the frames
for i in range(n_frames):
    os.remove(os.path.join(frames_dir, f'frame_{i}.png'))

# Remove the frames directory
os.rmdir(frames_dir)
