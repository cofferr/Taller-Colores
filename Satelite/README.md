# Análisis de Imagen Satelital con OpenCV

## Datos de la imagen

| Parámetro    | Valor           |
|--------------|-----------------|
| Archivo      | image_sat.jpg   |
| Filas        | 666             |
| Columnas     | 1024            |
| Canales      | 1 (escala de grises) |
| Profundidad  | CV_8U (8 bits)  |
| Total píxeles | 681,984        |

---

## 1. Representación Digital e Histograma

**Resultados obtenidos:**
- Valor mínimo de píxel: `0`
- Valor máximo de píxel: `232`
- Rango dinámico: `232`
- Media: `89.23`
- Desviación estándar: `37.02`

### ¿La imagen tiene alto contraste?

**Sí.** Con un rango dinámico de 232 de 255 posibles, la imagen prácticamente usa todo el espectro de intensidades. El resultado es un histograma bien disperso, con píxeles repartidos en un amplio abanico de valores: eso es lo que ves en imágenes de alto contraste.

### ¿Está concentrada en tonos oscuros?

**No exactamente.** Con una media de `89.23`, los píxeles tienden hacia los tonos medios-bajos (neutro sería 128). La distribución es bastante mixta: hay más oscuridad de la que esperarías en una foto normal, pero también hay presencia importante de medios tonos y algunos píxeles claros repartidos.

### ¿Qué podría representar eso?

En una imagen satelital con esta composición de tonos:

- **Tonos oscuros (0–60):** son **cuerpos de agua**: absorben mucha luz y apenas reflejan.
- **Tonos medios (60–150):** **vegetación**, **tierra** o **zonas agrícolas** con reflectancia normal.
- **Tonos claros (150–232):** probablemente **zonas urbanas**, **suelo muy árido** o **nubes**: estos elementos reflejan bastante energía.

---

## 2. Modelado Gaussiano de Píxeles (Segmentación)

**Resultados obtenidos:**

| Umbral   | Píxeles clasificados como fondo | Porcentaje |
|----------|---------------------------------|------------|
| 1σ       | 436,879                         | 64.06%     |
| **2σ**   | **655,272**                     | **96.08%** |
| 3σ       | 681,645                         | 99.95%     |

Rango de clasificación con 2σ: `[89.23 - 74.05, 89.23 + 74.05]` = `[15.18, 163.28]`

### ¿Qué regiones quedan como fondo (con 2σ)?

Con 2σ clasificamos como fondo todo lo que esté entre ~15 y ~163, lo que cubre un 96% de la imagen. Eso incluye prácticamente todos los elementos típicos: agua, vegetación, suelos. Solo excluimos lo muy oscuro (< 15, sombras profundas) y lo muy brillante (> 163, zonas urbanas super reflectantes o nubes).

### ¿Qué pasa si usamos 1σ?

Con 1σ solo el **64.06%** de la imagen se considera fondo. Es una segmentación muy exigente: solo los píxeles entre ~52 y ~126 pasan el corte. Útil si lo que quieres es aislar solo lo más homogéneo (como un lago bien definido), pero pierdes mucha información legítima del fondo.

### ¿Y si usamos 3σ?

Con 3σ el **99.95%** queda como fondo. Es tan permisivo que casi nada se considera "objeto de interés". El umbral cubre desde ~0 hasta ~200, abarcando casi toda la escala de grises. Solo excluyes los píxeles más extremadamente brillantes (> 200): pura reflexión especular o anomalías muy raras.

---

## 3. Filtros de Suavizado (Lowpass)

**Resultados obtenidos:**

| Métrica              | Original | Suavizada (Gaussiano 7×7, σ=1.5) |
|----------------------|----------|-----------------------------------|
| Desviación estándar  | 37.024   | 34.145                            |
| Varianza             | 1370.78  | 1165.87                           |
| Reducción de varianza| —        | **14.95%**                        |

### ¿Disminuyó la varianza?

