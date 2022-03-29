
from time import sleep, perf_counter
from threading import Thread

def proceso():
    print('Inicio Proceso...')
    sleep(2)
    print('Fin')

inicio = perf_counter()

#creo 2 procesos
t1 = Thread(target=proceso)
t2 = Thread(target=proceso)
t3 = Thread(target=proceso)
t4 = Thread(target=proceso)



#inicio los procesos (threads)
t1.start()
t2.start()
t3.start()
t4.start()


#espero que los procesos terminen
t1.join()
t2.join()
t3.join()
t4.join()

fin = perf_counter()

print(f'Los procesos tardaron {fin-inicio: 0.2f} segundos en completarse.')
