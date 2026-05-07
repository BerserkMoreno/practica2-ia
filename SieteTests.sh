#!/bin/bash

# --- NIVEL 0 ---
./practica2SG -m ./mapas/mapa30.map -n 0 -i 17 5 0 -t 17 17 0 -seed 0 -Ambiental 1000 -Energia 3000 -Tiempo 3000
./practica2SG -m ./mapas/mapa30.map -n 0 -i 24 7 2 -t 17 17 0 -seed 0 -Ambiental 1000 -Energia 3000 -Tiempo 3000
./practica2SG -m ./mapas/mapa30.map -n 0 -i 24 10 2 -t 17 17 0 -seed 0 -Ambiental 1000 -Energia 3000 -Tiempo 3000
./practica2SG -m ./mapas/2ez.map -n 0 -i 19 26 3 -t 11 27 1 -seed 0 -Ambiental 1000 -Energia 3000 -Tiempo 3000
./practica2SG -m ./mapas/mapa30.map -n 0 -i 16 9 2 -t 16 14 6 -seed 0 -Ambiental 1000 -Energia 3000 -Tiempo 3000
./practica2SG -m ./mapas/mapa50.map -n 0 -i 38 10 2 -t 40 24 6 -seed 0 -Ambiental 1000 -Energia 3000 -Tiempo 3000
./practica2SG -m ./mapas/mapa75.map -n 0 -i 69 3 2 -t 40 24 6 -seed 0 -Ambiental 1000 -Energia 3000 -Tiempo 3000

# --- NIVEL 1 ---
./practica2SG -m ./mapas/minivertiguito.map -n 1 -i 7 4 0 -t 24 24 7 -seed 0 -Ambiental 1000 -Energia 3000 -Tiempo 3000
./practica2SG -m ./mapas/minivertiguito.map -n 1 -i 7 4 0 -t 24 24 7 -seed 0 -Ambiental 1000 -Energia 1000 -Tiempo 3000
./practica2SG -m ./mapas/gemini2.map -n 1 -i 20 8 0 -t 13 13 3 -seed 0 -Ambiental 1000 -Energia 3000 -Tiempo 3000
./practica2SG -m ./mapas/mapa30.map -n 1 -i 16 7 7 -t 17 16 4 -seed 0 -Ambiental 1000 -Energia 3000 -Tiempo 3000
./practica2SG -m ./mapas/mapa50.map -n 1 -i 38 10 2 -t 40 24 6 -seed 0 -Ambiental 1000 -Energia 3000 -Tiempo 3000
./practica2SG -m ./mapas/mapa50_cuadricula.map -n 1 -i 45 25 0 -t 45 26 0 -seed 0 -Ambiental 1000 -Energia 3000 -Tiempo 3000
./practica2SG -m ./mapas/bosque_prohibido.map -n 1 -i 4 11 0 -t 9 10 2 -seed 0 -Ambiental 1000 -Energia 3000 -Tiempo 3000

# --- NIVEL 2 ---
./practica2SG -m ./mapas/2ez.map -n 2 -i 23 19 2 -t 14 25 6 -seed 0 -Ambiental 2219 -Energia 8445 -Tiempo 3000 -O 14 16
# Valor objetivo (max_value_no_penalty): 24
./practica2SG -m ./mapas/mapaop.map -n 2 -i 69 62 5 -t 20 45 6 -seed 0 -Ambiental 674 -Energia 5941 -Tiempo 3000 -O 53 4
# Valor objetivo (max_value_no_penalty): 33
./practica2SG -m ./mapas/vertigo.map -n 2 -i 12 28 6 -t 87 45 3 -seed 0 -Ambiental 2752 -Energia 9316 -Tiempo 3000 -O 82 11
# Valor objetivo (max_value_no_penalty): 51
./practica2SG -m ./mapas/mapa75.map -n 2 -i 55 30 3 -t 12 53 2 -seed 0 -Ambiental 1919 -Energia 9247 -Tiempo 3000 -O 51 53
# Valor objetivo (max_value_no_penalty): 15
./practica2SG -m ./mapas/scape26.map -n 2 -i 20 15 0 -t 22 12 1 -seed 0 -Ambiental 2303 -Energia 7662 -Tiempo 3000 -O 22 21
# Valor objetivo (max_value_no_penalty): 7
./practica2SG -m ./mapas/mapa75_espirales.map -n 2 -i 24 68 1 -t 23 22 1 -seed 0 -Ambiental 2222 -Energia 9163 -Tiempo 3000 -O 24 66
# Valor objetivo (max_value_no_penalty): 4
./practica2SG -m ./mapas/mapa75.map -n 2 -i 35 35 6 -t 49 19 5 -seed 0 -Ambiental 60 -Energia 4920 -Tiempo 3000 -O 51 35
# Valor objetivo (max_value_no_penalty): 10

