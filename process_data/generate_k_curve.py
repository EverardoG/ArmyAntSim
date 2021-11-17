from typing import final
import numpy as np
import matplotlib.pyplot as plt

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

# Generate x values for plotting
num_pts = 10000
x_values = np.linspace(0,25,num_pts)

# Select ks and offsets
ks = [0.1, 2.5, 20]
offsets = [0,5.5]

# Generate curve based on those xs, ks, and offsets
curves = []
for k in ks:
    curves_by_k = []
    for offset in offsets:
        curve = generate_curve(x_values, k, offset, x_goal)
        curves_by_k.append(curve)
    curves.append(curves_by_k)

# Plot those curves accordingly
fig, axs = plt.subplots(len(ks), len(offsets), figsize=(6,6), dpi=100)

y_low = -0.2
y_high = 2*np.pi+0.2


for ax_row, curves_by_k in zip(axs, curves):
    for ax, curve in zip(ax_row, curves_by_k):
        ax.plot(x_values, curve, label="Speed")
        ax.plot([x_goal, x_goal], [y_low, y_high], ":", label="Island Left Edge")

# Set title
# fig.suptitle("Speed Functions")

# set labels
plt.setp(axs[-1, :], xlabel='x Position (BL)')
plt.setp(axs[:, 0], ylabel='Speed (rad/s)')

# set legend
lines, labels = axs[-1][-1].get_legend_handles_labels()
fig.legend(lines, labels, loc = 'upper right')

plt.tight_layout()
plt.show()