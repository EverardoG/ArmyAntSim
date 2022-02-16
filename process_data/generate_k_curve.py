import numpy as np
import matplotlib
import matplotlib.pyplot as plt

matplotlib.rcParams['mathtext.fontset'] ='stix'
matplotlib.rcParams['font.family'] = 'STIXGeneral'

initial_speed = 2 * np.pi
final_speed = 0.5
x_goal = 15

def generate_curve(x_values: np.ndarray, k: float, offset: float, x_goal: float)->np.ndarray:
    x_shift = generate_x_shift(x_goal, k, offset)
    out = initial_speed/(1+np.exp(k*(x_values-x_shift)))
    return out

def generate_x_shift(x_goal: float, k: float, offset: float):
    out = (x_goal+offset) - (1/k)*np.log(initial_speed/final_speed - 1)
    return out

# Set correct matplotlib font
tick_font_size = 18
plt.rc('xtick', labelsize=tick_font_size)
plt.rc('ytick', labelsize=tick_font_size)
plt.rc('legend', fontsize=tick_font_size)
axes_font_size = 23
plt.rc('axes', labelsize=axes_font_size)
plt.rc('axes', titlesize=axes_font_size)
plt.rc('figure', titlesize=axes_font_size)

# Generate x values for plotting
num_pts = 10000
x_values = np.linspace(0,25,num_pts)

# Select ks and offsets
ks = [0.1, 5.0]
offsets = [0,3.73]

# Generate curve based on those xs, ks, and offsets
curves = []
for k in ks:
    curves_by_k = []
    for offset in offsets:
        curve = generate_curve(x_values, k, offset, x_goal)
        curves_by_k.append(curve)
    curves.append(curves_by_k)

# Plot those curves accordingly
fig, axs = plt.subplots(len(ks), len(offsets), figsize=(10,12), dpi=100)

y_low = -0.2
y_high = 2*np.pi+0.2

for ax_row, curves_by_k in zip(axs, curves):
    for ax, curve in zip(ax_row, curves_by_k):
        ax.plot(x_values, curve, label="Flipping Speed (rad/s)")
        ax.plot([x_goal, x_goal], [y_low, y_high], ":", label="Left Edge of Island")

# Set title
fig.suptitle("Flipping Speed as a Function of "+r'$x$'+" Position")

# Add k and offset labels
for ax, k_value in zip(axs[:, 0], ks):
    plt.setp(ax, ylabel=r'$k = $'+str(k_value))
for ax, offset_value in zip(axs[0, :], offsets):
    plt.setp(ax, title=r'$\sigma = $'+str(offset_value))

# Set x labels
for ax_row in axs:
    for ax in ax_row:
        ax.set_xlabel(r'$x$'+' Position (BL)', fontdict={'size':tick_font_size})

# Tighten up whitespace
fig.tight_layout()

# Add white space between plots and title
fig.subplots_adjust(top=0.9)

# Setup legend: Grab info for legend
lines, labels = axs[-1][-1].get_legend_handles_labels()
# Place legend
fig.legend(lines, labels, loc = 'upper right', bbox_to_anchor=(0.99,0.905))

plt.show()