# --- NIVEL 3 ---
./practica2SG -m ./mapas/mapa30.map -n 3 -i 5 55 2 -t 10 10 4 -seed 0 -Ambiental 1000 -Energia 3000 -Tiempo 3000 -O 12 5
# Valor objetivo (max_value_no_penalty): 189
./practica2SG -m ./mapas/mapa75.map -n 3 -i 5 5 2 -t 8 5 2 -seed 0 -Ambiental 2362 -Energia 8505 -Tiempo 3000 -O 31 54
# Valor objetivo (max_value_no_penalty): 114
./practica2SG -m ./mapas/mapa75.map -n 3 -i 5 5 2 -t 8 5 2 -seed 0 -Ambiental 2362 -Energia 8505 -Tiempo 3000 -O 31 50
# Valor objetivo (max_value_no_penalty): 96
./practica2SG -m ./mapas/minivertiguito.map -n 3 -i 14 17 7 -t 5 12 3 -seed 0 -Ambiental 1747 -Energia 7118 -Tiempo 3000 -O 25 16
# Valor objetivo (max_value_no_penalty): 54
./practica2SG -m ./mapas/islas25.map -n 3 -i 11 7 3 -t 49 58 2 -seed 0 -Ambiental 2400 -Energia 5947 -Tiempo 3000 -O 55 68
# Valor objetivo (max_value_no_penalty): 27
./practica2SG -m ./mapas/paldea25.map -n 3 -i 88 18 3 -t 70 49 4 -seed 0 -Ambiental 861 -Energia 4664 -Tiempo 3000 -O 69 71
# Valor objetivo (max_value_no_penalty): 71
./practica2SG -m ./mapas/mapa30.map -n 3 -i 17 17 3 -t 19 19 5 -seed 0 -Ambiental 2461 -Energia 3813 -Tiempo 3000 -O 22 6
# Valor objetivo (max_value_no_penalty): 35

# --- NIVEL 4 ---
./practica2SG -m ./mapas/minivertiguito.map -n 4 -i 16 8 3 -t 26 9 0 -seed 0 -Ambiental 648 -Energia 3000 -Tiempo 3000 -O 19 11
# Valor objetivo (max_value_no_penalty): 10
./practica2SG -m ./mapas/minivertiguito.map -n 4 -i 16 8 3 -t 26 9 0 -seed 0 -Ambiental 1000 -Energia 3000 -Tiempo 3000 -O 17 22
# Valor objetivo (max_value_no_penalty): 12
./practica2SG -m ./mapas/minivertiguito.map -n 4 -i 16 8 3 -t 26 9 0 -seed 0 -Ambiental 1000 -Energia 3000 -Tiempo 3000 -O 3 3
# Valor objetivo (max_value_no_penalty): 22
./practica2SG -m ./mapas/mapa30_26.map -n 4 -i 13 26 5 -t 16 9 6 -seed 0 -Ambiental 2364 -Energia 3000 -Tiempo 3000 -O 26 3
# Valor objetivo (max_value_no_penalty): 16
./practica2SG -m ./mapas/gemini2.map -n 4 -i 10 21 1 -t 20 4 2 -seed 0 -Ambiental 1500 -Energia 3000 -Tiempo 3000 -O 26 3
# Valor objetivo (max_value_no_penalty): 6
./practica2SG -m ./mapas/gemini2.map -n 4 -i 10 21 1 -t 20 4 2 -seed 0 -Ambiental 1500 -Energia 3000 -Tiempo 3000 -O 22 13
# Valor objetivo (max_value_no_penalty): 10
./practica2SG -m ./mapas/gemini2.map -n 4 -i 10 21 1 -t 20 4 2 -seed 0 -Ambiental 3000 -Energia 3000 -Tiempo 3000 -O 22 13
# Valor objetivo (max_value_no_penalty): 10

