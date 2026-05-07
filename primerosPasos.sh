#!/bin/bash
####### Test Nivel 0
if [[ $1 -eq 0 ]]; then
printf "./practica2 -m ./mapas/mapa30.map -n 0 -i 17 5 0 -t 17 17 0 -seed 0 -Tiempo 3000 -Ambiental 1000 -Energia 3000\n"
./practica2SG -m ./mapas/mapa30.map -n 0 -i 17 5 0 -t 17 17 0 -seed 0 -Tiempo 3000 -Ambiental 1000 -Energia 3000
# Los dos en casillas 'U'
printf "./practica2 -m ./mapas/mapa30.map -n 0 -i 24 7 2 -t 17 17 0 -seed 0 -Tiempo 3000 -Ambiental 1000 -Energia 3000\n"
./practica2SG -m ./mapas/mapa30.map -n 0 -i 24 7 2 -t 17 17 0 -seed 0 -Tiempo 3000 -Ambiental 1000 -Energia 3000
# Los dos en casillas 'U'
printf "./practica2 -m ./mapas/mapa30.map -n 0 -i 24 10 2 -t_17_17_9586598586598586598586598586598586598586598586598586598586598586598586598586598586598586598586598586598586598586\n"
./practica2SG -m ./mapas/mapa30.map -n 0 -i 24 10 2 -t_17_17_9586598586598586598586598586598586598586598586598586598586598586598586598586598586598586598586598586598586598586
# Los dos en casillas 'U'
printf "./practica2 -m ./mapas/mapa30.map -n 0 -i 16 9 2 -t 16 14 6 -seed 0 -Tiempo 3000 -Ambiental 1000 -Energia 3000\n"
./practica2SG -m ./mapas/mapa30.map -n 0 -i 16 9 2 -t 16 14 6 -seed 0 -Tiempo 3000 -Ambiental 1000 -Energia 3000
# Los dos en casillas 'U'
printf "./practica2 -m ./mapas/mapa50.map -n 0 -i 38 10 2 -t 40 24 6 -seed 0 -Tiempo 3000 -Ambiental 1000 -Energia 3000\n"
./practica2SG -m ./mapas/mapa50.map -n 0 -i 38 10 2 -t 40 24 6 -seed 0 -Tiempo 3000 -Ambiental 1000 -Energia 3000
# Los dos en casillas 'U'
printf "./practica2 -m ./mapas/mapa75.map -n 0 -i 69 3 2 -t 40 24 6 -seed 0 -Tiempo 3000 -Ambiental 1000 -Energia 3000\n"
./practica2SG -m ./mapas/mapa75.map -n 0 -i 69 3 2 -t 40 24 6 -seed 0 -Tiempo 3000 -Ambiental 1000 -Energia 3000
# Los dos en casillas 'U'

elif [[ $1 -eq 1 ]]; then 

####### Test Nivel 1
printf "./practica2 -m ./mapas/minivertiguito.map -n 1 -i 7 4 0 -t 24 24 7 -seed 0 -Tiempo 3000 -Ambiental 1000 -Energia 3000\n"
./practica2SG -m ./mapas/minivertiguito.map -n 1 -i 7 4 0 -t 24 24 7 -seed 0 -Tiempo 3000 -Ambiental 1000 -Energia 3000
# Maximo porcentaje posible de caminos y senderos
printf "./practica2 -m ./mapas/minivertiguito.map -n 1 -i 7 4 0 -t 24 24 7 -seed 0 -Tiempo 3000 -Ambiental 1000 -Energia 1000\n"
./practica2SG -m ./mapas/minivertiguito.map -n 1 -i 7 4 0 -t 24 24 7 -seed 0 -Tiempo 3000 -Ambiental 1000 -Energia 1000
# Maximo porcentaje posible de caminos y senderos
printf "./practica2 -m ./mapas/gemini2.map -n 1 -i 20 8 0 -t 13 13 3 -seed 0 -Tiempo 3000 -Ambiental 1000 -Energia 3000\n"
./practica2SG -m ./mapas/gemini2.map -n 1 -i 20 8 0 -t 13 13 3 -seed 0 -Tiempo 3000 -Ambiental 1000 -Energia 3000
# Maximo porcentaje posible de caminos y senderos
printf "./practica2 -m ./mapas/mapa30.map -n 1 -i 16 7 7 -t 17 16 4 -seed 0 -Tiempo 3000 -Ambiental 1000 -Energia 3000\n"
./practica2SG -m ./mapas/mapa30.map -n 1 -i 16 7 7 -t 17 16 4 -seed 0 -Tiempo 3000 -Ambiental 1000 -Energia 3000
# Maximo porcentaje posible de caminos y senderos
printf "./practica2 -m ./mapas/mapa50.map -n 1 -i 38 10 2 -t 40 24 6 -seed 0 -Tiempo 3000 -Ambiental 1000 -Energia 3000\n"
./practica2SG -m ./mapas/mapa50.map -n 1 -i 38 10 2 -t 40 24 6 -seed 0 -Tiempo 3000 -Ambiental 1000 -Energia 3000
# Maximo porcentaje posible de caminos y senderos
printf "./practica2 -m ./mapas/mapa50_cuadricula.map -n 1 -i 45 25 0 -t 45 26 0 -seed 0 -Tiempo 3000 -Ambiental 1000 -Energia 3000\n"
./practica2SG -m ./mapas/mapa50_cuadricula.map -n 1 -i 45 25 0 -t 45 26 0 -seed 0 -Tiempo 3000 -Ambiental 1000 -Energia 3000
# Maximo porcentaje posible de caminos y senderos
printf "./practica2 -m ./mapas/bosque_prohibido.map -n 1 -i 4 11 0 -t 9 10 2 -seed 0 -Tiempo 3000 -Ambiental 1000 -Energia 3000\n"
./practica2SG -m ./mapas/bosque_prohibido.map -n 1 -i 4 11 0 -t 9 10 2 -seed 0 -Tiempo 3000 -Ambiental 1000 -Energia 3000
# Maximo porcentaje posible de caminos y senderos

