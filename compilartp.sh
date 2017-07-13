#!/bin/bash

cd --
git clone https://github.com/sisoputnfrba/so-commons-library
cd so-commons-library

sudo make install

cd --

git clone https://github.com/sisoputnfrba/ansisop-parser

cd ansisop-parser/parse

make all
sudo make install

cd --

echo "Commons y Parser listos"

cd /home/utnso/tp-2017-1c-TodaviaSirve/evaluacion-final-esther/FS-ejemplo

tar -xzvf FS.tgz

cd --

mv /home/utnso/tp-2017-1c-TodaviaSirve/evaluacion-final-esther/FS-ejemplo /home/utnso/FS_SADICA

cd /home/utnso/tp-2017-1c-TodaviaSirve/Consola/src

gcc Consola.c escuchar_mensaje.c hilo_programa.c hilo_usuario.c loc.c mensaje.c parametros.c socket_cliente.c escuchar_mensaje.h hilo_prorama.h estructuras.h hilo_usuario.h log.h mensaje.h parametros.h socket_cliente.h -o Consola -lcommons -lpthread

echo "Consola compilada, ejecutar normalmente"

cd --

cd /home/utnso/tp-2017-1c-TodaviaSirve/CPU/src

gcc CPU.c cosas.c funcionesCPU.c funcionesParser.c log.c mensajes.c socket.c cosas.h estructuras.h funcionesCPU.h funcionesParser.h log.h mensajes.h socket.h -o CPU -lcommons -lparser-ansisop 

cd --

echo "CPU compilada, ejecutar normalmente"

cd /home/utnso/tp-2017-1c-TodaviaSirve/FileSystem/src

gcc FileSystem.c archivos.c fsecundarias.c funciones.c log.c manejor_errores.c mensaje.c socket.c archivos.h estructuras.h fsecundarias.h funciones.h log.h manejo_errores.h mensaje.h socket.h -o FileSystem -lcommons -lpthread

cd --

echo "FileSystem compilado, ejecutar normalmente"

cd /home/utnso/tp-2017-1c-TodaviaSirve/Kernel/src

gcc Kernel.c configuracion.c consolaKernel.c consolaManager.c cpuManager.c filesystem.c log.c manejo_conexiones.c manejo_errores.c memoria.c memoria2.c mensaje_consola.c mensaje.c metadata.c planificador.c semaforos_vglobales.c socket.c configuracion.h consolaKernel.h consolaManager.h cpuManager.h estructuras.h filesystem.h log.h manejo_conexiones.h manejo_errores.h memoria.h memoria2.h mensaje_consola.h mensaje.h metadata.h planificador.h semaforos_vglobales.h socket.h -o Kernel -lcommons -lpthread -lparser-ansisop

cd --

echo "Kernel compilado, ejecutar normalmente"

cd /home/utnso/tp-2017-1c-TodaviaSirve/Memoria/src

gcc Memoria.c consola_memoria.c hash.c log.c manejo_errores.c mensaje.c socket.c consola_memoria.h estructuras.h hash.h log.h manejo_errores.h mensaje.h socket.h -o Memoria -lcommons -lpthread

cd --

echo "Memoria compilada, ejecutar normalmente"

echo "Todo listo para comenzar la prueba, exitos!"
