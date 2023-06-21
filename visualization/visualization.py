import os
import numpy as np
import matplotlib.pyplot as plt
import pandas as pd
from matplotlib.lines import Line2D
from matplotlib import cm

# Define data paths
path_to_data = os.path.join(os.getcwd(), '..', 'data', 'data.txt')
path_to_centralized_data = os.path.join(
    os.getcwd(), '..', 'data', 'transformed_data.txt')
output_path = os.path.join(os.getcwd(), '.', 'images')

# Create output directory if it does not exist
if not os.path.exists(output_path):
    os.makedirs(output_path)

# Load the data file
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
ax_centralized = fig.add_subplot(122, projection='3d')  # Change 111 to 121

# Create a color map
colors = cm.viridis(norm(radius_deviation))

# Plot for the centralized data
surf_centralized = ax_centralized.plot_surface(
    x_centralized, y_centralized, z_grid, facecolors=colors, shade=False
)

# Set the axis labels for the centralized data
ax_centralized.set_title('3D Visualization of Centralized Readings')

# Non-centralized data visualization
data_noncentralized = np.loadtxt(path_to_data)
x_noncentralized = data_noncentralized * np.cos(theta_grid)
y_noncentralized = data_noncentralized * np.sin(theta_grid)
radius_deviation = np.abs(data_noncentralized - expected_radius)
norm = plt.Normalize(radius_deviation.min(), radius_deviation.max())

ax_noncentralized = fig.add_subplot(121, projection='3d')

colors = cm.viridis(norm(radius_deviation))
surf_noncentralized = ax_noncentralized.plot_surface(
    x_noncentralized, y_noncentralized, z_grid, facecolors=colors, shade=False
)
ax_noncentralized.set_title(
    '3D Visualization of Non-Centralized Initial Readings')

# Save and display the plt
plt.savefig(os.path.join(
    output_path, '3d.png'))
plt.show()


# Convert to pandas DataFrame
df = pd.DataFrame(data_noncentralized)
df_centralized = pd.DataFrame(data_centralized)

# Prepare the figure
plt.figure(figsize=(20, 12))

# Plot noncentralized data
for column in df:
    plt.plot(df[column], color='b')

# Plot centralized data
for column in df_centralized:
    plt.plot(df_centralized[column], linestyle='--', color='r')

plt.xlabel('Depth (mm)')
plt.ylabel('Reading (mm)')
plt.title('Finger readings through pipe')
plt.axhline(y=127, color='g', linestyle='-')

# Custom legend
custom_lines = [Line2D([0], [0], color='b', lw=2),
                Line2D([0], [0], color='r', lw=2, linestyle='--'),
                Line2D([0], [0], color='g', lw=2)]
plt.legend(custom_lines, ['Noncentralized Readings',
           'Centralized Readings', 'Pipe Radius (127 mm)'])

# Save and display the plt
plt.savefig(os.path.join(output_path, 'finger_readings_plot.png'))
plt.show()
