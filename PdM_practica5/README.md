# Práctica 5

## Objetivo:
Implementar un módulo de software para utilizar la UART. 

## Punto 1
Implementar un módulo de software en un archivos fuente `API_uart.c` con su correspondiente archivo de cabecera `API_uart.h` y ubicarlos en el proyecto dentro de  las carpetas `/API/src` y `/API/inc`, respectivamente.

En `API_uart.h` se deben ubicar los prototipos de las funciones públicas.
```c
bool_t uartInit();
void uartSendString(uint8_t * pstring);
void uartSendStringSize(uint8_t * pstring, uint16_t size);
void uartReceiveStringSize(uint8_t * pstring, uint16_t size);
```
En `API_uart.c` se deben ubicar los prototipos de las funciones privadas y la implementación de todas las funciones de módulo, privadas y públicas.

### Consideraciones para la implementación:
`uartInit()` debe realizar toda la inicialización de la UART.  Adicionalmente, debe imprimir por la terminal serie un mensaje con sus parámetros de configuración.
La función devuelve:
- `True`: si la inicialización es exitosa.
- `False`: si la inicialización no es exitosa.

`uartSendString(uint8_t *pstring)` recibe un puntero a un string que se desea enviar por la UART completo (hasta el caracter `‘\0’`) y debe utilizar la función de la HAL `HAL_UART_Transmit(...)` para transmitir el string.

`uartSendStringSize(uint8_t * pstring, uint16_t size)` recibe un puntero a un string que se desea enviar por la UART y un entero con la cantidad de caracteres que debe enviar. La función debe utilizar `HAL_UART_Transmit(...)` para transmitir el string.

Las funciones del módulo deben verificar **TODOS** los parámetros que reciben: para los punteros, se verifica que sean distintos a `NULL` y los parámetros de cantidad size deben estar acotados a valores razonables (¿cuáles?).

Se deben verificar los valores de retorno de **TODAS** las funciones del módulo UART de la HAL que utilicen. 

## Punto 2
Sobre el ejercicio de la práctica 4, implementar un programa que utilice el módulo de uart implementado por ustedes para informar al usuario cuando se producen los flancos ascendentes y descendentes.

En cada flanco se debe enviar un mensaje por la UART que diga “Flanco ascendente/descendente detectado”, según corresponda.  Cada mensaje se debe escribir en una línea nueva en la terminal de la PC.

## Punto 3 (optativo)
Sobre el mismo proyecto desarrollado para el punto 1 y el punto 2, agregar la siguiente funcionalidad: *Implementar una función para borrar todo el contenido de la terminal*.

Cada vez que se envíe un caracter `‘c’` desde la terminal de la PC, primero se debe borrar todo el contenido de la terminal.  Luego, el microcontrolador debe responder con el envío de los parámetros de inicialización de la UART en un formato que sea legible claramente desde la terminal. 

Enviar al menos cuatro parámetros de configuración de la UART.

El sistema debe responder adecuadamente a la consulta, no debe perder peticiones ni generar reportes no solicitados, ¿Cuál es la mejor estratégia para lograr esto?