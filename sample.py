import math
import numpy as np

#@title Funciones de generacion de muestras

#Como movimiento geometrico browneano
def generate_new_sample(sample,dt,sigma,r):
  sq_dt = math.sqrt(dt)
  Z = np.random.normal(loc=0,scale=sq_dt)
  new_sample = sample*math.exp((r - (sigma**2)/2) * dt + sigma*Z)
  return new_sample

def generate_negative_sample(prev_sample,sample,dt,sigma,r):
  exponential_term = sample / prev_sample
  divisor = exponential_term / math.exp((r - (sigma**2)/2) * dt) # e**(sigma*Z)
  negative_sample = sample / divisor**2  # sample* e**((r - (sigma**2)/2) * dt + sigma*Z - 2*sigma*Z) = sample* e**((r - (sigma**2)/2) * dt + sigma*(-Z))
  return negative_sample

#Para testear
def generate_dud_sample(sample,dt,sigma,r):
  return sample * 2
