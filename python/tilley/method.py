import math
import numpy as np

from helpers import *


def tilley(P, Q, s0, K, sigma, r, T, N, op_type, sharp_boundary=True):
    if op_type not in "CP":
        return
    R = P * Q
    dt = T / N
    discount_rate = math.exp(-r * dt)
    indexes = np.array(list(range(0, R)))
    stocks = np.ndarray(shape=[R, N], dtype=[("val", float), ("current", float)])
    true_indicator = np.zeros(shape=[R, N])
    for path in range(0, R):
        prev = s0
        for step in range(0, N):
            prev = generate_new_sample(prev, dt, sigma, r)
            stocks[path, step]["val"] = prev
        stocks[path, N - 1]["current"] = excercise_option(prev, K, op_type)

    for step in reversed(range(0, N - 1)):
        sort_key = lambda x: stocks[x, step]["val"]
        # Ordenamos en cada paso los valores de la trayectoria de mayor a menor
        indexes = np.array(sorted(indexes, key=sort_key, reverse=(op_type == "P")))
        tentative_indicator = np.full(shape=[R], fill_value=-1)
        holding_values = np.zeros(Q)

        for bundle in range(0, Q):
            bundle_currents = []
            for path in range(0, P):
                path_index = indexes[bundle * P + path]
                current_value = stocks[path_index, step + 1]["current"]
                bundle_currents.append(current_value)
            # En cada agrupamiento se calcula el valor de no ejercer para ese agrupamiento
            holding_value = discount_rate * np.mean(bundle_currents)
            holding_values[bundle] = holding_value
            for path in range(0, P):
                # En base a el valor de no ejercer del agrupamiento se actualiza
                # un indicador tentativo para cada trayectoria de este agrupamiento
                path_index = indexes[bundle * P + path]
                val = stocks[path_index, step]["val"]
                tentative_indicator[bundle * P + path] = (
                    1 if excercise_option(val, K, op_type) > holding_value else 0
                )

        boundary = R
        i = R - 1
        max_zero_len = 0
        while i >= 0:
            j = i
            if tentative_indicator[i] == 1:
                one_len = 0
                while tentative_indicator[j] == 1 and j >= 0:
                    one_len += 1
                    j -= 1
                if max_zero_len < one_len:
                    boundary = j + 1
                i = j
            else:
                zero_len = 0
                while tentative_indicator[j] == 0 and j >= 0:
                    zero_len += 1
                    j -= 1
                max_zero_len = max(zero_len, max_zero_len)
            i = j

        sharp_boundary_indicator = np.zeros(R)
        for i in range(boundary, R):
            sharp_boundary_indicator[i] = 1

        indicator = sharp_boundary_indicator if sharp_boundary else tentative_indicator

        for bundle in range(0, Q):
            holding_value = holding_values[bundle]
            for path in range(0, P):
                path_index = indexes[bundle * P + path]
                val = stocks[path_index, step]["val"]
                stocks[path_index, step]["current"] = (
                    excercise_option(val, K, op_type)
                    if indicator[bundle * P + path] == 1
                    else holding_value
                )

        # El primer 1 del indicador de frontera indica que en ese paso se ejerce
        if len(np.where(indicator == 1)[0]) > 0:
            first_one = np.where(indicator == 1)[0][0]
            true_indicator[indexes[first_one], step] = 1
            for j in range(step + 1, N):
                true_indicator[indexes[first_one], j] = 0

    estimator = 0
    for path in range(0, R):
        step = next(
            (step for step in range(0, N) if true_indicator[path, step] == 1), N - 1
        )
        val = excercise_option(stocks[path, step]["val"], K, op_type)
        estimator += math.exp(-r * (step + 1) * dt) * val
    return estimator / R
