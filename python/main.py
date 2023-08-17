import click
import math
import numpy as np
from scipy import stats
from time import perf_counter



from broadie_glasserman import broadie_glasserman_parallel
from finite_difference import explicit_finite_diference
from tilley import tilley
from helpers import estimate_option_price
METHODS = '123'


def common_data():
    click.echo('Ingrese los siguientes datos')
    s0 = click.prompt('Valor inicial de la opcion',type=float)
    strike = click.prompt('Valor del strike',type=float)
    T = click.prompt('Tiempo de expiracion (En años)', type=float)
    r = click.prompt('Tasa de interes anual',type = float)
    sigma = click.prompt('Parametro de volatilidad',type=float)
    type = click.prompt('Tipo de opción',type=click.Choice(['P','C']))
    return(s0,strike,T,r,sigma,type)

def b_s():
    s0,K,T,r,sigma,type = common_data()
    b = click.prompt('Numero de ramas',type=int)
    N = click.prompt('Cantidad de momentos de ejercicio',type=int)
    n = click.prompt('Cantidad de simulaciones',type=int)
    v_reduction = click.prompt('Reducir variancia?',type=click.Choice(['Y','N']))
    v_reduction = v_reduction == 'Y'
    alpha = 0.05
    z_alpha = stats.norm.ppf(1 - alpha/2)/math.sqrt(n)
    lower = []
    upper = []
    t2_start = perf_counter()
    for i in range(0,n):
      data = broadie_glasserman_parallel(b = b,N = N,T = T,s0 = s0,K = K,sigma = sigma,r = r,option_type = 'C',variance_reduction=v_reduction)
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
    print(f'Tiempo en transcurrido segundos: {t2_stop - t2_start}s')

def f_dif():
    s0,K,T,r,sigma,type = common_data()
    steps = click.prompt('Oportunidades de ejercicio',type= int)
    early_exercise = click.prompt('Opcion americana?',type=click.Choice(['Y','N']))
    early_exercise = early_exercise == 'Y'
    t1_start = perf_counter()
    grid = explicit_finite_diference(K,T,sigma,r,steps,type,early_exercise)
    price = estimate_option_price(s0,K,T,grid)
    t1_stop = perf_counter()
    print(f'Precio de la prima: {price}')
    print(f'Tiempo transcurrido en segundos: {t1_stop - t1_start}')


def _tilley():
    s0,K,T,r,sigma,type = common_data()
    steps = click.prompt('Oportunidades de ejercicio',type= int)
    Q = click.prompt('Cantidad de grupos de trayectorias',type = int)
    P = click.prompt('Trayectorias por grupo',type = int)
    N = click.prompt('Corridas totales de algoritmo',type= int)
    sharp_boundary = click.prompt('Refinar zona de trancision?',click.Choice(['Y','N']))
    sharp_boundary = sharp_boundary == 'N'
    prices = []
    t_start = perf_counter()
    for i in range(0,N):
        r = tilley(P,Q,s0,K,sigma,r,T,steps,type,sharp_boundary)
        prices.append(r)
    t_end = perf_counter()

    print(f'Precio estimado: {np.mean(prices)} Variancia: {np.std(prices)**2}')
    print(f'Tiempo transcurrido en segundos: {t_end - t_start}')
    


@click.command()
def method_selection():
    
    prompt = "\
    1) Broadie-Glasserman\n\
    2) Tilley\n\
    3) Diferencias Finitas\n\
Seleccione la opcion que quiera ejecutar"
    
    method = int(click.prompt(prompt,type=click.Choice(METHODS)))
    print(method)
    if method == 1:
        b_s()
    elif method == 2:
        _tilley()
    
    elif method == 3:
        f_dif()
    else:
        click.echo('Numero incorrecto')

if __name__=='__main__':
    method_selection()


    
