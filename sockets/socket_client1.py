#!/usr/bin/python

import socket
import sys

# Creando el socket TCP/IP
sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

# Enlace de socket y puerto
server_dir = ('localhost', 9879)
print ('empezando a levantar',server_dir)
sock.bind(server_dir)

# Escuchando conexiones entrantes
sock.listen(1)

while True:
    # Esperando conexion
    print ('Esperando para conectarse')
    conexion, client_dir = sock.accept()

    try:
        print ('conexion desde', client_dir)

        # Recibe los datos en trozos y los retransmito al cliente
        while True:
            data = conexion.recv(10)
            print ('recibido "%s"',data)
            if data:
                print ('enviando mensaje de vuelta al cliente')
                conexion.sendall(data)
            else:
                print ('no hay mas datos', client_dir)
                break

    finally:
        # Cerrando conexion
        conexion.close()
