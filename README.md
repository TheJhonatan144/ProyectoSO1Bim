# ProyectoSO1Bim
Proyecto de Sistemas Operativos 1er Bimestre

# Simulación de Ventas de Libros: Procesos vs Hilos

**Proyecto de Sistemas Operativos**  
**Autores:** Jhona Torres (líder), Sabina Zabala, Sebastián Yánes, Geovanny Pereira
**Fecha:** 17 Junio 2025

---

## Descripción

Este proyecto compara dos enfoques de concurrencia en C mediante la **simulación de ventas de libros a gran escala**:

- **Procesos**: usando `fork()` y **pipes** para comunicación.
- **Hilos**: usando **pthread** y **mutex** para sincronización.

Se mide y analiza empíricamente el rendimiento de ambos métodos.

---

## Características

- Menú interactivo para elegir ejecución por procesos o hilos.
- Timestamps de inicio y fin (legibles con `ctime`).
- Medición de duración total con `clock_gettime`.
- Informe detallado de ventas por grupo (A–H) y resumen.
- Código modular y comentado.
- Uso de mecanismos de sincronización adecuados (pipes y mutex).

---

## Estructura del repositorio

\`\`\`text
RepositorioGit/
├── main.c                  # Menú y punto de entrada
├── procesos_libros.c       # Simulación con procesos
├── procesos_libros.h       # Cabecera para procesos
├── hilos_libros.c          # Simulación con hilos
├── hilos_libros.h          # Cabecera para hilos
├── Makefile                # Reglas de compilación
├── .gitignore              # Archivos ignorados por Git
└── README.md               # Documentación del proyecto
\`\`\`

---

## Requisitos

- GCC (o Clang) con soporte C11  
- Biblioteca **pthread**  
- Git para control de versiones  
- Entorno UNIX (Linux, macOS) o **Windows** con MinGW/WSL

---

## Instalación y compilación

Clona el repositorio:
\`\`\`bash
git clone https://github.com/tu_usuario/Proyecto-SO.git
cd Proyecto-SO
\`\`\`

Compila todo con:
\`\`\`bash
make
\`\`\`
Esto generará el ejecutable \`proyecto\`.

---

## Uso

Ejecuta:
\`\`\`bash
./proyecto
\`\`\`

Verás un menú:
\`\`\`
=== Simulación de Ventas de Libros ===
1) Ejecutar con PROCESOS (fork)
2) Ejecutar con HILOS (pthread)
0) Salir
\`\`\`

- Elige **1** para procesos.  
- Elige **2** para hilos.  
- Elige **0** para salir.

---

## Ejemplo de salida

\`\`\`
Inicio (procesos): Tue Jun 10 08:00:00 2025
Proceso 12345 - Grupo A: Venta 1 = $45
...
Fin (procesos): Tue Jun 10 08:01:30 2025
Duración total (procesos): 90.123 segundos

--- Resumen de Ventas ---
Grupo A: $54,312.00
...
\`\`\`

---

## Colaboración

1. Crear rama nueva:
\`\`\`bash
git checkout -b feature/mi-funcionalidad
\`\`\`
2. Hacer cambios y commitear:
\`\`\`bash
git add .
git commit -m "Descripción del cambio"
\`\`\`
3. Enviar rama al remoto:
\`\`\`bash
git push -u origin feature/mi-funcionalidad
\`\`\`
4. Abrir Pull Request en GitHub para revisión y merge.

---

## Licencia

Este proyecto está bajo licencia **MIT**.