**Sí.** Bajó un 14.95%, de 1370.78 a 1165.87. Tiene lógica: el filtro Gaussiano promedia cada píxel con sus vecinos, así que las diferencias locales se suavizan y los valores se vuelven menos dispersos.

### ¿Qué significa físicamente?

Una varianza menor quiere decir que los píxeles vecinos son más parecidos entre sí. Los bordes afilados y los detalles pequeños desaparecen. En una imagen satelital, es como "desenfocar" un poco los límites entre regiones (agua y costa, por ejemplo): ganas claridad pero pierdes un poco de nitidez espacial.

### ¿Se redujo el ruido speckle (si es radar)?

**Sí, en parte.** El speckle es ruido de alta frecuencia, así que el Gaussiano lo atenúa al suavizar variaciones locales. Pero honestamente, el Gaussiano no es la opción ideal para speckle (en imágenes SAR prefieres Lee o Frost): porque además de ruido, también borra detalles reales junto con él.

---

## 4. Filtros de Afilado (Highpass)

**Resultados obtenidos:**

| Imagen                         | Desviación estándar |
|--------------------------------|---------------------|
| Original                       | 37.024              |
| Laplaciano (bordes detectados) | 66.078              |
| Imagen afilada (original + Lap)| 72.776              |
| Afilado con suavizado previo   | **43.528**          |

### ¿Se resaltan bordes?

**Sí.** El Laplaciano es un detector de segunda derivada: responde a cambios bruscos de intensidad. Mira los números: la desviación estándar sube de 37.02 a 66.08 (casi el doble). Eso confirma que está amplificando las transiciones: bordes entre agua y tierra, vegetación y ciudad, etc.

### ¿Se amplifica el ruido?

**Sí.** La imagen afilada directamente sube a 72.78 de desviación, más que nada anterior. El problema es que el Laplaciano no diferencia entre bordes reales y ruido: ambos son cambios bruscos, así que amplifica todo por igual. En una imagen satelital con speckle, terminas con un afilado ruidoso y desagradable de ver.

### ¿Conviene suavizar antes de afilar?

**Totalmente.** Si suavizas primero, el afilado final te da 43.53 de desviación, frente a 72.78 sin preprocesamiento. La diferencia de ~29 puntos es enorme. Por qué funciona: el Gaussiano limpia el ruido de alta frecuencia antes, así el Laplaciano solo amplifica los bordes reales. Resultado: bordes nítidos sin ruido amplificado.

---

## 5. Comparación Final — Pipeline Completo

**Pipeline aplicado:** `Original → GaussianBlur(7×7, σ=1.5) → Laplaciano → Suma`

**Resultados obtenidos:**

| Etapa                     | Desviación estándar |
|---------------------------|---------------------|
| Imagen original            | 37.024              |
| Tras suavizado Gaussiano   | 34.145              |
| Pipeline final (realzado)  | 37.260              |

### Interpretación del pipeline

El pipeline logra un **balance muy bueno**: la desviación final (37.26) es casi idéntica a la original (37.02). Eso significa que obtienes una imagen con bordes más definidos y ruido controlado, pero sin perder el nivel de contraste general.

**Cómo funciona cada paso:**
- **Etapa 1 (Gaussiano):** baja la varianza ~15%, eliminando ruido de alta frecuencia.
- **Etapa 2 (Laplaciano):** detecta bordes reales sin contaminación de ruido (porque ya fue atenuado).
- **Etapa 3 (Suma):** combina todo de nuevo para dar nitidez sin disparar la desviación.

Comparado con afilar directamente (72.78), este pipeline es mucho mejor: obtienes bordes claros, ruido bajo y estadísticas estables. Eso facilita cosas posteriores: clasificar tipos de terreno, detectar cambios, segmentar regiones, etc.

---

## Compilación y uso

```bash
# Compilar
g++ main.cpp -o hist `pkg-config --cflags --libs opencv4`

# Ejecutar con cualquier imagen
./hist ruta/a/tu/imagen.jpg
./hist ~/Downloads/image_sat.jpg
```

