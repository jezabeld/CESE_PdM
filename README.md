# Programación de Microcontroladores - CESE - UBA

Este repositorio contiene las prácticas de la materia **Programación de Microcontroladores** del Curso de Especialización en Sistemas Embebidos.  
Los proyectos están desarrollados para el entorno **STM32CubeIDE**.  

## Requisitos para correr estos proyectos

- **STM32CubeIDE** (versión utilizada: `1.17.0`)
- **STM32CubeMX** (integrado en CubeIDE)
- Placa de desarrollo **STM32 NUCLEO-F446RE**

---

## Clonar el repositorio

Para clonar este repositorio localmente, ejecutá el siguiente comando en tu terminal:

```bash
git clone https://github.com/jezabeld/CESE_PdM.git

# Opcionalmente podes clonar el repo cambiandole el nombre a la carpeta con el nombre del usuario en caso de que los repos a revisar tengan nombres parecidos, por ejemplo:

git clone https://github.com/jezabeld/CESE_PdM.git jezabeld_CESE_PdM
```

Les recomiendo crear una carpeta donde clonar los repos de companeros para hacer review asi no se les mezclan los proyectos propios con los de los demás.

## Importar los proyectos en STM32CubeIDE
Desde STM32CubeIDE, `File > Open Projects From File System`.
En `Import Source` hacé click en `Directory` y seleccioná la carpeta del proyecto dentro de la carpeta donde clonaste el repo, por ejemplo: `~/reposParaReview/jezabeld_CESE_PdM/PdM_practica0` y click en `Open`.
Deberías ver una pantalla como la siguiente en la que se lista la carpeta del proyecto y en `Import as` dice `STM32CubeIDE project`: 

![](./img/importProyect.png)

Hacé click en `Finish` para importar el proyecto.
Buildeá y corré el proyecto como siempre.