# --- NIVEL 5 ---
./practica2SG -m ./mapas/minivertiguito.map -n 5 -i 16 8 3 -t 26 9 0 -seed 0 -Ambiental 648 -Energia 3000 -Tiempo 3000 -O 19 11
# Valor objetivo (max_value_no_penalty): 150
./practica2SG -m ./mapas/minivertiguito.map -n 5 -i 16 8 3 -t 26 9 0 -seed 0 -Ambiental 1000 -Energia 3000 -Tiempo 3000 -O 17 22
# Valor objetivo (max_value_no_penalty): 210
./practica2SG -m ./mapas/minivertiguito.map -n 5 -i 16 8 3 -t 26 9 0 -seed 0 -Ambiental 1000 -Energia 3000 -Tiempo 3000 -O 3 3
# Valor objetivo (max_value_no_penalty): 370
./practica2SG -m ./mapas/mapa30_26.map -n 5 -i 13 26 5 -t 16 9 6 -seed 0 -Ambiental 2364 -Energia 3000 -Tiempo 3000 -O 26 3
# Valor objetivo (max_value_no_penalty): 260
./practica2SG -m ./mapas/vertigo.map -n 5 -i 80 75 2 -t 56 24 5 -seed 0 -Ambiental 2688 -Energia 9517 -Tiempo 3000 -O 4 36
# Valor objetivo (max_value_no_penalty): 800
./practica2SG -m ./mapas/mapa30.map -n 5 -i 25 20 6 -t 26 23 6 -seed 0 -Ambiental 1804 -Energia 6361 -Tiempo 3000 -O 14 17
# Valor objetivo (max_value_no_penalty): 100
./practica2SG -m ./mapas/mapa50_cuadricula.map -n 5 -i 25 38 6 -t 15 17 1 -seed 0 -Ambiental 1533 -Energia 4092 -Tiempo 3000 -O 30 24
# Valor objetivo (max_value_no_penalty): 300

# --- NIVEL 6 ---
./practica2SG -m ./mapas/minivertiguito.map -n 6 -i 16 8 3 -t 26 9 0 -seed 0 -Ambiental 648 -Energia 3000 -Tiempo 3000 -O 19 11
# Valor objetivo (max_value_no_penalty): 600
./practica2SG -m ./mapas/minivertiguito.map -n 6 -i 16 8 3 -t 26 9 0 -seed 0 -Ambiental 1000 -Energia 3000 -Tiempo 3000 -O 17 22
# Valor objetivo (max_value_no_penalty): 840
./practica2SG -m ./mapas/minivertiguito.map -n 6 -i 16 8 3 -t 26 9 0 -seed 0 -Ambiental 1000 -Energia 3000 -Tiempo 3000 -O 3 3
# Valor objetivo (max_value_no_penalty): 1480
./practica2SG -m ./mapas/mapa30_26.map -n 6 -i 13 26 5 -t 16 9 6 -seed 0 -Ambiental 2364 -Energia 3000 -Tiempo 3000 -O 26 3
# Valor objetivo (max_value_no_penalty): 1040
./practica2SG -m ./mapas/vertigo.map -n 6 -i 80 75 2 -t 56 24 5 -seed 0 -Ambiental 2688 -Energia 9517 -Tiempo 3000 -O 4 36
# Valor objetivo (max_value_no_penalty): 2900
./practica2SG -m ./mapas/mapa30.map -n 6 -i 25 20 6 -t 26 23 6 -seed 0 -Ambiental 1804 -Energia 6361 -Tiempo 3000 -O 14 17
# Valor objetivo (max_value_no_penalty): 400
./practica2SG -m ./mapas/mapa50_cuadricula.map -n 6 -i 25 38 6 -t 15 17 1 -seed 0 -Ambiental 1533 -Energia 4092 -Tiempo 3000 -O 30 24
# Valor objetivo (max_value_no_penalty): 1200

