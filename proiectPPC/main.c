#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*--------------------------------------------------*/
/* Detectăm platforma și configurăm ecranul & culori */
/*--------------------------------------------------*/
#ifdef _WIN32
#   ifndef WIN32_LEAN_AND_MEAN
#   define WIN32_LEAN_AND_MEAN
#   endif
#   include <windows.h>
#   ifndef ENABLE_VIRTUAL_TERMINAL_PROCESSING
#   define ENABLE_VIRTUAL_TERMINAL_PROCESSING 0x0004
#   endif
#   define CLEAR_SCREEN "cls"
#else
#   define CLEAR_SCREEN "clear"
#endif

/*----------------------------*/
/*        ANSI CULORI         */
/*----------------------------*/
#define COLOR_RESET      "\033[0m"
#define COLOR_GREEN      "\033[0;32m"
#define COLOR_RED        "\033[0;31m"
#define COLOR_LIGHT_BLUE "\033[0;94m"   /* albastru deschis */

#define MAX_BICYCLES 100
#define MAX_STRING   50

typedef struct {
    char type[MAX_STRING];
    char location[MAX_STRING];
    int  available;          /* 1 = disponibila, 0 = imprumutata */
} Bicycle;

static Bicycle bicycles[MAX_BICYCLES];
static int     bicycleCount = 0;

/*----------------------------------------------------------*/
/*   UTILITAR: verifică dacă tipul introdus este valid      */
/*----------------------------------------------------------*/
static int isValidType(const char *type)
{
    const char *validTypes[] = {"oras", "cursiera", "munte", "campie"};
    size_t n = sizeof(validTypes) / sizeof(validTypes[0]);

    for (size_t i = 0; i < n; ++i)
        if (strcmp(type, validTypes[i]) == 0)
            return 1;                   /* tip permis */

    return 0;                           /* tip interzis */
}

/*------------------------------------*/
/*  Utilitar pentru afisarea titlului */
/*------------------------------------*/
static void printTitle(const char *title)
{
    printf(COLOR_LIGHT_BLUE "===== %s =====\n" COLOR_RESET, title);
}

/*------------------------------------*/
/*          Persistenta fisier        */
/*------------------------------------*/
static void loadBicyclesFromFile(void)
{
    FILE *file = fopen("bicycles.txt", "r");
    if (!file) return;                  /* dacă nu există fişier */

    bicycleCount = 0;
    while (fscanf(file, "%49s %49s %d",
                  bicycles[bicycleCount].type,
                  bicycles[bicycleCount].location,
                  &bicycles[bicycleCount].available) == 3)
    {
        if (++bicycleCount >= MAX_BICYCLES) break;
    }
    fclose(file);
}

static void saveBicyclesToFile(void)
{
    FILE *file = fopen("bicycles.txt", "w");
    if (!file) {
        puts("Eroare: nu pot scrie fisierul!");
        return;
    }
    for (int i = 0; i < bicycleCount; ++i)
        fprintf(file, "%s %s %d\n",
                bicycles[i].type,
                bicycles[i].location,
                bicycles[i].available);
    fclose(file);
}

/*------------------------------------*/
/*         Functionalitati CRUD       */
/*------------------------------------*/
static void pauseAndReturn(void)
{
    puts("\nApasati Enter pentru a reveni...");
    getchar(); getchar();
}

static void addBicycle(void)
{
    system(CLEAR_SCREEN);
    printTitle("Adaugare Bicicleta");

    if (bicycleCount >= MAX_BICYCLES) {
        puts("Lista de biciclete este plina!");
        pauseAndReturn();
        return;
    }

    Bicycle b;

    printf("Introduceti tipul bicicletei (oras / cursiera / munte / campie): ");
    scanf("%49s", b.type);

    if (!isValidType(b.type)) {
        puts("Tip invalid! Puteti adauga doar: oras, cursiera, munte, campie.");
        pauseAndReturn();
        return;
    }

    printf("Introduceti locatia bicicletei: ");
    scanf("%49s", b.location);
    b.available = 1;                     /* nou‑adaugata = disponibila */

    bicycles[bicycleCount++] = b;
    saveBicyclesToFile();
    puts("Bicicleta adaugata cu succes!");

    pauseAndReturn();
}

