/*---Segunda práctica SS.OO II PISTOLEROS DE WICHITA---*/
	/*
	MIGUEL GONZÁLEZ HERRANZ
	CARLOS LEÓN ARJONA
	Grupo PA2
	*/
/*----------------------------------------------------*/

#include <iostream>
#include <Windows.h>
#include <stdio.h>
#include "pist3.h"

/*---VARIABLES GLOBALES---*/
struct variablesGlobales {
	char idenProc[26] = { 'A','B','C','D','E','F','G','H','I','J','K','L','M','N','O','P','Q','R','S','T','U','V','W','X','Y','Z' };
	int Muerto[26];
	int menor;
	int n_pistVivos;
	int n_pistoleros;
	HINSTANCE libreria;
	HANDLE hijo;
	HANDLE semaforo1;
	HANDLE semaforo2;
	HANDLE semaforo_p;
}variablesGlobales;
/*------------------------*/

/*---Funciones implementadas---*/
int buscaIndice(char);
int elegirCoordinador(void);
void calculaVivos(void);

void eliminaRecursos(void) {
	FreeLibrary(variablesGlobales.libreria);
	CloseHandle(variablesGlobales.semaforo1);
	CloseHandle(variablesGlobales.semaforo2);
	CloseHandle(variablesGlobales.semaforo_p);
	CloseHandle(variablesGlobales.hijo);
}
/*-----------------------------*/

/*---Funcion de la manejadora---*/
BOOL WINAPI consoleHandler(_In_ DWORD signal) {
	if (signal == CTRL_C_EVENT) {
		fflush(stdout);
		printf("\n\n[CTRL + C] Saliendo del programa...\n");
		fflush(stdout);
		eliminaRecursos();
		exit(0);
		return true;
	}
}
/*------------------------------*/

/*---PROTOTIPOS DE FUNCIONES DE LA LIBRERIA---*/
INT (*PIST_inicio) (unsigned int, int, int);
INT (*PIST_nuevoPistolero) (char);
CHAR (*PIST_vIctima) (void);
INT	(*PIST_disparar) (char);
INT	(*PIST_morirme) (void);
INT (*PIST_fin) (void);
/*--------------------------------------------*/

/*---Funcion del hijo---*/
DWORD WINAPI funcionHijo(LPVOID param);
/*----------------------*/

