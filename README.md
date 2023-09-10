
# Implementacion de metodos para estimacion de  primas americanas

## Como correr

Este repo tiene dos carpetas **CPP** y **PYTHON**, la carpeta con código en c++ solo tiene implementando el metodo de Broadie - Glaserman. La carpeta con código python tienen todos los metodos implementados.
pip install -r requirements.txt

### Python

cd python
pip install -r requirements.txt

python main.py

Se le presentara un menu para elegir que metodo utilizar y los parametros a seleccionar

(Fua que facil)

Por ejemplo, para el metodo de Broadie - Glasserman los parametros son:

- alpha = Confianza de que el precio real de la prima esta en el intervalo
(Confianza alpha => P(prima en el intervalo) = 1 - alpha)
- s0: Precio inicial del activo
- K: El strike
- b: La cantidad de ramas que se generan en el arbol
- N: La cantidad de oportunidades de ejercicio
- r: La de tasa continua anual de interes
- sigma: La desviacion estandar
- n: La cantidad de simulaciones
- T: Tiempo de expiracion, en años de la opcion

Con los parametros que estan hardcodeados (T=1,s0=100,K=100,sigma=0.1,r=0.05,option_type=C) la prima es 6.804957708822144
