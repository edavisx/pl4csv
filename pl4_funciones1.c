#include <stdio.h>
#include <stdlib.h>   //exit
#include <string.h>   //strncpy, strcat
#include <sys/stat.h> //struct stat
#include <stdint.h>   //intmax_t
#include <ctype.h>    //isdigit

#include "pl4_01.h"   // Variables Globales "extern"

struct estructura_pl4
{
  unsigned long int size;
  unsigned short int variables_sin_time;
  unsigned char fecha[21];
  unsigned char byte_0x00;
  unsigned int byte1_bloque_datos;
  unsigned int byte1_bloque_titulos;
  unsigned int cantidad_muestras;
  long int posicion_ultimo_time; 
  float valor_ultimo_time;
};

struct estructura_pl4 PL4;

//// 111: cálculo del tamaño de un fichero
int size_file(char * aa, int metodo)  
{
  struct stat f_datos1;
  long int x;
  long int sizeFile = 0;
  int c;
  
  switch (metodo)
  {
    case 1:
      fseek(fe_pl4, 0, SEEK_END);
      sizeFile = ftell(fe_pl4);
      break;
    case 2:      
      lstat(aa, &f_datos1);
      sizeFile = (intmax_t) f_datos1.st_size;
      break;
    case 3:  // para formato .pl4 con Byte 0x0 = 0x84
      if (typepl4 == '1')
      {
        fseek(fe_pl4, 56, SEEK_SET);  // Byte 0x38
        fscanf(fe_pl4, "%4c", &x);    // Lee y almacena 4 Bytes (little endian, hexadecimal)
        sizeFile = x-1;        
      }  
      break;
    case 4:  // para formato .pl4 con Byte 0x0 = dígito día fecha
      if (typepl4 == '2')
      {
        fseek(fe_pl4, 43, SEEK_SET);  // Byte 0x2B
        fscanf(fe_pl4, "%4c", &x);    // Lee y almacena 4 Bytes (little endian, hexadecimal)
        sizeFile = x;
      }
      break;
    case 5:
      x = 0;
      rewind(fe_pl4);
      while(1) 
      {
        c = fgetc(fe_pl4);
        if(feof(fe_pl4)) 
        { 
          sizeFile = x;
          break ;
        }
        x++;
      }
    default:
      return 111;
  }
}

//// 112: identificar tipo de fichero .pl4
int type_of_pl4(void)  
{
  unsigned char head[11];
  int i;
  head[10] = '\0';
  rewind(fe_pl4);
  for (i = 0; i <= 9; i++) 
  {
    head[i] = fgetc(fe_pl4);
  }
  printf("head = %s , con strlen %i y sizeof %i\n",head,strlen(head),sizeof(head));
  if (head[0] == 132 && head[3] == 45 && head[7] == 45) //.pl4 tipo 1; primer Byte es 0x84
  {
    typepl4 = '1';
  }
  else if (isdigit(head[0]) && head[2] == 45 && head[6] == 45) //.pl4 tipo 2
  {
    typepl4='2';
  }
  else 
  {
    printf("ERROR: el fichero no es de ningún tipo .pl4 reconocido. %i\n",head[0]);
    exit(-1);
  }
  return 112;
}

//// 113: parámetros de la Licencia
int edx_licencia(char *aa, char *bb)
{
  printf("\n%s - Venezuela \n\tPL4toCSV versión %s\n \
  \t Licencia GPLv3 --> https://www.gnu.org/licenses/gpl-3.0.en.html \
  \n\n", aa,bb);

return 113;
}

