# Device Driver para Matrix de LEDs de 8x8 con controlador MAX7219

Este *Device Driver* es un **Polled Driver** que permite configurar y controlar una matriz de LEDs de 8x8 que utiliza el controlador MAX7219. 

El propósito de este driver es facilitar el renderizado de un buffer de pantalla previamente generado, en la matriz de LEDs a través de la interfaz SPI, de forma similar a como se trabajaría cualquier otro display gráfico de mayor complejidad. 

El diseño sigue una *arquitectura modular por capas*,  donde la capa superior o de alto nivel funciona como abstracción del hardware, presentando una interfaz más amigable hacia el usuario.

Este driver tiene el siguiente alcance y limitaciones:

- Está diseñado para manejar una sola matriz y no puede manejar múltiples matrices en cascada sin antes extender la lógica de transmisión SPI.
- No admite operaciones de lectura, por lo que se compone únicamente de operaciones de escritura o comandos enviados a la matriz de LEDs para su control.
- No permite renderización parcial del display.

## Estructura del driver

```
ledMatrix_MAX7219/
 ├── Inc/
 │ ├── ledMatrix.h      # Interfaz de alto nivel del driver
 │ └── ledMatrix_port.h # Interfaz de bajo nivel (acceso al hardware)
 ├── Src/
 │ ├── ledMatrix.c      # Implementación de funciones de alto nivel
 │ └── ledMatrix_port.c # Implementación de funciones específicas del hardware y comunicación por SPI
 └── docs/              # Documentación sobre el driver
```

## Interfaz de alto nivel

Las funciones y estructuras disponibles para el usuario están definidas en `ledMatrix.h`.

Entre las funcionalidades presentadas mediante la interfaz al usuario se encuentran:
- Inicializar el controlador MAX7219 para trabajar con la matriz de LEDs.
- Setear la intensidad de los LEDs de forma simple con 3 niveles predefinidos.
- Activar y desactivar el modo SHUTDOWN de la matriz.
- Limpiar el display (apagar todos los LEDs).
- Renderizar un buffer de pantalla en el display de LEDs.

Este módulo utiliza una estructura `ledMatrix_t` que encapsula los datos necesarios para operar la matriz:
```c
typedef struct{
   GPIO_TypeDef * csPort;
   uint16_t csPin;
   SPI_HandleTypeDef * hSpi;
} ledMatrix_t;
```
Esta estructura permite definir con claridad qué SPI usar y qué pin se utilizará como **CS (chip select)** para habilitar el dispositivo.

Las funciones disponibles son las siguientes:

| Función | Descripción |
| ------- | ----------- |
| ledMatrixInit() | Inicializa la matriz configurando el MAX7219 |
| ledMatrixShutdown() | Habilita o deshabilita el modo SHUTDOWN |
| ledMatrixClear() | Limpia el display (apagando todos los LEDs) |
| ledMatrixRender() | Renderiza un buffer pantalla de 8 bytes sobre la matriz |
| ledMatrixSetIntensity() | Ajusta el brillo de los LEDs de la matriz |

**NOTA:** La función `ledMatrixRender()` espera un puntero a un arreglo de 8 bytes, donde cada byte representa una columna del display.

## Capa de acceso al Hardware
La capa inferior o de bajo nivel de este driver concentra la comunicación con el hardware mediante el protocolo SPI, utilizando las funciones que provee la HAL de STM32. Entre las funcionalidades de la capa de bajo nivel se encuentran:

- Setear un pin determinado en HIGH o LOW (utilizado para activar la comunicación mediante el pin designado como Chip Select.
- Enviar un comando o instrucción y su correspondiente valor por SPI. 
- Posibilidad de saber si el envío de la instrucción se realizó exitosamente o con errores, mediante el valor de retorno de la función.


## Ejemplo de uso básico

Ejemplo de inicialización y renderizado:
```c
ledMatrix_t myMatrix;
uint8_t displayBuffer[8] = {0x10,0x18,0x1C,0xFE,0xFE,0x1C,0x18,0x10}; // Flecha hacia arriba

ledMatrixInit(&myMatrix, &hspi2, GPIOB, GPIO_PIN_6);
ledMatrixSetIntensity(&myMatrix, INTENSITY_MEDIUM);
ledMatrixRender(&myMatrix, displayBuffer);
```

## Información general sobre la Matriz de LEDs de 8x8 con controlador MAX7219.

[Datasheet del controlador MAX7219](./docs/max7219-max7221.pdf)

Esquema de conexión del módulo:

![](./docs/max7219-dot-matrix-circuit-schematic.png)

El MAX7219 es un circuito integrado diseñado para el control de displays de LEDs, permitiendo gestionar hasta 64 LEDs mediante una interfaz serial compatible con SPI. Su arquitectura le permite operar con distintos tipos de arreglos, como matrices de LEDs de 8x8, displays de 7 segmentos de hasta 8 dígitos (originalmente diseñado para este fin) u otras configuraciones basadas en LEDs.

Este circuito integrado posee algunas características de utilidad como el control de la intensidad del display, un modo para testear el funcionamiento de los LEDs, un decodificador para BCD (Binary Coded Decimal), salidas compatibles para conexión en formato Daisy-Chain, y una pequeña memoria RAM interna con 8 registros de 8 bits donde guarda la información de cada dígito o grupo de 8 LEDs (filas o columnas) de la matriz.

Este integrado funciona como un multiplexor de filas o columnas de la matrix (según su conexión y orientanción). El esquematico de conexión es el siguiente:
[](./docs/max7219-dot-matrix-circuit-schematic.png)

### Registros e Instrucciones 

El MAX7219 tiene un conjunto de registros de dígitos y de control a los que se les puede enviar comandos a través de la interfaz SPI. Cada comando se envía con 16 bits:
- Los primeros 8 bits (D15-D8) representan la dirección del registro que se desea escribir.
- Los siguientes 8 bits (D7-D0) contienen el dato a escribir en ese registro.

| Nombre del Registro   | Dirección (Hexa)  | Descripción | 
| --------------------- | ----------------- | ----------- | 
| No-op                 | 0xX0              | Utilizado para manejar múltiples drivers en daisy-chain. El comando No-op (0xX0) seguido de cualquier valor (0xXX) permite no enviar ningún cambio al driver. | 
| Dígitos 0 a 7         | 0xX1 a 0xX8       | Registros en RAM para programar los dígitos. | 
| Modo de decodificación | 0xX9             | Habilita el modo de decodificación BCD para cada dígito. Cada bit de dato enviado a este registro corresponde a un dígito.  | 
| Intensidad del display | 0xXA             | Ajusta el brillo de los LEDs  mediante un PWM interno en 16 niveles (0xX0 a 0xXF). | 
| Límite de dígitos a escanear              | 0xXB | Setea el número de dígitos a mostrar de 1 a 8 (0xX0 a 0xX7). | 
| Shutdown              | 0xXC              | Controla el encendido/apagado del chip (0x00 = OFF, 0x01 = ON). Cuando el IC está en modo Shutdown ahorrará energía, pero aún puede ser programado. | 
| Test de Display       | 0xXF              | Operación en modo Test (todos los LEDs encendidos) o modo normal. | 



