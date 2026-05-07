#!/bin/bash

comandos=(
"./practica2SG -m ./mapas/minivertiguito.map -n 6 -i 16 8 3 -t 26 9 0 -seed 0 -Tiempo 3000 -Ambiental 648 -Energia 3000 -O 19 11"
"./practica2SG -m ./mapas/minivertiguito.map -n 6 -i 16 8 3 -t 26 9 0 -seed 0 -Tiempo 3000 -Ambiental 1000 -Energia 3000 -O 17 22"
"./practica2SG -m ./mapas/minivertiguito.map -n 6 -i 16 8 3 -t 26 9 0 -seed 0 -Tiempo 3000 -Ambiental 1000 -Energia 3000 -O 3 3"
"./practica2SG -m ./mapas/mapa30_26.map -n 6 -i 13 26 5 -t 16 9 6 -seed 0 -Tiempo 3000 -Ambiental 2364 -Energia 3000 -O 26 3"
"./practica2SG -m ./mapas/vertigo.map -n 6 -i 80 75 2 -t 56 24 5 -seed 0 -Tiempo 3000 -Ambiental 2688 -Energia 9517 -O 4 36"
"./practica2SG -m ./mapas/mapa30.map -n 6 -i 25 20 6 -t 26 23 6 -seed 0 -Tiempo 3000 -Ambiental 1804 -Energia 6361 -O 14 17"
"./practica2SG -m ./mapas/mapa50_cuadricula.map -n 6 -i 25 38 6 -t 15 17 1 -seed 0 -Tiempo 3000 -Ambiental 1533 -Energia 4092 -O 30 24"
"./practica2SG -m ./mapas/mapa75_espirales.map -n 6 -i 69 69 7 -t 3 69 5 -seed 0 -Tiempo 3000 -Ambiental 1417 -Energia 4150 -O 16 10"
"./practica2SG -m ./mapas/mapa50_cuadricula.map -n 6 -i 25 38 6 -t 15 17 1 -seed 0 -Tiempo 3000 -Ambiental 3533 -Energia 4092 -O 46 18"
"./practica2SG -m ./mapas/mapa75.map -n 6 -i 29 68 6 -t 14 52 2 -seed 0 -Tiempo 3000 -Ambiental 865 -Energia 5574 -O 52 32"
"./practica2SG -m ./mapas/mapa100.map -n 6 -i 13 33 3 -t 82 73 6 -seed 0 -Tiempo 3000 -Ambiental 1719 -Energia 4581 -O 25 61"
"./practica2SG -m ./mapas/mapa75.map -n 6 -i 29 68 6 -t 14 52 2 -seed 0 -Tiempo 3000 -Ambiental 1500 -Energia 5574 -O 48 16"
"./practica2SG -m ./mapas/mapaop.map -n 6 -i 42 36 1 -t 42 37 1 -seed 0 -Tiempo 3000 -Ambiental 2280 -Energia 3403 -O 26 22"
"./practica2SG -m ./mapas/islas_cambio_climatico.map -n 6 -i 27 94 7 -t 52 92 4 -seed 0 -Tiempo 3000 -Ambiental 2107 -Energia 4383 -O 11 10"
"./practica2SG -m ./mapas/mapa50.map -n 6 -i 17 27 6 -t 41 23 1 -seed 0 -Tiempo 3000 -Ambiental 2836 -Energia 3699 -O 32 16"
"./practica2SG -m ./mapas/gemini2.map -n 6 -i 10 21 1 -t 20 4 2 -seed 0 -Tiempo 3000 -Ambiental 1500 -Energia 3000 -O 26 3"
"./practica2SG -m ./mapas/luminalia25.map -n 6 -i 83 62 0 -t 39 75 0 -seed 0 -Tiempo 3000 -Ambiental 1500 -Energia 7552 -O 63 61"
)

limites=(600 840 1480 1040 2900 400 1200 600 1800 1080 2280 2240 900 2400 1280 440 2880)

total=${#comandos[@]}
pasados=0
rapidos=0

for i in "${!comandos[@]}"; do
    echo "Test $((i+1)):"
    
    salida=$(eval ${comandos[$i]} 2>&1)

    if echo "$salida" | grep -q "Nivel 6 completado con Exito!"; then
        ((pasados++))
        
        instantes=$(echo "$salida" | grep "Instantes consumidos" | awk -F': ' '{print $2}')
        limite=${limites[$i]}

        if [ "$instantes" -lt "$limite" ]; then
            echo "🚀 RÁPIDO - $instantes < $limite"
            ((rapidos++))
        else
            echo "⚠️ PASADO pero lento - $instantes >= $limite"
        fi
    else
        echo "❌ NO PASADO"
    fi

    echo "------------------------"
done

echo "====== RESUMEN ======"
echo "Tests pasados: $pasados/$total"
echo "Tests rápidos: $rapidos/$total"