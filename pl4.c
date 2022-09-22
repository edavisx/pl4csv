/*   Enrique Javier Davis Gutiérrez  -  Venezuela
          Licencia GPLv3 --> https://www.gnu.org/licenses/gpl-3.0.en.html
Referencia al autor y Respeta y Reproduce la Licencia y sus Libertades
*/

// Objetivo: 
//     |===-->  Convertir archivos PL4 a ASCII CSV

#include <stdio.h>
#include <stdlib.h>   //exit
#include <string.h>   //strncpy, strcat
#include <sys/stat.h> //struct stat
#include <stdint.h>   //intmax_t
#include <ctype.h>    //isdigit

#include "pl4_01.h"   // Variables Globales "extern"

FILE * fe_pl4;   //fichero PL4 de entrada
FILE * fo_csv;   //fichero CSV de salida

char typepl4 = 0;

////// Prototipos de Funciones
int edx_licencia(char * aa, char * bb);
int descriptores(char * aa, char * bb);
int size_file(char * aa, int bb);
int type_of_pl4(void);
int estructura_pl4(void);
int titulos_pl4(void);
int muestras_pl4(void);

int main(int argc, char **argv)
{
  edx_licencia("Enrique Javier Davis Gutiérrez", "0.3");
  if (argc > 2) 
  {
    fprintf(stderr, "% s: ERROR - más de un fichero de entrada\n ", argv[0]);
    exit(1);
  }
  else if (argc == 1)
  {
    fprintf(stderr, "ERROR - no hay fichero de entrada\nUso: %s <pathname>\n", argv[0]);
    exit(2);
  }
  else
  {
    if ((fe_pl4 = fopen(argv[1], "rb")) == NULL) 
    {
      fprintf(stderr, "%s: ERROR - no se puede abrir\n ", argv[1]);
      exit(3);
    } 
  }  
  char foo_csv[100]; // variable auxiliar para fo_csv; string de longitud definida
  strncpy(foo_csv,argv[1],strlen(argv[1])-3);
  strcat(foo_csv, "csv");
  fo_csv = fopen(foo_csv, "wt");
  type_of_pl4();
  estructura_pl4();
  titulos_pl4();
  muestras_pl4();
  fclose(fe_pl4);
  fclose(fo_csv);
  exit(0);
}