else 

####### Test Nivel 2
./practica2SG -m ./mapas/vertigo.map -n 2 -i 56 87 6 -t 56 87 0 -seed 0 -Tiempo 15000 -Ambiental 1000 -Energia 15000 -O 52 85
# 4
./practica2SG -m ./mapas/vertigo.map -n 2 -i 50 71 5 -t 50 71 0 -seed 0 -Tiempo 15000 -Ambiental 1000 -Energia 15000 -O 87 88
# 28
./practica2SG -m ./mapas/vertigo.map -n 2 -i 64 96 4 -t 64 96 0 -seed 0 -Tiempo 15000 -Ambiental 1000 -Energia 15000 -O 47 17
# 49
./practica2SG -m ./mapas/vertigo.map -n 2 -i 26 96 7 -t 26 96 0 -seed 0 -Tiempo 15000 -Ambiental 1000 -Energia 15000 -O 41 75
# 24
./practica2SG -m ./mapas/vertigo.map -n 2 -i 66 87 2 -t 66 87 0 -seed 0 -Tiempo 15000 -Ambiental 1000 -Energia 15000 -O 82 43
# 41
./practica2SG -m ./mapas/vertigo.map -n 2 -i 93 62 3 -t 93 62 0 -seed 0 -Tiempo 15000 -Ambiental 1000 -Energia 15000 -O 80 39
# 21
./practica2SG -m ./mapas/vertigo.map -n 2 -i 74 18 1 -t 74 18 0 -seed 0 -Tiempo 15000 -Ambiental 1000 -Energia 15000 -O 15 71
# 66

####### Test Nivel 3
./practica2SG -m ./mapas/mapa75_espirales.map -n 3 -i 47 7 0 -t 47 7 2 -seed 0 -Tiempo 5000 -Ambiental 1000 -Energia 10000 -O 26 18
# 92
./practica2SG -m ./mapas/mapa75_espirales.map -n 3 -i 36 24 0 -t 36 24 1 -seed 0 -Tiempo 5000 -Ambiental 1000 -Energia 10000 -O 22 4
# 72
./practica2SG -m ./mapas/mapa75_espirales.map -n 3 -i 6 66 0 -t 6 66 0 -seed 0 -Tiempo 5000 -Ambiental 1000 -Energia 10000 -O 25 59
# 123
./practica2SG -m ./mapas/mapa100.map -n 3 -i 88 3 0 -t 88 3 6 -seed 0 -Tiempo 5000 -Ambiental 1000 -Energia 10000 -O 50 16
# 92
./practica2SG -m ./mapas/mapa75_espirales.map -n 3 -i 35 3 0 -t 35 3 1 -seed 0 -Tiempo 5000 -Ambiental 1000 -Energia 10000 -O 46 60
# 107
./practica2SG -m ./mapas/mapa75_espirales.map -n 3 -i 39 37 0 -t 39 37 5 -seed 0 -Tiempo 5000 -Ambiental 1000 -Energia 10000 -O 7 59
# 108
./practica2SG -m ./mapas/mapa75_espirales.map -n 3 -i 35 50 0 -t 35 50 5 -seed 0 -Tiempo 5000 -Ambiental 1000 -Energia 10000 -O 45 63
# 34

