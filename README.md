# Soap Dynasty C++ Professional Edition

Este es el núcleo de tu juego en C++ utilizando el framework **Raylib**.

## Requisitos
1. **Compilador C++**: Recomendamos Mingw-w64 o Visual Studio 2022.
2. **Raylib**: Debes instalar la librería Raylib.
   - Si usas `vcpkg`: `vcpkg install raylib:x64-windows`
   - Si prefieres descargar el binario: [Raylib Releases](https://github.com/raysan5/raylib/releases)

## Cómo Compilar
### Usando CMake (Recomendado)
```bash
mkdir build
cd build
cmake ..
cmake --build .
```

### Usando g++ (Mingw) directamente
Si tienes `raylib` en el path:
```bash
g++ src/main.cpp -o SoapDynasty.exe -lraylib -lopengl32 -lgdi32 -lwinmm
```

## Estructura del Código
- `src/main.cpp`: Contiene la lógica principal, el bucle del juego y el sistema de renderizado isométrico.
- `assets/`: Todos tus gráficos organizados.

## Próximos Pasos
1. **Animaciones**: Implementar el recorte de spritesheets para los nuevos personajes de `Tiny RPG`.
2. **Mapa**: Usar un array 2D para definir diferentes tipos de terreno (agua, hierba, piedra).
3. **IA**: Implementar la lógica de "Pathfinding" para que los clientes caminen por los senderos.
