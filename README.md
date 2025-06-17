# Simulación de Ventas de Libros: Procesos vs Hilos

Proyecto de Sistemas Operativos 2025A – EPN  
Comparativa entre fork() + semáforo y pthread + mutex
Integrantes: Sabina Zabala
             Geovanni Pereira
             Sebastian Yánez
             Jhonatan Torres 

## Objetivo
Explorar las diferencias de tiempo de ejecución entre:
- **Procesos mono-hilo** (fork + semáforo)
- **Proceso multi-hilo** (pthread + mutex)

Simulando VENTAS_TOTALES transacciones repartidas en NUM_GRUPOS (A–H).

## Estructura
```
config.h
procesos.h/.c   # fork() + semáforo
hilos.h/.c      # pthread + mutex
inicio.sh        # script de compilación y ejecución
Makefile        # compilación y limpieza
README.md       # documentación
```

## Uso

1. Dar permiso al script:
   ```bash
   chmod +x inicio.sh
   ```
2. Compilar manual:
   ```bash
   make
   ```
3. Ejecutar:
   ```bash
   ./procesos    # genera reporte_procesos.txt
   ./hilos       # genera reporte_hilos.txt
   ```
4. O usar el script:
   ```bash
   ./inicio.sh
   ```
5. Limpiar:
   ```bash
   make clean
   ```

## Reportes
Los archivos `reporte_procesos.txt` y `reporte_hilos.txt` muestran:
- Subtotal por grupo (A–H)
- Número de grupos
- Total vendido
- Total de transacciones
- Tiempo de ejecución

## Conclusiones
- Hilos más eficientes al manejar grandes volúmenes de transacciones por su menor overhead.
- Procesos ofrecen aislamiento y mayor seguridad de datos.
- Sincronización con semáforo vs mutex para evitar condiciones de carrera.
