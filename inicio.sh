# start.sh — Compila y ejecuta ambas versiones, luego muestra reportes

# 1. Compila los programas de procesos e hilos.
# 2. Ejecuta las simulaciones.
# 3. Abre los reportes generados en terminales separadas.
# Se ejecut de forma secuencial y autimática para facilitar la comparación.
set -e

echo "Compilando procesos..."
gcc -std=c11 -Wall -pthread procesos.c -o procesos

echo "Compilando hilos..."
gcc -std=c11 -Wall -pthread hilos.c   -o hilos

echo "Ejecutando simulaciones..."
./procesos
./hilos

echo "Abriendo reportes..."
gnome-terminal -- bash -c "less reporte_procesos.txt; exec bash" &
gnome-terminal -- bash -c "less reporte_hilos.txt; exec bash" &

wait
