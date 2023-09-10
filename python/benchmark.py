import math
import numpy as np

from scipy import stats
from time import perf_counter

from broadie_glasserman import broadie_glasserman_optimized, broadie_glasserman_parallel

POSSIBLE_B = [4, 8, 16,24,36,48,50]
POSSIBLE_N = [10,20,40,50,60,70,100]

OPTION_DATA = {
    "N":4,
    "T":1,
    "s0":100,
    "K":100,
    "r":0.05,
    "sigma":0.1,
    "option_type":"C",
    "variance_reduction":True,
    "alpha": 0.05
}

def broadie_glasserman_full(iterations,branches,parallel = True):
    alpha = OPTION_DATA.pop("alpha")
    s0 = OPTION_DATA["s0"]
    K = OPTION_DATA["K"]
    z_alpha = stats.norm.ppf(1 - alpha/2)/math.sqrt(iterations)
    broadie = broadie_glasserman_parallel if parallel else broadie_glasserman_optimized
    lower = np.zeros(iterations)
    upper = np.zeros(iterations)
    for i in range(0,iterations):
        data = broadie(b = branches,**OPTION_DATA)
        lower[i] = data[0]
        upper[i] = data[1]
    sTheta = np.mean(lower)
    bTheta = np.mean(upper)
    sstd = np.std(lower)
    bstd = np.std(upper)
    interval = (max(max(s0 - K,0), sTheta -  sstd * z_alpha ), bTheta + bstd * z_alpha )
    return (interval,sTheta, bTheta)


if __name__ == "__main__":
    data = np.zeros([2,2,6,1,1,1,1],
                    dtype=[('test_type',int),('fixed_val',int),('val',int),("sTheta",float),
                           ("bTheta",float),("lowerBound",float),("upperBound",float),])
    print("Non parallel test")
    for fixed_val in range(2):
        test_prompt = "Fixed n test" if fixed_val else "Fixed b test"
        print(test_prompt)
        n = 50
        b = 16
        iterator = POSSIBLE_B if fixed_val else POSSIBLE_N
        for val in iterator:
            n,b = n,val if fixed_val else val,b
            test_prompt = f'braches: {b}' if fixed_val else f'iterations: {n}'
            print(test_prompt)
            res = broadie_glasserman_full(iterations=n,branches=b,parallel=False)
