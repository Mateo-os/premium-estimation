### Implementacion del metodo de Broadie-Glaserman para intervalos de confianza para primas americanas

### Como correr

pip install -r requirements.txt

python main.py

(Fua que facil)

Los parametros son:

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