/// 114: estructura del archivo .pl4
int estructura_pl4(void)
{
  unsigned int i;
  long int x;
  rewind(fe_pl4);
  PL4.byte_0x00 = PL4.fecha[0] = fgetc(fe_pl4);  
  if (typepl4 == '1') //.pl4 tipo 1-pe
  {
    printf("Tipo de archivo .pl4 \t\t:\t 1-pe\n");
    printf("PL4.byte1_bloque_titulos \t:\t 0x%02X\n",PL4.byte1_bloque_titulos = 80);
    
    for (i = 1; i <= 19; i++) 
    {
      PL4.fecha[i] = fgetc(fe_pl4);
    }
    PL4.fecha[20] = '\0'; // NUL '\0' de final de string

    printf("PL4.fecha \t\t\t:\t ");
    for (i = 1; i <= 19; i++) 
    {
      printf("%c",PL4.fecha[i]);
    }
    // número de variables: Método 1
    fseek(fe_pl4, 48, SEEK_SET);  // Byte 0x30
    fscanf(fe_pl4, "%4c", &PL4.variables_sin_time);    // Lee y almacena 4 Bytes (little endian, hexadecimal)
    fseek(fe_pl4, 52, SEEK_SET);  // Byte 0x34 indica comienzo Bloque Data
    fscanf(fe_pl4, "%4c", &PL4.byte1_bloque_datos);    // Lee y almacena 4 Bytes (little endian, hexadecimal)
    fseek(fe_pl4, 56, SEEK_SET);  // Byte 0x38, tamaño del Fichero
    fscanf(fe_pl4, "%4c", &PL4.size);    // Lee y almacena 4 Bytes (little endian, hexadecimal)
    printf("\nPL4.size \t\t\t:\t %i\n", PL4.size - 1);
    printf("PL4.byte1_bloque_datos \t\t:\t %i\n",PL4.byte1_bloque_datos - 1);
    printf("PL4.variables_sin_time \t\t:\t %i\n",PL4.variables_sin_time/2);
    PL4.cantidad_muestras =  ((PL4.size - 1) - (PL4.byte1_bloque_datos - 1))/(4*(PL4.variables_sin_time/2 + 1));
    printf("PL4.cantidad_muestras \t\t:\t %i\n",PL4.cantidad_muestras);
    PL4.posicion_ultimo_time = (PL4.size - 1) - 4*(PL4.variables_sin_time/2 +1);
    printf("PL4.posicion_ultimo_time \t:\t %i\n",PL4.posicion_ultimo_time);
    fseek(fe_pl4, PL4.posicion_ultimo_time, SEEK_SET); 
    fscanf(fe_pl4, "%4c", &PL4.valor_ultimo_time);    // Lee y almacena 4 Bytes (little endian, hexadecimal)
    printf("PL4.valor_ultimo_time \t\t:\t %f\n",PL4.valor_ultimo_time);
  }
  else
  {
    printf("Tipo de archivo .pl4 \t\t:\t 2-xy\n");
    printf("PL4.byte1_bloque_titulos \t:\t 0x%02X\n",PL4.byte1_bloque_titulos = 47); //primero de varios espacios seguidos (0x20)
    rewind(fe_pl4);

    for (i = 0; i <= 19; i++) 
    {
      PL4.fecha[i] = fgetc(fe_pl4);
    }
    PL4.fecha[20] = '\0'; // NUL '\0' de final de string
    printf("PL4.fecha \t\t\t:\t ");
    for (i = 0; i <= 18; i++) 
    {
      printf("%c",PL4.fecha[i]);
    }
    // número de variables: Método 1
    fseek(fe_pl4, 27, SEEK_SET);  // Byte 0x1B
    fscanf(fe_pl4, "%4c", &PL4.variables_sin_time);    // Lee y almacena 4 Bytes (little endian, hexadecimal)
    
    fseek(fe_pl4, 39, SEEK_SET);  // Byte 0x27 indica comienzo Bloque Data
    fscanf(fe_pl4, "%4c", &PL4.byte1_bloque_datos);    // Lee y almacena 4 Bytes (little endian, hexadecimal)
    fseek(fe_pl4, 43, SEEK_SET);  // Byte 0x2B, tamaño del Fichero
    fscanf(fe_pl4, "%4c", &PL4.size);    // Lee y almacena 4 Bytes (little endian, hexadecimal)
    
    printf("\nPL4.size \t\t\t:\t %i\n", PL4.size - 1);
    printf("PL4.byte1_bloque_datos \t\t:\t %i\n",PL4.byte1_bloque_datos - 1);
    printf("PL4.variables_sin_time \t\t:\t %i\n",PL4.variables_sin_time/2);
    PL4.cantidad_muestras =  ((PL4.size - 1) - (PL4.byte1_bloque_datos - 1))/(4*(PL4.variables_sin_time/2 + 1));
    printf("PL4.cantidad_muestras \t\t:\t %i\n",PL4.cantidad_muestras);
    PL4.posicion_ultimo_time = (PL4.size - 1) - 4*(PL4.variables_sin_time/2 +1);
    printf("PL4.posicion_ultimo_time \t:\t %i\n",PL4.posicion_ultimo_time);
    fseek(fe_pl4, PL4.posicion_ultimo_time, SEEK_SET); 
    fscanf(fe_pl4, "%4c", &PL4.valor_ultimo_time);    // Lee y almacena 4 Bytes (little endian, hexadecimal)
    printf("PL4.valor_ultimo_time \t\t:\t %f\n",PL4.valor_ultimo_time);
  }
  return 114;
}

/// 115: traspaso de nombres "titulos" de las variables a CSV
int titulos_pl4(void)
{
  unsigned char c;
  unsigned short int n;
  
  fprintf(fo_csv,"Tiempo(s);");
  fseek(fe_pl4, PL4.byte1_bloque_titulos, SEEK_SET);

  // caso typepl4 == '1' (tipo 1-pe) son 16 caracteres por descriptor de variable
  if (typepl4 == '1')
  {
    for (n = 1; n <= (16 * PL4.variables_sin_time/2); n++) 
    {
      fscanf(fe_pl4, "%c", &c);
      fprintf(fo_csv,"%c",c);    
      if (!(n % 16) && !(n == (16 * PL4.variables_sin_time/2))) fprintf(fo_csv,";");   
    } 
  }
  fprintf(fo_csv,"\n");  
    
  return 115;
}

/// 116: traspaso de los datos "muestras" a CSV
int muestras_pl4(void)
{
  unsigned long int i;
  unsigned long int n;
  float muestra;

  fseek(fe_pl4, PL4.byte1_bloque_datos-1, SEEK_SET);  // Byte 0x34
  // longitud de fila (Bytes) = (variables + time)*4   (cantidad de variables más el Time)
  for (i = PL4.byte1_bloque_datos-1; i < PL4.size - 1 ; i = i + (PL4.variables_sin_time/2 + 1)*4) 
  {
    for (n = 1; n <= (PL4.variables_sin_time/2 + 1); n++) 
    {
      fscanf(fe_pl4, "%4c", &muestra);
      fprintf(fo_csv,"%G",muestra);
      if (!(n==PL4.variables_sin_time/2 + 1)) fprintf(fo_csv,";"); 
    }
  fprintf(fo_csv,"\n");
  }
  return 116;
}