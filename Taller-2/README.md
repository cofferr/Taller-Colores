# Taller 2 - Procesamiento de Imágenes con OpenCV

Ejercicios resueltos usando el código base proporcionado por el profesor

## Requisitos

- OpenCV
- CMake 3.10 o superior
- Compilador de C++
## Descripción de los Ejercicios

### Ejercicio 1: Implementar Conversión RGB → HSV

**Archivo:** `ex1.cpp`

**Descripción:** Implementa la conversión manual de imágenes del espacio de color BGR a HSV utilizando las fórmulas estándar de conversión.

**Parámetros:**
- `ruta_imagen`: Ruta a la imagen a procesar (en cualquier formato soportado por OpenCV)

**Ejecución:**
```bash
./ex1 imagen.jpg
```

---

### Ejercicio 2: Modificar Saturación Manualmente

**Archivo:** `ex2.cpp`

**Descripción:** Realiza una conversión BGR→HSV, aumenta la saturación de los colores en un 50% (multiplicando el valor S por 1.5) y convierte de vuelta a BGR.

**Parámetros:**
- `ruta_imagen`: Ruta a la imagen a procesar

**Ejecución:**
```bash
./ex2 imagen.jpg
```

---

### Ejercicio 3: Implementar K-Means Completo

**Archivo:** `ex3.cpp`

**Descripción:** Implementa el algoritmo K-Means desde cero para cuantizar los colores de una imagen. Reduce la paleta de colores a K colores representativos mediante clustering. La imagen se redimensiona a 160x120 para acelerar el procesamiento.

**Parámetros:**
- `ruta_imagen`: Ruta a la imagen a procesar
- `K`: Número de colores finales en la paleta (por defecto: 5)

**Ejecución:**
```bash
./ex3 imagen.jpg 5
```

**Salida:** Muestra tres ventanas:
- Original: Imagen redimensionada (160x120)
- K-Means Manual K=5: Imagen cuantizada con los K colores
- Paleta: Visualización de los K colores centroides

**Ejemplo con diferentes valores K:**
```bash
./ex3 imagen.jpg 3   # Para paleta de 3 colores
./ex3 imagen.jpg 10  # Para paleta de 10 colores
./ex3 imagen.jpg 16  # Para paleta de 16 colores
```

---

### Ejercicio 4: Implementar Gray World

**Archivo:** `ex4.cpp`

**Descripción:** Implementa el algoritmo Gray World para corregir automáticamente el balance de blancos en imágenes. Calcula factores de escala independientes para cada canal (B, G, R) para llevar el promedio de cada canal al mismo nivel.

**Parámetros:**
- `ruta_imagen`: Ruta a la imagen a procesar

**Ejecución:**
```bash
./ex4 imagen.jpg
```

---

### Ejercicio 5: Implementar Corrección Gamma

**Archivo:** `ex5.cpp`

**Descripción:** Aplica una corrección gamma a la imagen para ajustar el brillo y contraste. Utiliza una tabla de búsqueda (LUT) para optimizar el rendimiento. Gamma > 1 oscurece la imagen, mientras que gamma < 1 la aclara.

**Parámetros:**
- `ruta_imagen`: Ruta a la imagen a procesar

**Ejecución:**
```bash
./ex5 imagen.jpg
```

---

### Ejercicio 6: Implementar Corrección de Viñeteo

**Archivo:** `ex6.cpp`

**Descripción:** Corrige el efecto de viñeteo que es común en fotografías digitales. Calcula la distancia desde el centro a cada píxel y aplica un factor de corrección basado en esa distancia.

**Parámetros:**
- `ruta_imagen`: Ruta a la imagen a procesar

**Ejecución:**
```bash
./ex6 imagen.jpg
```

---

## Compilación

```bash
mkdir -p build
cd build
cmake ..
make
```

## Estructura del Proyecto

```
.
├── CMakeLists.txt      # Configuración de CMake
├── ex1.cpp             # Conversión BGR→HSV
├── ex2.cpp             # Modificación de saturación
├── ex3.cpp             # K-Means manual
├── ex4.cpp             # Gray World
├── ex5.cpp             # Gamma correction
├── ex6.cpp             # Vignette correction
├── build/              # Directorio de compilación
└── README.md          # Este archivo
```