static void viewBicycles(void)
{
    system(CLEAR_SCREEN);
    printTitle("Lista Bicicletelor");

    if (bicycleCount == 0) {
        puts("Nu exista biciclete in sistem.");
    } else {
        for (int i = 0; i < bicycleCount; ++i) {
            const char *color  = bicycles[i].available ? COLOR_GREEN : COLOR_RED;
            const char *status = bicycles[i].available ? "disponibila" : "imprumutata";

            /* culoarea se aplică doar cuvântului status */
            printf("%d. %s - %s - %s%s%s\n",
                   i + 1,
                   bicycles[i].type,
                   bicycles[i].location,
                   color, status, COLOR_RESET);
        }
    }

    pauseAndReturn();
}

static void borrowBicycle(void)
{
    system(CLEAR_SCREEN);
    printTitle("Imprumutare Bicicleta");
    viewBicycles();
    if (bicycleCount == 0) return;

    int choice;
    printf("Alegeti numarul bicicletei pentru imprumut (0 pentru back): ");
    scanf("%d", &choice);

    if (choice == 0) return;

    if (choice > 0 && choice <= bicycleCount && bicycles[choice - 1].available) {
        bicycles[choice - 1].available = 0;
        saveBicyclesToFile();
        puts("Bicicleta imprumutata cu succes!");
    } else {
        puts("Selectie invalida sau bicicleta nu este disponibila.");
    }

    pauseAndReturn();
}

static void returnBicycle(void)
{
    system(CLEAR_SCREEN);
    printTitle("Returnare Bicicleta");
    viewBicycles();
    if (bicycleCount == 0) return;

    int choice;
    printf("Alegeti numarul bicicletei pentru returnare (0 pentru back): ");
    scanf("%d", &choice);

    if (choice == 0) return;

    if (choice > 0 && choice <= bicycleCount && !bicycles[choice - 1].available) {
        bicycles[choice - 1].available = 1;
        saveBicyclesToFile();
        puts("Bicicleta returnata cu succes!");
    } else {
        puts("Selectie invalida sau bicicleta nu a fost imprumutata.");
    }

    pauseAndReturn();
}

/*------------------------------------*/
/*               Meniu                */
/*------------------------------------*/
static void showMenu(void)
{
    int option;
    do {
        system(CLEAR_SCREEN);
        printf(COLOR_LIGHT_BLUE "======= Meniu Principal ========" COLOR_RESET "\n");
        puts("1. Vizualizare biciclete");
        puts("2. Adaugare bicicleta");
        puts("3. Imprumutare bicicleta");
        puts("4. Returnare bicicleta");
        puts("5. Iesire");
        printf("Alegeti o optiune: ");

        if (scanf("%d", &option) != 1) {
            int c; while ((c = getchar()) != '\n' && c != EOF) {}
            option = 0;                  /* forteaza default */
        }

        switch (option) {
            case 1: viewBicycles();  break;
            case 2: addBicycle();    break;
            case 3: borrowBicycle(); break;
            case 4: returnBicycle(); break;
            case 5: puts("Iesire..."); break;
            default: puts("Optiune invalida!"); pauseAndReturn();
        }
    } while (option != 5);
}

int main(void)
{
#ifdef _WIN32
    /* Activam secventele ANSI pe Windows 10+ */
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    if (hOut != INVALID_HANDLE_VALUE) {
        DWORD dwMode = 0;
        if (GetConsoleMode(hOut, &dwMode)) {
            SetConsoleMode(hOut, dwMode | ENABLE_VIRTUAL_TERMINAL_PROCESSING);
        }
    }
#endif

    loadBicyclesFromFile();
    showMenu();
    return 0;
}
