import math
import numpy as np
import multiprocessing
from multiprocessing import shared_memory

from helpers import *
from sample import *

def broadie_glasserman_individual_matrix(memory_data,b,N,T,s0,K,r,sigma,option_type,variance_reduction=True):

  memory = shared_memory.SharedMemory(name=memory_data['name'])
  matrix = np.ndarray(shape=memory_data['shape'],dtype=memory_data['type'],buffer=memory.buf)[memory_data['index']]
  dt = T/N
  discount_rate = math.exp(-r * dt)
  step = N - 1
  index_vector = np.zeros(N,dtype=int)
  while(step>=0):

    branch = index_vector[step]
    val = matrix[branch,step]['val']
    if step == N - 1 and branch < b - 1:
      matrix[branch,step]['bTheta'] = excercise_option(val,K,option_type)
      matrix[branch,step]['sTheta'] = excercise_option(val,K,option_type)
      father_val = matrix[index_vector[step - 1],step - 1]['val']

      if branch % 2 == 0 and variance_reduction:
        matrix[branch + 1,step]['val'] = generate_negative_sample(father_val,val,dt,sigma,r)
      else:
        matrix[branch + 1,step]['val'] = generate_new_sample(father_val,dt,sigma,r)
      index_vector[step] += 1
    elif step == N - 1 and branch == b-1:

      matrix[branch,step]['bTheta'] = excercise_option(val,K,option_type)
      matrix[branch,step]['sTheta'] = excercise_option(val,K,option_type)

      index_vector[step] = -1
      step -= 1

    elif step < N - 1 and branch < b-1:
      #Guardamos los estimadores de los hijos
      children_sThetas = matrix[:,step+1]['sTheta']
      children_bThetas = matrix[:,step+1]['bTheta']
      #Calculamos los estimadores respectivos de este nodo en base a los estimadores de los hijos
      matrix[branch,step]['sTheta'] = calculate_sTheta(val,children_sThetas,K,discount_rate,option_type)
      matrix[branch,step]['bTheta'] = calculate_bTheta(val,children_bThetas,K,discount_rate,option_type)

      if step > 0:
        #Si no estamos en la base del arbol, generamos una trayectoria entera hasta la hoja
        father_val = matrix[index_vector[step-1],step - 1]['val']
        if branch % 2 == 0 and variance_reduction:
          matrix[branch + 1, step]['val'] =  generate_negative_sample(father_val,val,dt,sigma,r)
        else:
          matrix[branch + 1, step]['val'] =  generate_new_sample(father_val,dt,sigma,r)
        index_vector[step] += 1
        prev_val = matrix[branch + 1,step]['val']
        for k in range(step + 1,N):
          matrix[0,k]['val'] = generate_new_sample(prev_val,dt,sigma,r)
          prev_val = matrix[0 ,k]['val']
          index_vector[k] = 0
        step = N - 1
      else:
        step -= 1

    elif step < N-1  and branch == b - 1:

      children_sThetas = matrix[:,step+1]['sTheta']
      children_bThetas = matrix[:,step+1]['bTheta']

      matrix[branch,step]['sTheta'] = calculate_sTheta(val,children_sThetas,K,discount_rate,option_type)
      matrix[branch,step]['bTheta'] = calculate_bTheta(val,children_bThetas,K,discount_rate,option_type)

      index_vector[step] = -1
      step = step - 1

  return matrix


def broadie_glasserman_parallel(b,N,T,s0,K,r,sigma,option_type,variance_reduction=False):
  dt = T/N
  sq_dt = math.sqrt(dt)

  dummy = np.zeros([b,b,N],dtype=[('val',float),('sTheta',float),('bTheta',float)])
  shm = shared_memory.SharedMemory(create=True,size=dummy.nbytes)
  dynamic_matrix = np.ndarray(shape=dummy.shape,dtype=dummy.dtype,buffer=shm.buf)
  dummy = None

  memory_data = {
    'name':shm.name,
    'shape':[b,b,N],
    'type':[('val',float),('sTheta',float),('bTheta',float)],
  }

  processes = []
  for h in range(0,b):
    if (h % 2 == 1) and variance_reduction:
      last_s1 = dynamic_matrix[h-1,0,0]['val']
      dynamic_matrix[h,0,0]['val'] = generate_negative_sample(s0,last_s1,dt,sigma,r)
    else:
      dynamic_matrix[h,0,0]['val'] = generate_new_sample(s0,dt,sigma,r)
    prev_val = dynamic_matrix[h,0,0]['val']
    for j in range(1,N):
      dynamic_matrix[h,0,j]['val'] = generate_new_sample(prev_val,dt,sigma,r)
      prev_val = dynamic_matrix[h,0,j]['val']
    memory_data['index'] = h
    p = multiprocessing.Process(target=broadie_glasserman_individual_matrix,args=[memory_data,b,N,T,s0,K,r,sigma,option_type,variance_reduction])
    processes.append(p)
    p.start()

  for p in processes:
    p.join()
  sThetas = dynamic_matrix[:,0,0]['sTheta']
  bThetas = dynamic_matrix[:,0,0]['bTheta']

  discount_rate = math.exp(-r * dt)
  sTheta = calculate_sTheta(s0,sThetas,K,discount_rate,option_type)
  bTheta = calculate_bTheta(s0,bThetas,K,discount_rate,option_type)

  return(sTheta,bTheta)