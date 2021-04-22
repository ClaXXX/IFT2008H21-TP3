# IFT2008H21-TP3
Troisième et dernier travail pratique pour le cours d'algorithme et structure de donnée. Le but du TP est de faire un programme qui suggère des synonymes pour les mots d'un texte donné en entrée.

## Requirements
les packets à installer avant de pouvoir faire tourner les tests sont gtest et gtest-devel.
Doxygen doit être préalablement installer avant de pouvoir générer la documentation.

## Build
### Setup
```shell
 mkdir build
 cd build
```

### Executable
```shell
 cmake ..
 make
```

### Test & Documentation
```shell
 cmake -DCMAKE_BUILD_TYPE=Debug ..
 make
```