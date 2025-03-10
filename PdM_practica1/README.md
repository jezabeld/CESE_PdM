# Práctica 1

Link al archivo [main.c](./Core/Src/main.c) del proyecto.

## Objetivo:
Familiarizarse con el entorno de trabajo STM32CubeIDE + NUCLEO-F446RE + firmware. 

## Punto 1:
Implementar un programa que haga parpadear el led de la placa NUCLEO-F446RE LD2.

El led debe permanecer encendido 200 ms con duty cycle 50%.

## Punto 2:
Utilizar el pulsador (B1) para controlar el tiempo de encendido.  Cada vez que se presiona el pulsador el tiempo de encendido debe alternar entre 200 ms y 500 ms.

### Algunas preguntas para pensar el ejercicio:

¿De qué serviría usar un array de “tiempos” en el programa? ¿Qué pasa con nuestro programa si nos piden agregar/sacar/cambiar un tiempo de encendido? 
```
Un array de tiempos podría servir para tener una lista de los valores ordenados por los que ir saltando a medida que el usuario presiona el botón. Un array es fácil de recorrer en forma circular simplemente llevando cuenta de cuantos valores contiene y en qué índice se encuentra posicionado el valor actual.
```
¿Cómo responde el programa a las pulsaciones, hay falsos positivos o pulsaciones no detectadas? 
```
Si el botón no se configura con una interrupción, siempre se observarán pulsaciones no detectadas en cualquier momento del programa en el que el mismo no esté "leyendo" el valor del botón. 
Por otro lado, los botones al ser dispositivos mecánicos son susceptibles a un efecto de rebote que puede generar que se detecten múltiples pulsaciones o cambios de valor del pin con una misma pulsación. En general esto se suele mitigar usando algun timer o delay pequeño inmediatamente después de obtener el estado del botón.
```
¿Cuál es el mejor momento para leer el pulsador, luego de un ciclo completo de la secuencia o después de encender y apagar el led? ¿Qué diferencia hay entre estas alternativas?
```
Ningún momento es en sí mejor que el otro para leer el estado del botón, ya que en ambos casos se dará un tiempo (de aproximadamente la duración del delay de intermitencia del led) en el que el cambio de estado del botón no será detectado.
```
¿Cambiaría las respuestas a las preguntas anteriores si el tiempo de encendido del led fuera sensiblemente más grande, 2 segundos por ejemplo? ¿Y si fuera  sensiblemente más chico, 50 ms por ejemplo?
```
Si el delay fuera más grande, el método de Polling del botón dejaría más en evidencia el intervalo de "no detección" de la pulsación, y si fuera más corto, se podrían detectar muchos más falsos negativos debido al rebote del botón.
```

