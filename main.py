import math
import numpy as np

from scipy import stats
from time import perf_counter

from optimized import broadie_glasserman_optimized
from parallel import broadie_glasserman_parallel

if __name__ == "__main__":
    s0 = 100
    K = 100
    sigma = 0.1
    T = 1
    r = 0.05
    b = 50
    N = 4
    n = 10
    alpha = 0.05
    z_alpha = stats.norm.ppf(1 - alpha/2)/math.sqrt(n)
    lower = []
    upper = []
    print('-------------------OPTIMIZADO-------------------------')
    t2_start = perf_counter()
    for i in range(0,n):
      data = broadie_glasserman_optimized(b = b,N = N,T = T,s0 = s0,K = K,sigma = sigma,r = r,option_type = 'C',variance_reduction=True)
      print(f'{i}: {data}')
      lower.append(data[0])
      upper.append(data[1])
    t2_stop = perf_counter()
    sTheta = np.mean(lower)
    bTheta = np.mean(upper)
    sstd = np.std(lower)
    bstd = np.std(upper)
    print(f'Variancia sTheta:{sstd}. Variancia bTheta: {bstd} ')
    interval = (max(max(s0 - K,0), sTheta -  sstd * z_alpha ), bTheta + bstd * z_alpha )
    print(f'Intervalo: {interval}')
    print(f'Tiempo en segundos: {t2_stop - t2_start}s')
    print('-------------------PARALELIZADO-------------------------')
    lower = []
    upper = []
    t2_start = perf_counter()
    for i in range(0,n):
      data = broadie_glasserman_parallel(b = b,N = N,T = T,s0 = s0,K = K,sigma = sigma,r = r,option_type = 'C',variance_reduction=True)
      print(f'{i}: {data}')
      lower.append(data[0])
      upper.append(data[1])
    t2_stop = perf_counter()
    sTheta = np.mean(lower)
    bTheta = np.mean(upper)
    sstd = np.std(lower)
    bstd = np.std(upper)
    print(f'Variancia sTheta:{sstd}. Variancia bTheta: {bstd} ')
    interval = (max(max(s0 - K,0), sTheta -  sstd * z_alpha ), bTheta + bstd * z_alpha )
    print(f'Intervalo: {interval}')
    print(f'Tiempo en segundos: {t2_stop - t2_start}s')