int main(int argc, char* argv[])
{
	/*---COMPROBACIÓN DEL NÚMERO DE ARGUMENTOS---*/
	if (argc < 3 || argc > 4) {
		printf("\nError en los argumentos\n");
		exit(100);
	}
	/*------------------------------------------*/
	
	/*---VARIABLES---*/
	int n_pist;
	int velocidad;
	int semilla;
	//contadores
	int i,j;
	//obtenemos el menor
	variablesGlobales.menor = 0;
	/*---------------*/

	/*---COMPROBACIÓN DE ARGUMENTOS---*/
	//Asignamos el valor a las variables n_pist y velocidad
	n_pist = atoi(argv[1]);
	velocidad = atoi(argv[2]);
	if (argv[3] == NULL) {
		semilla = 0;
	}
	else {
		semilla = atoi(argv[3]);
	}

	if (n_pist <= 1 || n_pist > 26) {
		printf("\r\nError en los argumentos\r\n");
		fflush(stdout);
		exit(100);
	}

	if (velocidad < 0) {
		printf("\r\nError en los argumentos\r\n");
		fflush(stdout);
		exit(100);
	}
	/*--------------------------------*/

	/*---Modificamos SIGINT---*/
	if (SetConsoleCtrlHandler(consoleHandler, TRUE) == 0) {
		PERROR("SetConsoleCtrlHandler");
		exit(100);
	}
	/*------------------------*/

	/*---Creación semáforos---*/
	variablesGlobales.semaforo1 = CreateSemaphore(NULL, 0, n_pist, "victima");
	if (variablesGlobales.semaforo1 == NULL) {
		PERROR("Creacion de semaforo1");
		exit(100);
	}
	variablesGlobales.semaforo2 = CreateSemaphore(NULL, 0, n_pist, "muerte");
	if (variablesGlobales.semaforo2 == NULL) {
		PERROR("Creacion de semaforo2");
		exit(100);
	}
	variablesGlobales.semaforo_p = CreateSemaphore(NULL, 0, 1, "padre");
	if (variablesGlobales.semaforo_p == NULL) {
		PERROR("Creacion de semaforo_p");
		exit(100);
	}
	/*------------------------*/

	//Cargamos el array de muertos con 1
	for (j = 0; j < n_pist; j++) {
		variablesGlobales.Muerto[j] = 1;
	}

	/*---INSTANCIAMOS LA LIBRERIA DE FUNCIONES---*/
	variablesGlobales.libreria = LoadLibrary("pist3.dll");
	if (variablesGlobales.libreria == NULL) {
		printf("\r\nError al cargar la libreria\r\n");
		fflush(stdout);
		exit(100);
	}
	/*-------------------------------------------*/

	/*---OBTENEMOS EL PUNTERO A LAS FUNCIONES---*/
	PIST_inicio = (int(*)(unsigned int,int,int))GetProcAddress(variablesGlobales.libreria, "PIST_inicio");
	if (PIST_inicio == NULL) {
		PERROR("PIST_inicio");
		eliminaRecursos();
		exit(100);
	}

	PIST_nuevoPistolero = (int(*)(char))GetProcAddress(variablesGlobales.libreria,"PIST_nuevoPistolero");
	if (PIST_nuevoPistolero == NULL) {
		PERROR("PIST_nuevoPistolero");
		eliminaRecursos();
		exit(100);
	}

	PIST_vIctima = (char(*)(void))GetProcAddress(variablesGlobales.libreria, "PIST_vIctima");
	if (PIST_vIctima == NULL) {
		PERROR("PIST_vIctima");
		eliminaRecursos();
		exit(100);
	}

	PIST_disparar = (int(*)(char))GetProcAddress(variablesGlobales.libreria, "PIST_disparar");
	if (PIST_disparar == NULL) {
		PERROR("PIST_disparar");
		eliminaRecursos();
		exit(100);
	}

	PIST_morirme = (int(*)(void))GetProcAddress(variablesGlobales.libreria, "PIST_morirme");
	if (PIST_morirme == NULL) {
		PERROR("PIST_morirme");
		eliminaRecursos();
		exit(100);
	}

	PIST_fin = (int(*)(void))GetProcAddress(variablesGlobales.libreria, "PIST_fin");
	if (PIST_fin == NULL) {
		PERROR("PIST_fin");
		eliminaRecursos();
		exit(100);
	}
	/*------------------------------------------*/

	//Llamamos a la función PIST_inicio para comenzar nuestro programa
	if (PIST_inicio(n_pist, velocidad, semilla) == -1) {
		PERROR("PIST_inicio");
		eliminaRecursos();
		exit(100);
	}

	variablesGlobales.n_pistVivos = n_pist;
	variablesGlobales.n_pistoleros = n_pist;

	/*---Código del padre---*/
	//Vamos a creacion de los hilos pertinentes
	for (i = 0; i < n_pist; i++) {
		variablesGlobales.hijo = CreateThread(NULL, NULL, funcionHijo, LPVOID(i), 0, NULL);
	}
	variablesGlobales.semaforo_p = OpenSemaphore(SEMAPHORE_ALL_ACCESS, TRUE, "padre");
	if (variablesGlobales.semaforo_p == NULL) {
		PERROR("OpenSemaphore");
		eliminaRecursos();
		exit(100);
	}
	//El padre se quedará esperando a que hayan muertos todos los hijos o todos menos 1
	if (WaitForSingleObject(variablesGlobales.semaforo_p, INFINITE) == WAIT_FAILED) {
		PERROR("Semaforo de espera del padre");
		eliminaRecursos();
		exit(100);
	}
	if (PIST_fin() == -1) {
		PERROR("PIST_fin");
		eliminaRecursos();
		exit(100);
	}
	eliminaRecursos();

	//Si han muerto todos los pistoleros, devolverá 0, si no devolverá el índice del menor
	if (variablesGlobales.n_pistVivos == 0) {
		return 0;
	}
	else {
		return variablesGlobales.menor;
	}
	/*---------------------*/

}

