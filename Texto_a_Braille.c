/*******************************************
 * Este programa toma un documento de texto
 * (.txt) como input y crea un nuevo doc.
 * con el texto convertido a los caracteres
 * Unicode correspondientes a la signografía
 * braille del español.
 * 
 * Creó: Oliver Daniel Almaraz Hernández
 * el 17-09-2020.
 * oliver.almaraz@gmail.com
 * 
 * Repositorio Github:
 * github.com/oliver-almaraz/Texto_a_Braille
 * 
 * 
 *      EXPLICACIÓN DEL SISTEMA BRAILLE
 * En el braille grado 1 (sin contracciones)
 * casi siempre un signo corresponde a una
 * letra del alfabeto español, pero hay excep-
 * ciones:
 *  - Las mayúsculas son los mismos signos que
 * las minúsculas pero precedidas del signo de
 * mayúscula (⠨).
 *  - Los números 1-0 son los mísmos signos que
 * las letras a-j, pero anteponiéndoles el
 * signo numeral (⠼). Si se pone un signo
 * numeral significa que todo lo que sigue será
 * un número hasta que haya un espacio, una le-
 * tra posterior a j, un signo de mayúscula o
 * el punto 5 (⠐).
 *  - Algunos signos de puntuación requieren
 * dos signos braille.
 * 
 * SI NO PUEDES VISUALIZAR LOS SIGUIENTES
 * SIGNOS DEBERÁS INSTALAR LAS FUENTES DE
 * TIPOGRAFÍA BRAILLE [⠁⠃⠉⠙⠑]
********************************************/

#include <stdio.h>
#include <string.h>
#include <wchar.h>
#include <locale.h>
#include <stdlib.h>

const char minus[26] = {
    'a','b','c','d','e','f','g','h','i','j',
    'k','l','m','n','o','p','q','r','s','t',
    'u','v','x','y','z','w'
};
const char *minusBrai[26] = {
    "⠁","⠃","⠉","⠙","⠑","⠋","⠛","⠓","⠊","⠚",
    "⠅","⠇","⠍","⠝","⠕","⠏","⠟","⠗","⠎","⠞",
    "⠥","⠧","⠭","⠽","⠵","⠺"
};
const int *diacriticos[14] = {
    // á , é , í , ó , ú , ü , ñ (161 - 188)
    161, 169, 173, 179, 186, 188, 177,
    // Á , É , Í , Ó , Ú , Ü , Ñ (129 - 156)
    129, 137, 141, 147, 154, 156, 145
};
const char *diacriticosBrai[14] = {
    // á , é , í , ó , ú , ü , ñ
    "⠷","⠮","⠌","⠬","⠾","⠳","⠻",
    // Á , É , Í , Ó , Ú , Ü , Ñ
    "⠷","⠮","⠌","⠬","⠾","⠳","⠻"
};
const char nums[10] = {
    '1', '2', '3', '4', '5', '6', '7', '8', '9', '0'
};
const char punct[26] = {
    '.', ',', ';', ':', '-',
    '?', '!', '"', '(', ')', '*',

    '$','%', '=', '+', '#',
    '|', '\'', '/', '\\', '{', '}',
    '[', ']', '@', '&'

};
const char *punctBrai[26] = {
    "⠄", "⠂", "⠆", "⠒", "⠤",
    "⠢", "⠖", "⠦", "⠣", "⠜", "⠔",

    "⠸⠜", "⠸⠴", "⠶", "⠖", "⠼⠐",
    "⠸", "⠄", "⠠⠂", "⠐⠄", "⠐⠸", "⠸⠂",
    "⠷", "⠾", "⠐", "⠠⠯"
};
const int punctEsp[2] = {
    // ¿ , ¡
    191, 161
};
const char *punctEspBrai[2] = {
    // ¿ , ¡
    "⠢", "⠖"
};

