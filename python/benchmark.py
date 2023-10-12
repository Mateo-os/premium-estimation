import math
import numpy as np
import pandas as pd

from scipy import stats
from time import perf_counter

from broadie_glasserman import broadie_glasserman_optimized, broadie_glasserman_parallel

POSSIBLE_B = [4,  8, 16,32,50]
POSSIBLE_N = [10, 30,50,70,100]
DATA_LEN = len(POSSIBLE_B)

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
    alpha = OPTION_DATA["alpha"]
    function_data = OPTION_DATA.copy()
    function_data.pop("alpha")
    s0 = OPTION_DATA["s0"]
    K = OPTION_DATA["K"]
    z_alpha = stats.norm.ppf(1 - alpha/2)/math.sqrt(iterations)
    broadie = broadie_glasserman_parallel if parallel else broadie_glasserman_optimized
    lower = np.zeros(iterations)
    upper = np.zeros(iterations)
    for i in range(0,iterations):
        data = broadie(b = branches,**function_data)
        lower[i] = data[0]
        upper[i] = data[1]
    sTheta = np.mean(lower)
    bTheta = np.mean(upper)
    sstd = np.std(lower)
    bstd = np.std(upper)
    interval = (max(max(s0 - K,0), sTheta -  sstd * z_alpha ), bTheta + bstd * z_alpha )
    return (sTheta, sstd, bTheta, bstd, interval)


if __name__ == "__main__":
    data = np.zeros([2,2,DATA_LEN, 8])
    paralell = 1
    test_prompt = "Non parallel test" if not paralell else "Parralel test"
    print(test_prompt)
    fixed_val = 1
    test_prompt = "Fixed n test" if fixed_val else "Fixed b test"
    print(test_prompt)
    n = 100
    b = 30
    iterator = POSSIBLE_B if fixed_val else POSSIBLE_N
    for i in range(DATA_LEN):
        val = iterator[i]
        n,b = (n,val) if fixed_val else (val,b)
        test_prompt = f'branches: {b}' if fixed_val else f'iterations: {n}'
        print(test_prompt)
        t_start = perf_counter()
        res = broadie_glasserman_full(iterations=n,branches=b,parallel= paralell)
        t_end = perf_counter()
        data[paralell,fixed_val,i,0] = val
        data[paralell,fixed_val,i,1] = res[0]
        data[paralell,fixed_val,i,2] = res[1]
        data[paralell,fixed_val,i,3] = res[2]
        data[paralell,fixed_val,i,4] = res[3]
        data[paralell,fixed_val,i,5] = res[4][0]
        data[paralell,fixed_val,i,6] = res[4][1]
        data[paralell,fixed_val,i,7] = t_end - t_start
    df = pd.DataFrame(data[paralell,fixed_val])
    non_fixed_val = 'b' if fixed_val else 'n'
    fixed_val = 'n' if fixed_val else 'b'
    csv_name = f"{'non_' if not paralell else ''}paralell_fixed_{fixed_val}.csv"
    header = [ non_fixed_val,
                "lower estimator",
                "lower estimator variance",
                "upper estimator",
                "upper estimator variance",
                "lower bound",
                "upper bound",
                "elapsed time"]
    df.to_csv(csv_name,header=header)