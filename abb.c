#include <stdbool.h>
#include <stdlib.h>
#include "abb.h"
#include "pila.h"
#include <string.h>
#include <stdio.h>

typedef struct nodo_abb { 		
		struct nodo_abb* h_izquierdo;
		struct nodo_abb* h_derecho;
		char* clave;
		void* dato;
		} nodo_abb_t;

typedef struct abb {
	nodo_abb_t* raiz; 
	abb_destruir_dato_t como_destruir;
	size_t cantidad;
	abb_comparar_clave_t comparar_clave;
	} abb_t;

typedef struct abb_in_iter {
		const abb_t* arbol;
		pila_t* pila_hojas;
		} abb_in_iter_t;

void abb_destruir_nodos(abb_t* arbol,nodo_abb_t* raiz);

//Busca el padre de 'nodo'
nodo_abb_t* buscar_padre(abb_t* arbol,nodo_abb_t* raiz, nodo_abb_t* nodo);

//Crea una nueva raiz
nodo_abb_t* nueva_raiz(char* clave, void* dato);

//Añade una nueva hoja a la estructura del arbol
void agregar_hoja(abb_t* arbol, nodo_abb_t* raiz, nodo_abb_t* hoja_nueva);

//Busca el nodo con la clave pasada por parametro y lo devuelve
nodo_abb_t* abb_buscar(const abb_t* arbol, nodo_abb_t *raiz,const char *clave);

//Devuelve la hoja que seria candidata a reemplazar el nodo pasado por parametro
nodo_abb_t* reemplazante(nodo_abb_t* nodo);

void _abb_in_order(nodo_abb_t *raiz, bool visitar(const char *, void *, void *), void *extra);

//Apila los hijos izquierdos del nodo recibido por parametro en la pila
void apilar_hojas_izquierdas(abb_in_iter_t *iter, nodo_abb_t* nodo);

//Recibe una cadena y devuelve una copia de la misma
char *strdup(const char *old);

char *strdup(const char *old) {
	if (!old){
		return NULL;
		}
    char *new;
    if((new = malloc(strlen(old) + 1)) == NULL){
        return NULL;
        }
    strcpy(new, old);
    return new;
}

nodo_abb_t* nueva_raiz(char* clave, void* dato){
	nodo_abb_t *nodo = malloc(sizeof(nodo_abb_t));
	if (!nodo){
		return NULL;
		}
	nodo->h_derecho = NULL;
	nodo->h_izquierdo = NULL;
	nodo->clave = clave;
	nodo->dato = dato;
	return nodo;
	}

abb_t* abb_crear(abb_comparar_clave_t cmp, abb_destruir_dato_t destruir_dato){

	if (!cmp){
		return NULL;
		}
	abb_t* arbol_binario = malloc(sizeof(abb_t));
	if(!arbol_binario){
		return NULL;
		}
	arbol_binario->raiz = NULL;
	arbol_binario->cantidad = 0;
	arbol_binario->como_destruir = destruir_dato;
	arbol_binario->comparar_clave = cmp;
	return arbol_binario;
	}

void abb_destruir_nodos(abb_t* arbol,nodo_abb_t* raiz){
	
	if (!raiz){
		return;
		}
	//printf("toy en la raz con clave %s con direccion %p, su hijo derecho es %p, su izquierdo es %p\n",raiz->clave,raiz,raiz->h_derecho,raiz->h_izquierdo);
	abb_destruir_nodos(arbol,raiz->h_derecho);
	abb_destruir_nodos(arbol,raiz->h_izquierdo);
	//printf("Voy a liberar la raz con clave %s, su direccion es %p, su hijo derecho es %p su izquierdo es %p\n",raiz->clave,raiz,raiz->h_derecho,raiz->h_izquierdo);
	//puts("Voy a hacer free");
	free(raiz->clave);
	if(raiz->dato && arbol->como_destruir){
		arbol->como_destruir(raiz->dato);
		}
	free(raiz);
	}	
		
void abb_destruir(abb_t *arbol){
	abb_destruir_nodos(arbol,arbol->raiz);
	free(arbol);
	}

void agregar_hoja(abb_t* arbol, nodo_abb_t* raiz, nodo_abb_t* hoja_nueva){
	
	abb_comparar_clave_t comparar = arbol->comparar_clave;
	
	if (comparar(raiz->clave,hoja_nueva->clave) == 1){
		if(!raiz->h_izquierdo){
			raiz->h_izquierdo = hoja_nueva;
			arbol->cantidad++;
			return;
			}
		agregar_hoja(arbol,raiz->h_izquierdo,hoja_nueva);
		}
	else if (comparar(raiz->clave,hoja_nueva->clave) == -1){
		if(!raiz->h_derecho){
			raiz->h_derecho = hoja_nueva;
			arbol->cantidad++;
			return;
			}
		agregar_hoja(arbol,raiz->h_derecho,hoja_nueva);
		}
	else{
		if (arbol->como_destruir){
			arbol->como_destruir(raiz->dato);
			}
		raiz->dato = hoja_nueva->dato;
		free(hoja_nueva->clave);
		free(hoja_nueva);
		}
	}	
	
