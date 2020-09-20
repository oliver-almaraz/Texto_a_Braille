/*******************************************
 *  Este programa toma un documento de texto
 * (.txt) como input y crea un nuevo doc.
 * con el texto convertido a los caracteres
 * Unicode correspondientes a la signografía
 * braille del español.
 * 
 *      COMPILAR PARA WINDOWS
 *  Antes de compilar para Windows DESCOMENTA
 * las partes del código que se introducen con:
 *      //WIN//
 * Incluí el ícono ya compilado: "icono.o", pue-
 * des incluirlo al momento de enlazar (link).
 * (El '.exe' ya compilado está en los releases
 * del repositorio en Github.com).
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
 * SIGNOS CONFIGURA TU EDITOR O IDE CON
 * CODIFICACIÓN UTF-8: [⠁⠃⠉⠙⠑]
********************************************/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
//WIN// #include <windows.h>

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
const int diacriticos[14] = {
  // á ,  é ,  í ,  ó ,  ú ,  ü ,  ñ (161 - 188)
    161, 169, 173, 179, 186, 188, 177,
  // Á ,  É ,  Í ,  Ó ,  Ú ,  Ü ,  Ñ (129 - 156)
    129, 137, 141, 147, 154, 156, 145
};
const char *diacriticosBrai[14] = {
  // á ,  é , í , ó , ú , ü , ñ
    "⠷","⠮","⠌","⠬","⠾","⠳","⠻",
  // Á ,  É , Í , Ó , Ú , Ü , Ñ
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
  // ¿ ,  ¡
    191, 161
};
const char *punctEspBrai[2] = {
  // ¿ ,  ¡
    "⠢", "⠖"
};

int main(int argc, char *argv[]) {

    //WIN// SetConsoleOutputCP(65001); // Unreliable, works using mingw64-i686-gcc-core (x86, 7.4.0-1) though.

    printf("\n\"Texto a Braille\"\n");
    printf("Traductor de documentos de texto (.txt) a braille español (Unicode)\n\n");

    char origen[250]; // Nombre del doc. de texto a convertir.
    if (argc == 2) {
        strncpy(origen, argv[1], 250);
    } else {
        printf("Arrastra a esta ventana el archivo de texto (.txt) a convertir o escribe su ruta:\n");
        fgets(origen, 250, stdin);
        origen[strcspn(origen, "\n")] = 0; // se elimina el \n al final
    }
    printf("Abriendo '%s'...\n", origen);
    
    FILE *source = fopen(origen, "r"); // Documento origen en modo de solo lectura
    if (source == NULL) {
        printf("No se pudo abrir el archivo.\n¿Existe? ¿De casualidad está en uso por otra aplicación?\n");
        //WIN// system("pause");
        exit(1);
    }
    
    char destino[250]; // Se crea el nombre del archivo output.
    strcpy(destino, origen);
    for (int i=0; i<4; i++) { // se le quita la extensión (.txt)
        destino[strlen(destino)-1] = '\0';
    }
    strcat(destino, "(braille).txt"); // y se le agrega texto para identificarlo + extención.
    printf("\nDocumento destino (en el mismo directorio que el documento origen):\n'%s'\n",destino);
    
    FILE *dest = fopen(destino, "w"); // Doc. destino en modo de escritura.
    if (dest == NULL) {
        printf("No se pudo crear el documento destino.");
        printf("¿Hay un documento con el mismo nombre abierto por otra aplicación?");
        //WIN// system("pause");
        exit(1);
    }
    
    int letra; // Almacena el char que será leído por fgetc()
    _Bool NUMERAL = 0; // Flags
    _Bool DIACRITICO = 0;
    _Bool PUNCT_ESP = 0;
    
    while ((letra = fgetc(source)) != EOF) {

        if (isalpha(letra)) { // Si es una letra (no diacríticos)
            if (NUMERAL && islower(letra) && letra < 107)
                fprintf(dest, "%s", "⠐"); // Si hay números antes se separan las letras (a-j) con el punto 5.

            if (islower(letra)) { // Minúscula
                NUMERAL = 0; // Las letras después de núms. desactivan el NUMERAL.
                for (int i=0; i<26; i++) {
                    if (letra == minus[i]) {
                        fprintf(dest, "%s", minusBrai[i]);
                    }
                }
            } else if (isupper(letra)) { // Mayúscula
                NUMERAL = 0;
                for (int i=0; i<26; i++) {
                    if (tolower(letra) == minus[i]) {
                        // Signo de mayúscula + letra
                        fprintf(dest, "%s%s", "⠨", minusBrai[i]);
                    }
                }
            }
        }
          else if (isspace(letra)) { // Si es espacio, Tab, \n
            NUMERAL = 0;
            if (letra == ' ') // El espacio también es especial
                fprintf(dest, "%s", "⠀");
            else
                fprintf(dest, "%c", letra);
        }
          else if (isdigit(letra)) { //Si es un número
            for (int i=0; i<10; i++) {
                if (letra == nums[i]) {
                    if ( ! NUMERAL) { // Marcador para no repetir inneces. el signo numeral.
                        NUMERAL = 1;
                        // Signo numeral + letra
                        fprintf(dest, "%s%s", "⠼", minusBrai[i]);
                    } else {
                        fprintf(dest, "%s", minusBrai[i]);
                    }
                }
            }
        } else if (ispunct(letra)) { // Signo de puntuación simple ( ? ! etc.)
            for (int i=0; i<26; i++) {
                if (letra == punct[i]) {
                    fprintf(dest, "%s", punctBrai[i]);
                }
            }
        } else if (letra == 194) { // Para '¿' y '¡' fgetc() interpreta dos caracteres (int), el primero siempre es 194
            PUNCT_ESP = 1; // FLAG para s. de puntuación especial ( ¿ ¡ )
        } else if (PUNCT_ESP) {
            PUNCT_ESP = 0; //FLAG solo dura activa una vuelta
            for (int i=0; i<2; i++) {
                if (letra == punctEsp[i]) {
                    fprintf(dest, "%s", punctEspBrai[i]);
                }
            }
        } else if (letra == 195) { // Para [áÁ-úÚ,üÜ,ñÑ] fgetc() interpreta dos caracteres (int), el primero siempre es 195
            DIACRITICO = 1; // Flag para letra con signo diacrítico
            NUMERAL = 0;
        } else if (DIACRITICO) {
            DIACRITICO = 0; // Flag solo dura una vuelta
            if (letra > 160 && letra < 189) { // Si es letra con signo diacrítico minúscula
                for (int i=0; i<7; i++) {
                    if (letra == diacriticos[i]) {
                        fprintf(dest, "%s", diacriticosBrai[i]);
                    }
                }
            } else if (letra > 128 && letra < 157) { // Si es letra con sig. diac. mayus.
                for (int i=7; i<14; i++) {
                    if (letra == diacriticos[i]) {
                        fprintf(dest, "%s%s", "⠨", diacriticosBrai[i - 7]);
                    }
                }
            }
        }    
    }

    printf("\n¡Conversión terminada con éxito!\n\n");
    fclose(source);
    fclose(dest);
    
    printf("Esta es una aplicación gratuita y de código abierto,\n");
    printf("si quieres ayudar a mejorarla visita su repositorio:\n");
    printf("https://github.com/oliver-almaraz/Texto_a_Braille\n");
    printf("oliver.almaraz@gmail.com\n\n");
    
    //WIN// system("pause");

    return 0;
}
