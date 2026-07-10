#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int main() {
    srand(time(NULL));
    // Simula uma leitura bruta de registradores de hardware (Valores entre 15 e 98)
    int leitura_bruta = (rand() % 84) + 15;
    printf("%d\n", leitura_bruta);
    return 0;
}