bool abb_guardar(abb_t *arbol, const char *clave, void *dato){
	char* copia_clave = strdup(clave);
	nodo_abb_t *nueva_hoja = nueva_raiz(copia_clave,dato);
	
	if (!nueva_hoja){
		return false;
		}
	
	if (!arbol->raiz){
		arbol->raiz = nueva_hoja;
		arbol->cantidad++;
		return true;
		}

	agregar_hoja(arbol,arbol->raiz,nueva_hoja);
	return true;
	}

size_t abb_cantidad(const abb_t *arbol){
	return arbol->cantidad;
	}

nodo_abb_t* abb_buscar(const abb_t* arbol, nodo_abb_t *raiz,const char *clave){
	if (!raiz){
		return NULL;
		}
	abb_comparar_clave_t comparar = arbol->comparar_clave;
	if (comparar(raiz->clave,clave) == 1){
		return abb_buscar(arbol,raiz->h_izquierdo,clave);
		}
	else if (comparar(raiz->clave,clave) == -1){
		return abb_buscar(arbol,raiz->h_derecho,clave);
		}
	else{
		return raiz;
		}	
	}

bool abb_pertenece(const abb_t *arbol, const char *clave){
	nodo_abb_t* nodo = abb_buscar(arbol,arbol->raiz,clave);
	return nodo;
	}
void *abb_obtener(const abb_t *arbol, const char *clave){
	nodo_abb_t* nodo = abb_buscar(arbol,arbol->raiz,clave);
	void *dato = NULL;
	if (nodo){
		dato = nodo->dato;
		}
	return dato;
	}

nodo_abb_t* buscar_padre(abb_t* arbol,nodo_abb_t* raiz,nodo_abb_t* nodo){
	
	if (raiz == nodo){
		return NULL;
		}
	if ((raiz->h_derecho == nodo) || (raiz->h_izquierdo == nodo)){
		return raiz;
		}
	else if (arbol->comparar_clave(raiz->clave,nodo->clave) == 1){
		return buscar_padre(arbol,raiz->h_izquierdo,nodo);
		}
	else{
		return buscar_padre(arbol,raiz->h_derecho,nodo);
		}
	}
		
nodo_abb_t* reemplazante_borrado(nodo_abb_t* nodo){
	
	if (!nodo->h_izquierdo && !nodo->h_derecho){
		return NULL;
		}
	else if(!nodo->h_derecho){ //su remplazo sera su hijo izquierdo
		return nodo->h_izquierdo;
		}
	else{ //tengo que buscar el hijo mas a la izquierda del hijo derecho
		nodo_abb_t* nieto_derecho = nodo->h_derecho->h_izquierdo;
		if (!nieto_derecho){
			return nodo->h_derecho;
			}
		while (nieto_derecho->h_izquierdo){
			nieto_derecho = nieto_derecho->h_izquierdo;
			}
		return nieto_derecho;
		}
	}
void *abb_borrar(abb_t *arbol, const char *clave){
	nodo_abb_t* nodo_a_borrar = abb_buscar(arbol,arbol->raiz,clave);
	if (!nodo_a_borrar){
		return NULL;
		}
	void *dato = nodo_a_borrar->dato;
	nodo_abb_t* padre_nodo_a_borrar = buscar_padre(arbol,arbol->raiz,nodo_a_borrar);
	nodo_abb_t* hijo_izquierdo_padre_nodo_borrar = NULL;
	nodo_abb_t* hijo_derecho_padre_nodo_borrar = NULL;
	if (padre_nodo_a_borrar){
		hijo_izquierdo_padre_nodo_borrar = padre_nodo_a_borrar->h_izquierdo;
		hijo_derecho_padre_nodo_borrar = padre_nodo_a_borrar->h_derecho;
		}
	nodo_abb_t* reemplazante = reemplazante_borrado(nodo_a_borrar);
	if (padre_nodo_a_borrar){
		printf("El padre del nodo a borrar es %s\n",padre_nodo_a_borrar->clave);
		}
	if (hijo_izquierdo_padre_nodo_borrar){
		printf("El hijo izquierdo del padre del nodo a borrar es %s\n",hijo_izquierdo_padre_nodo_borrar->clave);
		}
	if (hijo_derecho_padre_nodo_borrar){
		printf("El hijo derecho del padre del nodo a borrar es %s\n",hijo_derecho_padre_nodo_borrar->clave);
		}	
	nodo_abb_t* reemplazante_h_derecho = NULL;
	nodo_abb_t* reemplazante_h_izquierdo = NULL;
	nodo_abb_t* padre_reemplazante = NULL;
	nodo_abb_t* padre_reemplazante_h_derecho = NULL;
	nodo_abb_t* padre_reemplazante_h_izquierdo = NULL;
	if (reemplazante){		
		printf("La clave de reemplazante es %s\n",reemplazante->clave);
		reemplazante_h_derecho = reemplazante->h_derecho;
		if (reemplazante_h_derecho){
			printf("La clave del hijo derecho del reemplazante es %s\n",reemplazante->h_derecho->clave);
			}
		reemplazante_h_izquierdo = reemplazante->h_izquierdo;
		if (reemplazante_h_izquierdo){
			printf("La clave del hijo izquierdo del reemplazante es %s\n",reemplazante->h_izquierdo->clave);
			}
		padre_reemplazante = buscar_padre(arbol,arbol->raiz,reemplazante);
		if (padre_reemplazante){
			printf("La clave del padre del reemplazante es es %s\n",padre_reemplazante->clave);
			}
		padre_reemplazante_h_derecho = padre_reemplazante->h_derecho;
		if (padre_reemplazante_h_derecho){
			printf("La clkave del hijo derecho del padre del reemplazante es %s\n",padre_reemplazante->h_derecho->clave);
			}
	    padre_reemplazante_h_izquierdo = padre_reemplazante->h_izquierdo;
		if (padre_reemplazante_h_izquierdo){
			printf("La clave izquierda del padre del reemplazante es %s\n",padre_reemplazante->h_derecho->clave);
			}
		}
	if (arbol->como_destruir){		
		arbol->como_destruir(nodo_a_borrar->dato);
		}
	
	arbol->cantidad--;
	free(nodo_a_borrar->clave);
	free(nodo_a_borrar);
	return dato;
	}


