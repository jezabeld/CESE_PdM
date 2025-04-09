# TP "Nivel de burbuja" en STM32

Este proyecto fue creado con el objetivo de funcionar como TP final para las materias "Protocolos de Comunicación en Sistemas Embebidos" y "Programación de Microcontroladores".

A continuación se presentan algunas características de los dispositivos utilizados.

## Matriz de LEDs de 8x8 con controlador MAX7219.

[Datasheet](./docs/max7219-max7221.pdf)

El MAX7219 es un circuito integrado diseñado para el control de displays de LEDs, permitiendo gestionar hasta 64 LEDs mediante una interfaz serial compatible con SPI. Su arquitectura le permite operar con distintos tipos de arreglos, como matrices de LEDs de 8x8, displays de 7 segmentos de hasta 8 dígitos (originalmente diseñado para este fin) u otras configuraciones basadas en LEDs.

Este circuito integrado posee algunas características de utilidad como el control de la intensidad del display, un modo para testear el funcionamiento de los LEDs, un decodificador para BCD (Binary Coded Decimal), salidas compatibles para conexión en formato Daisy-Chain, y una pequeña memoria RAM interna con 8 registros de 8 bits donde guarda la información de cada dígito o grupo de 8 LEDs (filas o columnas) de la matriz.

Este integrado funciona como un multiplexor de filas o columnas de la matrix (según su conexión y orientanción). El esquematico de conexión es el siguiente:
[](./docs/max7219-dot-matrix-circuit-schematic.png)

### Registros e Instrucciones

El MAX7219 tiene un conjunto de registros de dígitos y de control a los que se les puede enviar comandos a través de la interfaz SPI. Cada comando se envía con 16 bits:
- Los primeros 8 bits (D15-D8) representan la dirección del registro que se desea escribir.
- Los siguientes 8 bits (D7-D0) contienen el dato a escribir en ese registro.
A continuación se detallan los registros con su dirección y una breve descripción de los mismos:

| Nombre del Registro   | Dirección (Hexa)  | Descripción | 
| --------------------- | ----------------- | ----------- | 
| No-op                 | 0xX0              | Utilizado para manejar múltiples drivers en daisy-chain. El comando No-op (0xX0) seguido de cualquier valor (0xXX) permite no enviar ningún cambio al driver. | 
| Dígitos 0 a 7         | 0xX1 a 0xX8       | Registros en RAM para programar los dígitos. | 
| Modo de decodificación | 0xX9             | Habilita el modo de decodificación BCD para cada dígito. Cada bit de dato enviado a este registro corresponde a un dígito.  | 
| Intensidad del display | 0xXA             | Ajusta el brillo de los LEDs  mediante un PWM interno en 16 niveles (0xX0 a 0xXF). | 
| Límite de dígitos a escanear              | 0xXB | Setea el número de dígitos a mostrar de 1 a 8 (0xX0 a 0xX7). | 
| Shutdown              | 0xXC              | Controla el encendido/apagado del chip (0x00 = OFF, 0x01 = ON). Cuando el IC está en modo Shutdown ahorrará energía, pero aún puede ser programado. | 
| Test de Display       | 0xXF              | Operación en modo Test (todos los LEDs encendidos) o modo normal. | 

El Device Driver desarrollado en este proyecto no admite operaciones de lectura sobre la matriz de leds, por lo que se compone únicamente de operaciones de escritura o `Comandos` enviados a la matriz para su control.

## Acelerometro y giroscopio de 6 ejes GY-521 con controlador MPU-6500.

[Datasheet](./docs/PS-MPU-6500A-01-v1.3.pdf)
[Register Map](./docs/MPU-6500-Register-Map2.pdf)

STANDARD POWER MODES 
The following table lists the user-accessible power modes for MPU-6500. 

| Mode  | Name                          | Gyro      | Accel         | DMP       |
| ----  | ----------------------------- | --------- | ------------- | --------- | 
| 1     | Sleep Mode                    | Off       | Off           | Off       | 
| 2     | Standby Mode                  | Drive On  | Off           | Off       |  
| 3*    | Low-Power Accelerometer Mode  | Off       | Duty-Cycled   | Off       | 
| 4     | Low-Noise Accelerometer Mode  | Off       | On            | Off       | 
| 5     | Gyroscope Mode                | On        | Off           | On or Off | 
| 6     | 6-Axis Mode                   | On        | On            | On or Off |  

(*) The MPU-6500 can be put into Accelerometer Only Low Power Mode using the following steps:  
- (i) Set CYCLE bit to 1 
- (ii) Set SLEEP bit to 0 
- (iii) Set TEMP_DIS bit to 1  
- (iv) Set DIS_XG, DIS_YG, DIS_ZG bits to 1

In  this  mode,  the  device  will  power  off  all  devices  except  for  the  primary  I2C  interface,  waking  only  
the  accelerometer at  fixed  intervals to take  a  single measurement.  The  frequency  of  wake-ups  can  
be configured with LP_WAKE_CTRL as shown below.  
| LP_WAKE_CTRL  | Wake-up Frequency | 
| ------------  | ----------------- | 
| 0             | 1.25 Hz           | 
| 1             | 5 Hz              | 
| 2             | 20 Hz             | 
| 3             | 40 Hz             |


### Registros e Instrucciones

| Nombre del Registro   | Dirección (Hexa)  | Descripción | 
| --------------------- | ----------------- | ----------- | 
| WHO_AM_I              | 0x75              | __ |
| CONFIG                | 0x1A              | __ |
| GYRO_CONFIG           | 0x1B              | __ |
| ACCEL_CONFIG          | 0x1C              | __ |
| PWR_MGMT_1            | 0x6B              | __ |
| PWR_MGMT_2            | 0x6C              | __ |
| ACCEL_XOUT_H          | 0x3B              | __ |
| ACCEL_XOUT_L          | 0x3C              | __ |
| ACCEL_YOUT_H          | 0x3D              | __ |
| ACCEL_YOUT_L          | 0x3E              | __ |
| ACCEL_ZOUT_H          | 0x3F              | __ |
| ACCEL_ZOUT_H          | 0x40              | __ |



*The  slave  address  of  the  MPU-6500  is  b110100X  which  is  7  bits  long. The  LSB  bit  of  the  7  bit  address  is determined by the logic level on pin AD0. The address of the divice should be b1101000 if pin AD0 is logic low.*

The contents of WHO_AM_I is an 8-bit device 
ID. The default value of the register is 0x70 for MPU-6500. This is different from the I2C address of 
the device as seen on the slave I2C controller by the applications processor. The I2C address of the 
MPU-6500 is 0x68 or 0x69 depending upon the value driven on AD0 pin.

SENSOR DATA REGISTERS  
The sensor data registers contain the latest gyro, accelerometer, auxiliary sensor, and temperature measurement 
data. They are read-only registers, and are accessed via the serial interface. Data from these registers may be read 
anytime. 

FIFO 
The MPU-6500 contains a 512-byte FIFO register that is accessible via the Serial Interface. The FIFO configuration 
register determines which data is written into the FIFO. Possible choices include gyro data, accelerometer data, 
temperature readings, auxiliary sensor readings, and FSYNC input. A FIFO counter keeps track of how many bytes of 
valid data are contained in the FIFO. The FIFO register supports burst reads. The interrupt function may be used to 
determine when new data is available.




Note: The (max) accelerometer output rate is 1kHz. This means that for a Sample Rate greater than 1kHz, 
the same accelerometer sample may be output to the FIFO, DMP, and sensor registers more than 
once. 