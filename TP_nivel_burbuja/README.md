# Nivel de Superficies Digital

Este proyecto fue creado con el objetivo de funcionar como TP final para las materias "Protocolos de Comunicación en Sistemas Embebidos" y "Programación de Microcontroladores" de la **Carrera de Especialización en Sistemas Embebidos** de la UBA.

El proyecto consiste en un **Nivel de Superficies Digital** que simula el funcionamiento del clásico *nivel de burbuja* utilizado en carpintería, pero implementado digitalmente. Utiliza un acelerómetro MPU-6500 para medir la inclinación de una superficie en los ejes X e Y, representando el desplazamiento de la burbuja en una matriz de LEDs 8x8. 
Se modeló la aplicación como una máquina de estado finito cuyos estados se corresponden a los modos de funcionamiento: *medición simple del nivel de una superficie* o *comparador de superficies*. Este segundo modo permite almacenar una medición y compararla con otra en simultáneo. 

El sistema está basado en una placa STM32 NUCLEO-F446RE, y emplea los siguientes protocolos de comunicación para comunicarse con los módulos de entradas y salidas: I²C para el acelerómetro, SPI para la matriz de LEDs y UART para mostrar los valores de entrada y salida por el monitor serial.

## Estructura del Proyecto

La aplicación del nivel de superficies se encuentra desarrollada en el archivo `main.c`. La documentación del proyecto fue generada con *Doxygen*, el archivo `Doxyfile` y la documentación generada en `html` se encuentran ubicados en la carpeta `docs/` ubicada a nivel de proyecto.

Esta aplicación utiliza varios drivers específicos desarrollados para facilitar el uso de retardos no bloqueantes, el debounce del botón utilizado para cambiar de modo y el envío de datos mediante la UART. Estos drivers específicos fueron desarrollados como parte de las prácticas de la materia *Programación de Microcontroladores* y mejorados o adaptados para este proyecto, se encuentran ubicados en la carpeta `API/`.

Para la configuración y comunicación con los módulos externos seleccionados para el proyecto (el acelerómetro y el display de LEDs) se desarrollaron los *Device Drivers* correspondientes que se encuentran ubicados respectivamente en las carpetas `ledMatrix_MAX7219/` y `GY521_MPU6500/`, acompañados de su respectiva documentación.

```
TP_nivel_burbuja/                                                
├── README.md                          # README del proyecto (a nivel aplicación) 
├── docs/                              
|    ├── Doxyfile                      # Doxyfile del proyecto
|    └── html/                         # Documentación del proyecto (en HTML) generada con Doxygen
├── Core/                              
|    ├── Inc/                            
|    |    ├── main.h                             
|    |    └── …                          
|    ├── Src/                            
|    |    ├── main.c                   # Archivo principal de la aplicación         
|    |    └── …                        
└── Drivers/                            
     ├── API/                          # Drivers Específicos
     |    ├── README.md                # README de los drivers específicos         
     |    ├── Inc/                     
     |    |    ├── API_delay.h         # Proporciona funciones para retardos no bloqueantes                 
     |    |    ├── API_debounce.h      # Implementa una máquina de estados para el debounce de botón                   
     |    |    └── API_uart.h          # Interfaz para la configuración y uso de UART2               
     |    └── Src/                        
     |         ├── API_delay.c         # Implementación de las funciones de retardo no bloqueante                 
     |         ├── API_debounce.c      # Implementación de la máquina de estados de debounce                   
     |         └── API_uart.c          # Implementación de las funciones de UART2    
     |                  
     ├── ledMatrix_MAX7219/            # Device Driver para Matrix de LEDs de 8x8 con controlador MAX7219           
     |    ├── README.md                # README del driver       
     |    ├── docs/                    # Documentación sobre el driver
     |    ├── Inc/                       
     |    |    ├── ledMatrix.h         # Interfaz de alto nivel del driver                           
     |    |    └── ledMatrix_port.h    # Interfaz de bajo nivel (acceso al hardware)                     
     |    └── Src/                          
     |         ├── ledMatrix.c         # Implementación de funciones de alto nivel                
     |         └── ledMatrix_port.c    # Implementación de funciones específicas del hardware y comunicación por SPI   
     |                 
     └── GY521_MPU6500/                # Device Driver para el acelerómetro del GY-521 con controlador MPU-6500         
          ├── README.md                # README del driver       
          ├── docs/                    # Documentación sobre el driver     
          ├── Inc/                       
          |    ├── GY521.h             # Interfaz de alto nivel del driver                       
          |    └── GY521_port.h        # Interfaz de bajo nivel (acceso al hardware)                 
          └── Src/                          
               ├── GY521.c             # Implementación de funciones de alto nivel               
               └── GY521_port.c        # Implementación de funciones específicas del hardware y comunicación por I²C                 
```

## Características

- Matriz de LEDs 8x8: Se utiliza para mostrar la posición de la burbuja en un espacio bidimensional.
- Acelerómetro GY-521 (MPU-6050): Para detectar la inclinación de la superficie.
- Modos de operación:
  - Medición Simple: Muestra la posición dinámica de la burbuja.
  - Comparación de Niveles: Permite guardar la posición de la burbuja y compararla con una posición dinámica nueva.



- Interfaz UART: Los datos sobre el estado de la burbuja y las lecturas del acelerómetro se envían a través de UART para su visualización o procesamiento externo.



## Funcionamiento

### Inicialización
1. Al encender el sistema, se realiza una secuencia de inicialización que configura la matriz de LEDs, el acelerómetro y los periféricos necesarios.
2. Se muestra una imagen de bienvenida en la matriz de LEDs.

### Modo de Medición Simple
En este modo, el sistema lee constantemente los valores de aceleración del GY-521 y mapea estos valores a las coordenadas de la matriz de LEDs. El "dot" se mueve de acuerdo a la inclinación de la superficie, representando la burbuja en tiempo real.

### Modo de Comparación de Niveles
Este modo permite al usuario fijar una posición de la burbuja. Luego, puede comparar esta posición con las mediciones actuales, mostrando si la burbuja se encuentra en una posición más alta, más baja o en la misma.

### Comunicación UART
El sistema envía datos a través de UART, incluyendo:
- El estado de la burbuja.
- Las lecturas del acelerómetro.
- Las posiciones de la burbuja (movil y fija).

## Funciones Principales

- **bubbleInit()**: Inicializa el sistema y los periféricos.
- **bubbleUpdate()**: Actualiza el estado de la burbuja, incluyendo la lectura del acelerómetro y el procesamiento del botón.
- **drawMatrix()**: Dibuja la representación de la burbuja en la matriz de LEDs.
- **mapAccelToPosition()**: Mapea los valores del acelerómetro a las coordenadas de la matriz.
- **sendOutputsByUart()**: Envía los datos actuales por UART.



