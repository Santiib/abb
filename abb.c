#include "abb.h"
#include "pila.h"
#include <string.h>
#include <stdlib.h>

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

typedef struct abb_iter {
		const abb_t* arbol;
		pila_t* pila_hojas;
		} abb_iter_t;

void abb_destruir_nodos(abb_t* arbol,nodo_abb_t* raiz);



//Crea una nueva raiz
nodo_abb_t* nueva_raiz(char* clave, void* dato);

//Modifica la raiz del arbol con la raiz nueva 'raiz' y sus dos hijos 
//PRE: El arbol fue creado
void replantar_raiz(abb_t* arbol, nodo_abb_t* raiz, nodo_abb_t* derecho, nodo_abb_t* izquierdo);

//Añade una nueva hoja a la estructura del arbol
//PRE: El arbol fue creado
void agregar_hoja(abb_t* arbol, nodo_abb_t* raiz, nodo_abb_t* hoja_nueva);

//Recorre el arbol aplicando la funcion visitar a cada nodo, se detiene cuando la funcion
//devuelva false.
void _abb_in_order(nodo_abb_t *raiz, bool visitar(const char *, void *, void *), void *extra, bool*);

//Apila los hijos izquierdos del nodo recibido por parametro en la pila
void apilar_hojas_izquierdas(abb_iter_t *iter, nodo_abb_t* nodo);

//Recibe una cadena y devuelve una copia de la misma
char *strdup(const char *old);

//Devuelve la hoja que seria candidata a reemplazar el nodo pasado por parametro
//y guarda a su padre en el segundo parametro
//Pre: El nodo existe y tiene un hijo mayor con hijos menores
nodo_abb_t* reemplazante_borrado(nodo_abb_t* nodo,nodo_abb_t** padre);

//Actualiza a un hijo de un nodo, asignandole 'nuevo_hijo'
//Pre: El nodo 'padre' y el nodo 'borrado' existen
void actualizar_padre(nodo_abb_t* padre,nodo_abb_t* nodo_borrado, nodo_abb_t* nuevo_hijo);