DWORD WINAPI funcionHijo(LPVOID param) {
	int yo = (int)param;
	char victima;
	int k, n;
	/*---Abrimos los semaforos---*/
	variablesGlobales.semaforo1 = OpenSemaphore(SEMAPHORE_ALL_ACCESS, TRUE, "victima");
	if (variablesGlobales.semaforo1 == NULL) {
		PERROR("OpenSemaphore");
		eliminaRecursos();
		exit(100);
	}
	variablesGlobales.semaforo2 = OpenSemaphore(SEMAPHORE_ALL_ACCESS, TRUE, "muerte");
	if (variablesGlobales.semaforo2 == NULL) {
		PERROR("OpenSemaphore");
		eliminaRecursos();
		exit(100);
	}
	variablesGlobales.semaforo_p = OpenSemaphore(SEMAPHORE_ALL_ACCESS, TRUE, "padre");
	if (variablesGlobales.semaforo_p == NULL) {
		PERROR("OpenSemaphore");
		eliminaRecursos();
		exit(100);
	}
	/*---------------------------*/
	if (PIST_nuevoPistolero(variablesGlobales.idenProc[yo]) == -1) {
		PERROR("PIST_nuevoPistolero");
		eliminaRecursos();
		exit(100);
	}
	while (1) {
		if (yo == variablesGlobales.menor) {
			/*---Enviamos la señal para que todos los procesos comiencen a disparar---*/
			if (ReleaseSemaphore(variablesGlobales.semaforo1, variablesGlobales.n_pistVivos - 1, NULL) == 0) {
				PERROR("Señal de disparo");
				eliminaRecursos();
				exit(100);
			}
			/*------------------------------------------------------------------------*/

			victima = PIST_vIctima();
			if (victima == '@') {
				PERROR("PIST_vIctima");
				eliminaRecursos();
				exit(100);
			}

			//Modificamos el valor del array para marcar los procesos que están muertos
			variablesGlobales.Muerto[buscaIndice(victima)] = 0;

			if (PIST_disparar(victima) == -1) {
				PERROR("PIST_disparar");
				eliminaRecursos();
				exit(100);
			}

			/*---Esperamos a que el resto de procesos hayan disparado---*/
			for (k = 0; k < variablesGlobales.n_pistVivos - 1; k++) {
				if (WaitForSingleObject(variablesGlobales.semaforo2, INFINITE) == WAIT_FAILED) {
					PERROR("Espera disparos");
					eliminaRecursos();
					exit(100);
				}
			}
			/*----------------------------------------------------------*/

			/*---Enviamos la señal para que todos los procesos mueran a la vez---*/
			if (ReleaseSemaphore(variablesGlobales.semaforo1, variablesGlobales.n_pistVivos - 1, NULL) == 0) {
				PERROR("Señal de muerte");
				eliminaRecursos();
				exit(100);
			}
			/*-------------------------------------------------------------------*/

			if (variablesGlobales.Muerto[yo] == 0) {
				if (PIST_morirme() == -1) {
					PERROR("PIST_morirme");
					eliminaRecursos();
					exit(100);
				}
			}

			/*---Esperamos a que mueran el resto de procesos---*/
			for (n = 0; n < variablesGlobales.n_pistVivos - 1; n++) {
				if (WaitForSingleObject(variablesGlobales.semaforo2, INFINITE) == WAIT_FAILED) {
					PERROR("Espera de muerte");
					eliminaRecursos();
					exit(100);
				}
			}
			/*-------------------------------------------------*/

			/*---Ahora elegiremos el nuevo coordinador de la ronda y el coordinador actual gestionará la ronda---*/
			calculaVivos();
			variablesGlobales.menor = elegirCoordinador();
			if (variablesGlobales.n_pistVivos == 0) {
				if (ReleaseSemaphore(variablesGlobales.semaforo_p, 1, NULL) == 0) {
					PERROR("Error en la señal al padre");
					eliminaRecursos();
					exit(100);
				}
				//Se suicida
				ExitThread(NULL);
			}
			if (variablesGlobales.n_pistVivos <= 1) {
				if (variablesGlobales.Muerto[yo] == 0) {
					//manda la señal al semaforo de espera de procesos
					if (ReleaseSemaphore(variablesGlobales.semaforo1, variablesGlobales.n_pistVivos, NULL) == 0) {
						PERROR("Error en la señal de ronda");
						eliminaRecursos();
						exit(100);
					}
					ExitThread(NULL);
				}
				if (variablesGlobales.Muerto[yo] == 1) {
					//Es el ultimo pistolero vivo
					//Envia la señal al padre
					if (ReleaseSemaphore(variablesGlobales.semaforo_p, 1, NULL) == 0) {
						PERROR("Error en la señal al padre");
						eliminaRecursos();
						exit(100);
					}
					//Se suicida
					ExitThread(NULL);
				}
			}
			else {
				//manda la señal de que ha terminado de gestionar la ronda y se suicida si conviene
				if (variablesGlobales.Muerto[yo] == 0) {
					//se suicida
					if (ReleaseSemaphore(variablesGlobales.semaforo1, variablesGlobales.n_pistVivos, NULL)==0) {
						PERROR("Error en la señal de ronda");
						eliminaRecursos();
						exit(100);
					}
					ExitThread(NULL);
				}
				else {
					//Manda la señal de que ha terminado de gestionar la ronda
					if (ReleaseSemaphore(variablesGlobales.semaforo1, variablesGlobales.n_pistVivos - 1, NULL) == 0) {
						PERROR("Error en la señal de ronda");
						eliminaRecursos();
						exit(100);
					}
				}
			}
			/*---------------------------------------------------------------------------------------------------*/
		}
		else {
			/*---Esperan a que el coordinador de la ronda envie la señal y empiecen a disparar---*/
			if (WaitForSingleObject(variablesGlobales.semaforo1, INFINITE) == WAIT_FAILED) {
				PERROR("Espera de disparo");
				eliminaRecursos();
				exit(100);
			}
			/*-----------------------------------------------------------------------------------*/

			victima = PIST_vIctima();
			if (victima == '@') {
				PERROR("PIST_vIctima");
				eliminaRecursos();
				exit(100);
			}

			variablesGlobales.Muerto[buscaIndice(victima)] = 0;

			if (PIST_disparar(victima) == -1) {
				PERROR("PIST_disparar");
				eliminaRecursos();
				exit(100);
			}

			/*---Manda la señal de que ya ha disparado---*/
			if (ReleaseSemaphore(variablesGlobales.semaforo2, 1, NULL) == 0) {
				PERROR("Error en la señal de disparo");
				eliminaRecursos();
				exit(100);
			}
			/*-------------------------------------------*/

			/*---Esperan la señal del proceso coordinador para morir si han sido disparados o para continuar la ronda---*/
			if (WaitForSingleObject(variablesGlobales.semaforo1, INFINITE) == WAIT_FAILED) {
				PERROR("Espera de muerte");
				eliminaRecursos();
				exit(100);
			}
			/*----------------------------------------------------------------------------------------------------------*/

			if (variablesGlobales.Muerto[yo] == 0) {
				if (PIST_morirme() == -1) {
					PERROR("PIST_morirme");
					eliminaRecursos();
					exit(100);
				}
				//Envia la señal de que ha muerto
				if (ReleaseSemaphore(variablesGlobales.semaforo2, 1, NULL) == 0) {
					PERROR("Error en la señal de muerte");
					eliminaRecursos();
					exit(100);
				}
				ExitThread(NULL);
			}

			/*---Envia la señal de que ha gestionado la muerte---*/
			if (ReleaseSemaphore(variablesGlobales.semaforo2, 1, NULL) == 0) {
				PERROR("Error en la señal de muerte");
				eliminaRecursos();
				exit(100);
			}
			/*---------------------------------------------------*/

			/*---Esperan a que el coordinador haya terminado de gestionar la ronda---*/
			if (WaitForSingleObject(variablesGlobales.semaforo1, INFINITE) == WAIT_FAILED) {
				PERROR("Espera de ronda");
				eliminaRecursos();
				exit(100);
			}
			/*-----------------------------------------------------------------------*/

			/*---Si el numero de pistoleros es 1, se suicidará y llamará al padre, si no, continuará la siguiente ronda---*/
			if (variablesGlobales.n_pistVivos == 1) {
				//Envia la señal al padre
				if (ReleaseSemaphore(variablesGlobales.semaforo_p, 1, NULL) == 0) {
					PERROR("Error en la señal al padre");
					eliminaRecursos();
					exit(100);
				}
				//Se suicida
				ExitThread(NULL);
			}
			/*------------------------------------------------------------------------------------------------------------*/

		}
	}
}

/*---Funcion para buscar el índice de la letra introducida---*/
int buscaIndice(char c) {
	int k;
	for (k = 0; k < variablesGlobales.n_pistoleros; k++) {
		if (variablesGlobales.idenProc[k] == c) {
			return k;
		}
	}
}
/*-----------------------------------------------------------*/

/*---Funcion para elegir el nuevo coordinador---*/
int elegirCoordinador() {
	int k;
	for (k = 0; k < variablesGlobales.n_pistoleros; k++) {
		if (variablesGlobales.Muerto[k] == 1) {
			return k;
		}
	}
}
/*---------------------------------------------*/

/*---Funcion para calcular los pistoleros que quedan vivos---*/
void calculaVivos() {
	int k;
	variablesGlobales.n_pistVivos = 0;
	for (k = 0; k < variablesGlobales.n_pistoleros; k++) {
		if (variablesGlobales.Muerto[k] == 1) {
			variablesGlobales.n_pistVivos = variablesGlobales.n_pistVivos + 1;
		}
	}
}
/*-----------------------------------------------------------*/