abb_in_iter_t *abb_iter_in_crear(const abb_t *arbol){
	abb_in_iter_t *iter = malloc(sizeof(abb_in_iter_t));
	if (!iter){
		return NULL;
		}
	pila_t* pila = pila_crear();
	if (!pila){
		return NULL;
		}
	iter->pila_hojas = pila;
	apilar_hojas_izquierdas(iter,arbol->raiz);
	iter->arbol = arbol;
	return iter;
	}

void apilar_hojas_izquierdas(abb_in_iter_t *iter, nodo_abb_t* nodo){
	
	while (nodo){
		pila_apilar(iter->pila_hojas,nodo);
		nodo = nodo->h_izquierdo;
		}
	}

bool abb_iter_in_avanzar(abb_in_iter_t *iter){
	
	if (pila_esta_vacia(iter->pila_hojas)){
		return false;
		}
	
	nodo_abb_t* hoja_actual = pila_ver_tope(iter->pila_hojas);	
	nodo_abb_t* actual_h_derecho = hoja_actual->h_derecho;
	pila_desapilar(iter->pila_hojas);
	if (actual_h_derecho){
		pila_apilar(iter->pila_hojas,actual_h_derecho);
		if (actual_h_derecho->h_izquierdo){
		apilar_hojas_izquierdas(iter,actual_h_derecho->h_izquierdo);
			}
		}
	
	return true;
	}
	
	
const char *abb_iter_in_ver_actual(const abb_in_iter_t *iter){
	if (pila_esta_vacia(iter->pila_hojas)){
		return NULL;
		}
	nodo_abb_t* nodo_actual = pila_ver_tope(iter->pila_hojas);
	const char* clave_actual = nodo_actual->clave;
	return clave_actual;
	}

bool abb_iter_in_al_final(const abb_in_iter_t *iter){
	return pila_esta_vacia(iter->pila_hojas);
	}

void abb_iter_in_destruir(abb_in_iter_t* iter){
	pila_destruir(iter->pila_hojas);
	free(iter);
	}

void _abb_in_order(nodo_abb_t *raiz, bool visitar(const char *, void *, void *), void *extra){
	
	
	if (raiz == NULL){
		return;
		}
	if (raiz->h_izquierdo){
		_abb_in_order(raiz->h_izquierdo,visitar,extra);
		}
	bool continuar = visitar(raiz->clave,raiz->dato,extra);
	if (continuar && raiz->h_derecho){
		_abb_in_order(raiz->h_derecho,visitar,extra);
		}
	}
void abb_in_order(abb_t *arbol, bool visitar(const char *, void *, void *), void *extra){
	if (arbol->raiz == NULL){
		return;
		}
	_abb_in_order(arbol->raiz,visitar,extra);
	}

/*bool imprimir(const char* clave, void* dato, void* extra);	
bool imprimir(const char* clave, void* dato, void* extra){
	printf("El dato de la clave %s es %d\n",clave,(int)dato);
	return  true;
	}
bool dupdatos(const char* clave, void* dato, void* extra);	

bool dupdatos(const char* clave, void* dato, void* extra){
	int factor = (int)extra;
	dato = (void*)((int)dato*factor);
	return true;
*/	
