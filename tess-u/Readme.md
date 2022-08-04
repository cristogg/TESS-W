TESS (Telescope Encoder & Sky Sensor)
tess-u se ejecuta en un microcontrolador AT49S4051. 
El programa se carga por ISP son el cargador de Atmel.
Lee la frecuencia generada por el sensor de brillo entre 0.01 Hz y 50KHz.
Por I2C lee una termopila MLX90614 y un acelerometro y magnetómetro LSM303.
Las lecturas se transmiten por puerto serie a 9600bps.
