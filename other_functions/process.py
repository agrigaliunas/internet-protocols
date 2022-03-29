from time import sleep, perf_counter
from threading import Thread

def proceso():
    print('Inicio Proceso...')
    sleep(1)
    print('Fin')


inicio = perf_counter()

proceso()
proceso()

fin = perf_counter()

print(f'Los procesos tardaron {fin - inicio: 0.2f} segundos en completarse.')
