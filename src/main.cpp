#include "Game.h"
#include "raylib.h"

int main() {
    // Asegurar que el directorio de trabajo es el de la aplicación
    // Esto ayuda a encontrar los assets si se lanza desde otra carpeta
    ChangeDirectory(GetApplicationDirectory());
    
    Game soapDynasty;
    soapDynasty.Run();
    return 0;
}
