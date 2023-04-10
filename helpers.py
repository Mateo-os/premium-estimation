import numpy as np


#@title Funciones que ejecutan los calculos
#funcion para ejercitar una opcion, ya sea put o call
def excercise_option(val,K,op_type):
  if op_type == 'C':
    return max(val-K,0)
  if op_type == 'P':
    return  max(K-val,0)
  else:
    raise Exception


#calculo de la cota superior o 'Big Theta'
def calculate_bTheta(val,children_thetas,K,discount_rate,op_type):
  excercise_value = excercise_option(val,K,op_type)
  holding_value = np.mean(children_thetas) * discount_rate
  return max(excercise_value,holding_value)

#Calculo de la cota inferior o 'Small Theta'
def calculate_sTheta(val,children_thetas,K,discount_rate,op_type):
  excercise_value = excercise_option(val,K,op_type)
  estimated_payoffs = []
  children_sum = np.sum(children_thetas)
  for child in children_thetas:
    associated_value = ((children_sum - child)/(len(children_thetas) - 1)) * discount_rate
    value = child * discount_rate if excercise_value < associated_value else excercise_value
    estimated_payoffs.append(value)
  return np.mean(estimated_payoffs)