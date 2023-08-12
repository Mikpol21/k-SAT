import matplotlib.pyplot as plt

# BP

ratios_2000 = [9.0, 9.15, 9.30, 9.40, 9.50, 9.60]
bp_2000 = [0.98, 1, 0.96, 0.84, 0.52, 0.28]
ratios_1000 = [9.0, 9.1, 9.2, 9.3]
bp_1000 = [0.98, 0.9, 0.94, 0.74]

fig, ax = plt.subplots(2)
ax[0].title.set_text('BP on 4-SAT')
ax[0].plot(ratios_2000, bp_2000, 'c', label='2000')
ax[0].plot(ratios_1000, bp_1000, 'y', label='1000')
ax[0].legend()
ratios_tree_100000 = [2.6, 2.8, 3.0, 3.2, 3.4, 3.6]
majority_100000 = [0.66, 0.44, 0.06, 0., 0, 0.]
tree_100000 = [1, 0.92, 0.88, 0.48, 0, 0]
johnson_100000 = [0.98, 0.98, 0.94, 0.86, 0.24, 0]
ax[1].set_title('Johnson, Majority, Tree on 3-SAT', fontsize=10)
ax[1].plot(ratios_tree_100000, tree_100000, 'r', label='tree')
ax[1].plot(ratios_tree_100000, johnson_100000, 'g', label='johnson')
ax[1].plot(ratios_tree_100000, majority_100000, 'b', label='majority')

ax[1].legend()
plt.savefig("results.png")

