# Type-2-Clones
Proyecto Final Arquitectura Computadores II

# Para instalar en Ubuntu 16 libjsoncpp

sudo apt-get install libjsoncpp-dev

# Estructura del JSON:


[
   {
      "LN" : # de lineas(int),
      "IF" : # de condicionales(int),
      "CIC": # de ciclos(int),
      "RTN": # de retornos(int),
      "ARG": # de argumentos del metodo(int),
      "LOC": # de variables locales(int),
      "FUN": # de llamadas a funciones(int),
      "class": nombre de la clase(string),
      "name": nombre del metodo(string)		
   },
   {


   },
...

]


CODE_DETECTION.CPP:
g++ -o clone_Detection main.cpp clone_detection.cpp -std=c++11 -ljsoncpp

CUDA:
make
