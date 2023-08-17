import math
import numpy as np
from helpers import *


       #addsg
#@title Metodo de Broadie-Glasserman con uso optimizado de memoria
  #Index vector muestra las elecciones tomadas en cada nivel de profundidad
  #ej: Con N = 3, b = 3 [0 , 0, 1, 2]
  #Representa:
  #                                                                                             s0
  #                                /                                                             |                                                              \
  #                              s11                                                            s12                                                            s13
  #                        /      |      \                                                /      |      \                                                /      |      \
  #                      /        |        \                                            /        |        \                                            /        |        \
  #                    /          |          \                                        /          |          \                                        /          |          \
  #                  /            |            \                                    /            |            \                                    /            |            \
  #                /              |              \                                /              |              \                                /              |              \
  #              /                |                \                            /                |                \                            /                |                \
  #            /                  |                  \                        /                  |                  \                        /                  |                  \
  #        s211                 s212                 s213                 s221                 s222                 s223                 s231                 s232                 s233
  #   /      |     \        /     |     \        /     |     \        /     |     \        /     |      \      /      |      \      /      |     \       /      |     \       /      |     \
  # s3111  s3112  s3113  s3121  s3122  s3123  s3131  s3132  s3133  s3211  s3212  s3213  s3221  s3222  s3223  s3231  s3232  s3233  s3311  s3312  s3313  s3321  s3322  s3323  s3331  s3332  s3333
  #                                     ^^^^
  #                                   Este nodo

def broadie_glasserman_optimized(b,N,T,s0,K,sigma,r,option_type,variance_reduction=False):
  discount_rate = math.exp(-r * T/N)
  dt = T/N
  dynamic_matrix = np.zeros([b,N + 1],dtype = [('val',float),('sTheta',float),('bTheta',float)])
  index_vector = np.zeros(N + 1,dtype=int)
  dynamic_matrix[0,0]['val'] = s0


  for j in range(1,N + 1):
    dynamic_matrix[0,j]['val'] = generate_new_sample(dynamic_matrix[0,j-1]['val'],dt,sigma,r)
  step = N

  while(step >= 0):
    #Asegurarnos que este valor no se toca
    assert index_vector[0] == 0

    #Guardamos la rama actual y el valor actual del nodo que estamos revisando
    branch = index_vector[step]
    val = dynamic_matrix[branch,step]['val']

    if step == N and branch < b - 1:
      #En las hojas del arbol calculamos los estimadores como el valor de ejercer la opcion
      dynamic_matrix[branch,step]['sTheta'] = excercise_option(val,K,option_type)
      dynamic_matrix[branch,step]['bTheta'] = excercise_option(val,K,option_type)
      #Encontramos el padre en la matriz
      father_val = dynamic_matrix[index_vector[step-1],step - 1]['val']
      #Generamos el nodo hermano del que estamos revisando en base al padre
      if branch % 2 == 0 and variance_reduction:
        dynamic_matrix[branch + 1, step]['val'] =  generate_negative_sample(father_val,val,dt,sigma,r)
      else:
        dynamic_matrix[branch + 1, step]['val'] =  generate_new_sample(father_val,dt,sigma,r)

      index_vector[step] += 1
    elif step == N and branch == b - 1:
      #Como este es la ultima hoja de un nodo, solo calculamos los thetas sin generar otro valor
      dynamic_matrix[branch,step]['sTheta'] = excercise_option(val,K,option_type)
      dynamic_matrix[branch,step]['bTheta'] = excercise_option(val,K,option_type)

      index_vector[step] = -1
      step -= 1
    elif step < N  and branch < b - 1:
      #Guardamos los estimadores de los hijos
      children_sThetas = dynamic_matrix[:,step+1]['sTheta']
      children_bThetas = dynamic_matrix[:,step+1]['bTheta']
      #Calculamos los estimadores respectivos de este nodo en base a los estimadores de los hijos
      dynamic_matrix[branch,step]['sTheta'] = calculate_sTheta(val,children_sThetas,K,discount_rate,option_type)
      dynamic_matrix[branch,step]['bTheta'] = calculate_bTheta(val,children_bThetas,K,discount_rate,option_type)

      if step > 0:
        #Si no estamos en la base del arbol, generamos una trayectoria entera hasta la hoja
        father_val = dynamic_matrix[index_vector[step-1],step - 1]['val']
        if branch % 2 == 0 and variance_reduction:
          dynamic_matrix[branch + 1, step]['val'] =  generate_negative_sample(father_val,val,dt,sigma,r)
        else:
          dynamic_matrix[branch + 1, step]['val'] =  generate_new_sample(father_val,dt,sigma,r)
        index_vector[step] += 1
        prev_val = dynamic_matrix[branch + 1,step]['val']
        for k in range(step + 1,N):
          dynamic_matrix[0,k]['val'] = generate_new_sample(prev_val,dt,sigma,r)
          prev_val = dynamic_matrix[0 ,k]['val']
          index_vector[k] = 0
        step = N
      else:
        step -= 1
    elif step < N  and branch == b - 1:

      children_sThetas = dynamic_matrix[:,step+1]['sTheta']
      children_bThetas = dynamic_matrix[:,step+1]['bTheta']

      dynamic_matrix[branch,step]['sTheta'] = calculate_sTheta(val,children_sThetas,K,discount_rate,option_type)
      dynamic_matrix[branch,step]['bTheta'] = calculate_bTheta(val,children_bThetas,K,discount_rate,option_type)

      index_vector[step] = -1
      step = step - 1

  return(dynamic_matrix[0,0]['sTheta'], dynamic_matrix[0,0]['bTheta'])