int main(int argc, char *argv[]) {
    
    setlocale(LC_ALL, "es_MX.UTF-8");
    
    char origen[200]; // Nombre del doc. de texto a convertir.
    if (argc == 2) {
        strncpy(origen, argv[1], 200);
    } else {
        printf("Arrastra a esta ventana el archivo de texto (.txt) a convertir o escribe su ruta:\n");
        fgets(origen, 200, stdin);
        origen[strcspn(origen, "\n")] = 0; // se elimina el \n al final
    }
    printf("\nAbriendo '%s'...\n", origen);
    
    FILE *source = fopen(origen, "r, ccs=UTF-8"); // Documento origen en modo de solo lectura (UTF-8)
    if (source == NULL) {
        printf("No se pudo abrir el archivo.\n¿Existe? ¿De casualidad está en uso por otra aplicación?\n");
        printf("(Presiona 'Enter' para salir)");
        fgets(NULL,10,stdin);
        exit(1);
    }
    
    char destino[50]; // Se crea el nombre del archivo output.
    strcpy(destino, origen);
    for (int i=0; i<4; i++) { // se le quita la extensión (.txt)
        destino[strlen(destino)-1] = '\0';
    }
    strcat(destino, "(braille).txt"); // y se le agrega texto para identificarlo + extención.
    printf("\nDocumento destino (en el mismo directorio que el documento origen):\n'%s'\n",destino);
    
    FILE *dest = fopen(destino, "w, ccs=UTF-8"); // Doc. destino en modo de escritura.
    if (dest == NULL) {
        printf("No se pudo crear el archivo destino.\n¿Tengo permiso para acceder al directorio?\n");
        printf("(Presiona 'Enter' para salir)");
        fgets(NULL,10,stdin);
        exit(1);
    }
    
    unsigned int letra;
    _Bool NUMERAL = 0; // Flags
    _Bool DIACRITICO = 0;
    _Bool PUNCT_ESP = 0;
    
    while ((letra = fgetc(source)) != EOF) {

        if (isalpha(letra)) { // Si es una letra (no diacríticos)
            if (NUMERAL && islower(letra) && letra < 107)
                fwprintf(dest, L"%s", "⠐"); // Si hay números antes se separan las letras (a-j) con el punto 5.

            if (islower(letra)) { // Minúscula
                NUMERAL = 0; // Las letras después de núms. desactivan el NUMERAL.
                for (int i=0; i<26; i++) {
                    if (letra == minus[i]) {
                        fwprintf(dest, L"%s", minusBrai[i]);
                    }
                }
            } else if (isupper(letra)) { // Mayúscula
                NUMERAL = 0;
                for (int i=0; i<26; i++) {
                    if (tolower(letra) == minus[i]) {
                        // Signo de mayúscula + letra
                        fwprintf(dest, L"%s%s", "⠨", minusBrai[i]);
                    }
                }
            }
        }
          else if (isspace(letra)) { // Si es espacio, Tab, \n
            NUMERAL = 0;
            if (letra == ' ') // El espacio también es especial
                fwprintf(dest, L"%s", "⠀");
            else
                fwprintf(dest, L"%c", letra);
        }
          else if (isdigit(letra)) { //Si es un número
            for (int i=0; i<10; i++) {
                if (letra == nums[i]) {
                    if ( ! NUMERAL) { // Marcador para no repetir inneces. el signo numeral.
                        NUMERAL = 1;
                        // Signo numeral + letra
                        fwprintf(dest, L"%s%s", "⠼", minusBrai[i]);
                    } else {
                        fwprintf(dest, L"%s", minusBrai[i]);
                    }
                }
            }
        } else if (ispunct(letra)) { // Signo de puntuación simple ( ? ! etc.)
            for (int i=0; i<26; i++) {
                if (letra == punct[i]) {
                    fwprintf(dest, L"%s", punctBrai[i]);
                }
            }
        } else if (letra == 194) { // FLAG para s. de puntuación especial ( ¿ ¡ )
            PUNCT_ESP = 1;
        } else if (PUNCT_ESP) {
            PUNCT_ESP = 0; //FLAG solo dura activa una vuelta
            for (int i=0; i<2; i++) {
                if (letra == punctEsp[i]) {
                    fwprintf(dest, L"%s", punctEspBrai[i]);
                }
            }
        } else if (letra == 195) { // Flag para letra con signo diacrítico
            DIACRITICO = 1;
            NUMERAL = 0;
        } else if (DIACRITICO) {
            DIACRITICO = 0; // Flag solo dura una vuelta
            if (letra > 160 && letra < 188) { // Si es letra con signo diacrítico minúscula
                for (int i=0; i<7; i++) {
                    if (letra == diacriticos[i]) {
                        fwprintf(dest, L"%s", diacriticosBrai[i]);
                    }
                }
            } else if (letra > 128 && letra < 161) { // Si es letra con sig. diac. mayus.
                for (int i=7; i<14; i++) {
                    if (letra == diacriticos[i]) {
                        fwprintf(dest, L"%s%s", "⠨", diacriticosBrai[i - 7]);
                    }
                }
            }
        }    
    }

    printf("\n¡Conversión terminada con éxito!\n");
    fclose(source);
    fclose(dest);
    
    printf("Esta es una aplicación gratuita y de código abierto,\n");
    printf("si quieres ayudar a mejorarla visita su repositorio:\n");
    printf("https://github.com/oliver-almaraz/Texto_a_Braille\n");
    printf("oliver.almaraz@gmail.com\n\n");
    printf("(Presiona 'Enter' para salir)");
    fgets(NULL,10,stdin);
    
    return 0;
}
