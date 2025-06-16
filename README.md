# Computer_Graphics_Project

## Compilation

### MacOS

1. Installer les dépendances avec Homebrew :
```bash
brew install glew glfw
```

2. Compiler le projet :
```bash
mkdir build && cd build
cmake ..
make
```

3. Lancer l'application :
```bash
./3DViewer
```

### Windows

1. Installer MSYS2 depuis [https://www.msys2.org/](https://www.msys2.org/)

2. Ouvrir MSYS2 MINGW64 et installer les dépendances :
```bash
pacman -S mingw-w64-x86_64-glew mingw-w64-x86_64-glfw mingw-w64-x86_64-cmake mingw-w64-x86_64-gcc
```
> Note : `pacman` est le gestionnaire de paquets de MSYS2, similaire à `apt` sur Linux ou `brew` sur macOS. Il permet d'installer et de gérer les bibliothèques et outils nécessaires au développement.

3. Compiler le projet :
```bash
mkdir build && cd build
cmake -G "MinGW Makefiles" ..
mingw32-make
```

4. Lancer l'application :
```bash
./3DViewer
```