####### Test Nivel 4
./practica2SG -m ./mapas/minivertiguito.map -n 4 -i 16 8 3 -t 26 9 0 -seed 0 -Tiempo 3000 -Ambiental 648 -Energia 3000 -O 19 11
# 10
./practica2SG -m ./mapas/minivertiguito.map -n 4 -i 16 8 3 -t 26 9 0 -seed 0 -Tiempo 3000 -Ambiental 1000 -Energia 3000 -O 17 22
# 12
./practica2SG -m ./mapas/minivertiguito.map -n 4 -i 16 8 3 -t 26 9 0 -seed 0 -Tiempo 3000 -Ambiental 1000 -Energia 3000 -O 3 3
# 22
./practica2SG -m ./mapas/mapa30_26.map -n 4 -i 13 26 5 -t 16 9 6 -seed 0 -Tiempo 3000 -Ambiental 2364 -Energia 3000 -O 26 3
# 16
./practica2SG -m ./mapas/gemini2.map -n 4 -i 10 21 1 -t 20 4 2 -seed 0 -Tiempo 3000 -Ambiental 1500 -Energia 3000 -O 26 3
# 6
./practica2SG -m ./mapas/gemini2.map -n 4 -i 10 21 1 -t 20 4 2 -seed 0 -Tiempo 3000 -Ambiental 1500 -Energia 3000 -O 22 13
# 10
./practica2SG -m ./mapas/gemini2.map -n 4 -i 10 21 1 -t 20 4 2 -seed 0 -Tiempo 3000 -Ambiental 3000 -Energia 3000 -O 22 13
# 12

####### Test Nivel 5
./practica2SG -m ./mapas/minivertiguito.map -n 5 -i 16 8 3 -t 26 9 0 -seed 0 -Tiempo 3000 -Ambiental 648 -Energia 3000 -O 19 11
# 150
./practica2SG -m ./mapas/minivertiguito.map -n 5 -i 16 8 3 -t 26 9 0 -seed 0 -Tiempo 3000 -Ambiental 1000 -Energia 3000 -O 17 22
# 210
./practica2SG -m ./mapas/minivertiguito.map -n 5 -i 16 8 3 -t 26 9 0 -seed 0 -Tiempo 3000 -Ambiental 1000 -Energia 3000 -O 3 3
# 370
./practica2SG -m ./mapas/mapa30_26.map -n 5 -i 13 26 5 -t 16 9 6 -seed 0 -Tiempo 3000 -Ambiental 2364 -Energia 3000 -O 26 3
# 260
./practica2SG -m ./mapas/vertigo.map -n 5 -i 80 75 2 -t 56 24 5 -seed 0 -Tiempo 3000 -Ambiental 2688 -Energia 9517 -O 4 36
# 800
./practica2SG -m ./mapas/mapa30.map -n 5 -i 25 20 6 -t 26 23 6 -seed 0 -Tiempo 3000 -Ambiental 1804 -Energia 6361 -O 14 17
# 100
./practica2SG -m ./mapas/mapa50_cuadricula.map -n 5 -i 25 38 6 -t 15 17 1 -seed 0 -Tiempo 3000 -Ambiental 1533 -Energia 4092 -O 30 24
# 300

####### Test Nivel 6
./practica2SG -m ./mapas/minivertiguito.map -n 6 -i 16 8 3 -t 26 9 0 -seed 0 -Tiempo 3000 -Ambiental 648 -Energia 3000 -O 19 11
# 600
./practica2SG -m ./mapas/minivertiguito.map -n 6 -i 16 8 3 -t 26 9 0 -seed 0 -Tiempo 3000 -Ambiental 1000 -Energia 3000 -O 17 22
# 840
./practica2SG -m ./mapas/minivertiguito.map -n 6 -i 16 8 3 -t 26 9 0 -seed 0 -Tiempo 3000 -Ambiental 1000 -Energia 3000 -O 3 3
# 1480
./practica2SG -m ./mapas/mapa30_26.map -n 6 -i 13 26 5 -t 16 9 6 -seed 0 -Tiempo 3000 -Ambiental 2364 -Energia 3000 -O 26 3
# 1040
./practica2SG -m ./mapas/vertigo.map -n 6 -i 80 75 2 -t 56 24 5 -seed 0 -Tiempo 3000 -Ambiental 2688 -Energia 9517 -O 4 36
# 3200
./practica2SG -m ./mapas/mapa30.map -n 6 -i 25 20 6 -t 26 23 6 -seed 0 -Tiempo 3000 -Ambiental 1804 -Energia 6361 -O 14 17
# 400
./practica2SG -m ./mapas/mapa50_cuadricula.map -n 6 -i 25 38 6 -t 15 17 1 -seed 0 -Tiempo 3000 -Ambiental 1533 -Energia 4092 -O 30 24
# 1200
fi
