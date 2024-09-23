import numpy as np
from scipy.integrate import odeint
import matplotlib.pyplot as plt

# Parameters
D = 50         # Bolus insulin dose (units)
V_d = 10000    # Volume of distribution (mL), 10 liters
tau = 30       # Absorption time constant (min)
t_inj = 0      # Time of injection (min)
R_b = 0.025    # Basal insulin infusion rate (units/min) adjusted for physiological range
n_I = 0.1      # Insulin clearance rate (1/min)

# Insulin absorption function with conversion from units to µU/mL
def u_inj(t, D, V_d, tau, t_inj):
    if t >= t_inj:
        return (D * 1e6 / V_d / tau) * np.exp(-(t - t_inj) / tau)
    else:
        return 0

# Basal insulin infusion in µU/mL
u_basal = R_b * 1e6 / V_d  # Adjusted basal insulin rate

# Define the ODE for plasma insulin concentration including basal and bolus insulin
def bergman_with_basal_and_bolus_insulin(I, t, n_I, D, tau, t_inj, u_basal):
    u = 0  # Endogenous insulin secretion (assumed to be 0)
    u_injection = u_inj(t, D, V_d, tau, t_inj)
    
    # Differential equation for insulin concentration
    dIdt = -n_I * I + u + u_injection + u_basal
    
    return dIdt

# Time points (from 0 to 120 minutes)
t = np.linspace(0, 120, 1000)

# Initial insulin concentration
I0 = 0  # Zero initial insulin

# Solve the ODE
I_solution = odeint(bergman_with_basal_and_bolus_insulin, I0, t, args=(n_I, D, tau, t_inj, u_basal))

# Plot the results
plt.plot(t, I_solution, label='Plasma Insulin Concentration')
plt.axvline(x=t_inj, color='r', linestyle='--', label='Bolus Injection')
plt.title('Plasma Insulin with Basal and Bolus Infusion (Adjusted)')
plt.xlabel('Time (minutes)')
plt.ylabel('Plasma Insulin Concentration (µU/mL)')
plt.legend()
plt.grid()
plt.show()
