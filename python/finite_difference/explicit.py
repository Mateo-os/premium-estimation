import numpy as np 
from helpers import excercise_option

def explicit_finite_diference(K,T, sigma,r,asset_steps,op_type,early_excercise=False):

  ds = 2*K/asset_steps                      # 2*K reprensenta el infinito
  dt = 0.9/ (sigma*asset_steps)**2          # En el paper toma este dt como medida de estabilidad para que el metodo converga
  time_steps = int(T/dt) + 1
  print(f'TIME STEPS: {time_steps} ')
  print(f'dS: {ds} ')
  dt = T/time_steps                         # Se modifica dt para que dt * time_steps = T
  print(f'dt {dt}')

  asset_array = np.zeros(asset_steps)
  grid = np.zeros([asset_steps,time_steps + 1]) # grid[i,j] contiene el valor de la opcion cuando 
                                            # el subyacente vale i*ds en el tiempo T - j*dt 
  payoff = np.zeros(asset_steps)

  for i in range(0,asset_steps):
    val = i * ds
    asset_array[i] = val 
    grid[i,0] = excercise_option(val,K,op_type) # Valor de la opcion en tiempo T, se ejerce la opcion
    payoff[i] = grid[i,0]


  for j in range(1,time_steps + 1):
    for i in range(1,asset_steps - 1):
      s = asset_array[i]
      
      delta = (grid[i + 1, j - 1] - grid[i - 1,j - 1])/(2*ds)                       # Estimacion por diferencia central
      gamma = (grid[i + 1, j - 1] - 2*grid[i, j-1] + grid[i - 1, j - 1])/(ds**2)    # Estimacion por diferencia central  
      theta = -0.5 * (sigma**2) * s**2 * gamma - r * s * delta + r *  grid[i, j -1] # Ecuacion de Black - Scholes

      grid[i,j] = grid[i, j - 1] - dt * theta  # Theta es la derivada de la opcion respecto del tiempo 
                                               # si el precio se mantiene y el tiempo retrocede en dt 
                                               # el precio de la opcion se  reduce en dt * theta

    grid[0,j] = grid[0, j - 1] * (1 - r) * dt                                         # Condicion de borde en S = 0
    grid[asset_steps - 1, j] = 2 * grid[asset_steps - 2,j] - grid[asset_steps - 3,j]  # Condicion de borde en S = Inf = 2 * K

    if early_excercise:
      for i in range(0,asset_steps):
        grid[i,j] = max(grid[i,j],payoff[i])


  return grid  