//Busca un nodo con la clave 'clave en el arbol, devuelve NULL si no se encuentra, caso contrario,
//devuelve al nodo y en ultimo doble puntero se almacena a su padre
nodo_abb_t* abb_buscar(const abb_t* arbol, nodo_abb_t *raiz,const char *clave,nodo_abb_t* padre_aux,nodo_abb_t** padre_clave);


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
	abb_destruir_nodos(arbol,raiz->h_derecho);
	abb_destruir_nodos(arbol,raiz->h_izquierdo);
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
	if (comparar(raiz->clave,hoja_nueva->clave) > 0){
		if(!raiz->h_izquierdo){
			raiz->h_izquierdo = hoja_nueva;
			arbol->cantidad++;
			return;
			}
		agregar_hoja(arbol,raiz->h_izquierdo,hoja_nueva);
		}
	else if (comparar(raiz->clave,hoja_nueva->clave) < 0){
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

size_t abb_cantidad(abb_t *arbol){
	return arbol->cantidad;
	}

nodo_abb_t* abb_buscar(const abb_t* arbol, nodo_abb_t *raiz,const char *clave,nodo_abb_t* padre_aux,nodo_abb_t** padre_clave){
	if (!raiz){
		return NULL;
		}
	abb_comparar_clave_t comparar = arbol->comparar_clave;
	if (comparar(raiz->clave,clave) > 0){
		padre_aux = raiz;	
		return abb_buscar(arbol,raiz->h_izquierdo,clave,padre_aux,padre_clave);
		}
	else if (comparar(raiz->clave,clave) < 0){
		padre_aux = raiz;
		return abb_buscar(arbol,raiz->h_derecho,clave,padre_aux,padre_clave);
		}
	else{
		if (padre_clave){
			padre_clave[0] = padre_aux;
			}		
		return raiz;
		}	
	}

bool abb_pertenece(const abb_t *arbol, const char *clave){
	nodo_abb_t* nodo = abb_buscar(arbol,arbol->raiz,clave,NULL,NULL);
	return nodo;
	}

void *abb_obtener(const abb_t *arbol, const char *clave){
	nodo_abb_t* nodo = abb_buscar(arbol,arbol->raiz,clave,NULL,NULL);
	void *dato = NULL;
	if (nodo){
		dato = nodo->dato;
		}
	return dato;
	}
		
nodo_abb_t* reemplazante_borrado(nodo_abb_t* nodo,nodo_abb_t** padre){
	nodo_abb_t* menor_derecho = nodo;
	while (menor_derecho->h_izquierdo){
		padre[0] = menor_derecho;
		menor_derecho = menor_derecho->h_izquierdo;
		}
	return menor_derecho;
	}

void replantar_raiz(abb_t* arbol, nodo_abb_t* nueva_raiz, nodo_abb_t* n_derecho, nodo_abb_t* n_izquierdo){
	arbol->raiz = nueva_raiz;
	if (nueva_raiz){
		nueva_raiz->h_derecho = n_derecho;
		nueva_raiz->h_izquierdo = n_izquierdo;
		}
	}

void actualizar_padre(nodo_abb_t* padre,nodo_abb_t* nodo_borrado, nodo_abb_t* nuevo_hijo)	{
	if (padre->h_izquierdo == nodo_borrado){
		padre->h_izquierdo = nuevo_hijo;
		}
	else{
		padre->h_derecho = nuevo_hijo;
		}
	}

void *abb_borrar(abb_t *arbol, const char *clave){
	nodo_abb_t** padre = malloc(sizeof(nodo_abb_t));
	if (!padre){
		return NULL;
		}
	nodo_abb_t* padre_aux = NULL;
	nodo_abb_t* nodo_borrar = abb_buscar(arbol,arbol->raiz,clave,padre_aux,padre);
	if (!nodo_borrar){
		free(padre);
		return NULL;
		}
	void *dato = nodo_borrar->dato;
	nodo_abb_t* padre_nodo_borrar = padre[0];
	
	//CASO 1: El nodo a borrar no tiene hijos

	if (!nodo_borrar->h_izquierdo && !nodo_borrar->h_derecho){
		if (nodo_borrar != arbol->raiz){
			actualizar_padre(padre_nodo_borrar,nodo_borrar,NULL);
			}
		else{
			replantar_raiz(arbol,NULL,NULL,NULL);
			}
		}

	//CASO 2: El nodo a borrar tiene 1 unico hijo

	else if (!nodo_borrar->h_izquierdo){ //El unico hijo esta a su derecha
		if (nodo_borrar != arbol->raiz){ 
			actualizar_padre(padre_nodo_borrar,nodo_borrar,nodo_borrar->h_derecho);
			}
		else{
			replantar_raiz(arbol,nodo_borrar->h_derecho,nodo_borrar->h_derecho->h_derecho,nodo_borrar->h_derecho->h_izquierdo);
			}
		}
	else if (!nodo_borrar->h_derecho){ //El unico hijo esta a su izquierda
		if (nodo_borrar != arbol->raiz){ 
			actualizar_padre(padre_nodo_borrar,nodo_borrar,nodo_borrar->h_izquierdo);
			}
		else{
			replantar_raiz(arbol,nodo_borrar->h_izquierdo,nodo_borrar->h_izquierdo->h_derecho,nodo_borrar->h_izquierdo->h_izquierdo);
			}
		}

	//CASO 3: El nodo a borrar tiene 2 hijos

	else if (!nodo_borrar->h_derecho->h_izquierdo){ //3.1: El hijo derecho no tiene sucesores menores
		if (nodo_borrar != arbol->raiz){	
			actualizar_padre(padre_nodo_borrar,nodo_borrar,nodo_borrar->h_derecho);
			nodo_borrar->h_derecho->h_izquierdo = nodo_borrar->h_izquierdo;
			}
		else{
			replantar_raiz(arbol,nodo_borrar->h_derecho,nodo_borrar->h_derecho->h_derecho,nodo_borrar->h_izquierdo);
			}		
		}	
	else { //3.2: El hijo derecho tiene sucesores menores
		nodo_abb_t** padre_reemplazante_aux = malloc(sizeof(nodo_abb_t));
		nodo_abb_t* reemplazante = reemplazante_borrado(nodo_borrar->h_derecho,padre_reemplazante_aux);
		nodo_abb_t* padre_reemplazante = padre_reemplazante_aux[0];
		padre_reemplazante->h_izquierdo = NULL;
		if (reemplazante->h_derecho){
			padre_reemplazante->h_izquierdo = reemplazante->h_derecho;
			}
		if (nodo_borrar == arbol->raiz){
			replantar_raiz(arbol,reemplazante,nodo_borrar->h_derecho,nodo_borrar->h_izquierdo);
			}
		else{
			actualizar_padre(padre_nodo_borrar,nodo_borrar,reemplazante);				
			reemplazante->h_derecho = nodo_borrar->h_derecho;
			reemplazante->h_izquierdo = nodo_borrar->h_izquierdo;
			}
		free(padre_reemplazante_aux); 
		}
	if (arbol->como_destruir){		
		arbol->como_destruir(nodo_borrar->dato);
		}
	arbol->cantidad--;
	free(nodo_borrar->clave);
	free(nodo_borrar);
	free(padre);
	return dato;
	}

abb_iter_t *abb_iter_in_crear(const abb_t *arbol){
	abb_iter_t *iter = malloc(sizeof(abb_iter_t));
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

void apilar_hojas_izquierdas(abb_iter_t *iter, nodo_abb_t* nodo){
	while (nodo){
		pila_apilar(iter->pila_hojas,nodo);
		nodo = nodo->h_izquierdo;
		}
	}

bool abb_iter_in_avanzar(abb_iter_t *iter){
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
const char *abb_iter_in_ver_actual(const abb_iter_t *iter){
	if (pila_esta_vacia(iter->pila_hojas)){
		return NULL;
		}
	nodo_abb_t* nodo_actual = pila_ver_tope(iter->pila_hojas);
	const char* clave_actual = nodo_actual->clave;
	return clave_actual;
	}

bool abb_iter_in_al_final(const abb_iter_t *iter){
	return pila_esta_vacia(iter->pila_hojas);
	}

void abb_iter_in_destruir(abb_iter_t* iter){
	pila_destruir(iter->pila_hojas);
	free(iter);
	}

void _abb_in_order(nodo_abb_t *raiz, bool visitar(const char *, void *, void *), void *extra, bool *continuar){
	if (raiz == NULL || *continuar == false){
		return;
		}
	if (raiz->h_izquierdo && *continuar){
		_abb_in_order(raiz->h_izquierdo,visitar,extra,continuar);
		}
	*continuar = visitar(raiz->clave,raiz->dato,extra);
	
	if (*continuar && raiz->h_derecho){
		_abb_in_order(raiz->h_derecho,visitar,extra,continuar);
		}
	}

void abb_in_order(abb_t *arbol, bool visitar(const char *, void *, void *), void *extra){
	if (arbol->raiz == NULL){
		return;
		}
	bool *continuar = malloc(sizeof(bool));
	*continuar = true;
	_abb_in_order(arbol->raiz,visitar,extra,continuar);
	free(continuar);
	}
