import numpy as np


# @title Funciones que ejecutan los calculos
# funcion para ejercitar una opcion, ya sea put o call
def excercise_option(val, K, op_type):
    if op_type == "C":
        return max(val - K, 0)
    if op_type == "P":
        return max(K - val, 0)
    else:
        raise Exception


# calculo de la cota superior o 'Big Theta'
def calculate_bTheta(val, children_thetas, K, discount_rate, op_type):
    excercise_value = excercise_option(val, K, op_type)
    holding_value = np.mean(children_thetas) * discount_rate
    return max(excercise_value, holding_value)


# Calculo de la cota inferior o 'Small Theta'
def calculate_sTheta(val, children_thetas, K, discount_rate, op_type):
    excercise_value = excercise_option(val, K, op_type)
    estimated_payoffs = []
    children_sum = np.sum(children_thetas)
    for child in children_thetas:
        associated_value = (
            (children_sum - child) / (len(children_thetas) - 1)
        ) * discount_rate
        value = (
            child * discount_rate
            if excercise_value < associated_value
            else excercise_value
        )
        estimated_payoffs.append(value)
    return np.mean(estimated_payoffs)


# Estimar el precio de la opcion segun la matriz de diferencuas finitas
def estimate_option_price(s0, K, T, grid):
    S_steps, T_steps = grid.shape
    ds = 2 * K / S_steps
    dt = T / T_steps
    index = int(s0 / ds)
    if index * ds == s0:
        return grid[index, T_steps - 1]
    v1 = grid[index + 1, T_steps - 1]
    v2 = grid[index, T_steps - 1]

    a2 = ((index + 1) * ds - s0) * dt
    a1 = (s0 - index * ds) * dt
    return (v1 * a1 + v2 * a2) / (a1 + a2)
