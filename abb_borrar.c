
//Busca el padre de 'nodo'
nodo_abb_t* buscar_padre(abb_t* arbol,nodo_abb_t* raiz, nodo_abb_t* nodo){
	nodo_abb_t *nodo_anterior;
	nodo_abb_t *nodo_actual = raiz;
	int comparacion = arbol->comparar_clave(nodo_actual->clave, nodo->clave);
	while(comparacion != 0){
		nodo_anterior = nodo_actual;
		if(comparacion < 0){
			nodo_actual = nodo_actual->h_izquierdo;
		}else{
			nodo_actual = nodo_actual->h_derecho;
		}
		comparacion = arbol->comparar_clave(nodo_actual->clave, nodo->clave);
	}
	return nodo_anterior;
}



//dado un nodo busca donde termina por su lado mayor(derecha) y devuelve el ultimo nodo.
nodo_abb_t *buscar_ult_mayor(nodo_abb_t* nodo){
	nodo_abb_t *nodo_actual = nodo;
	while(nodo_actual->h_derecho != NULL){
		nodo_actual = nodo_actual->h_derecho;
	}
	return nodo_actual;
}

//FALTA en caso de que el nodo sea la raiz, se podria redireccionar a una funcion borrar_raiz().	
void *abb_borrar(abb_t *arbol, const char *clave){
	nodo_abb_t* nodo_a_borrar = abb_buscar(arbol,arbol->raiz,clave);
	if (!nodo_a_borrar){
		return NULL;
	}
	void *dato = nodo_a_borrar->dato;
	nodo_abb_t* padre_nodo_a_borrar = buscar_padre(arbol,arbol->raiz,nodo_a_borrar);
	bool hijo_izquierdo_de_padre = arbol->comparar_clave(nodo_a_borrar->clave, padre_nodo_a_borrar->clave) < 0;
	
	if((nodo_a_borrar->h_izquierdo == NULL) && (nodo_a_borrar->h_derecho == NULL)){
		
		free(nodo_a_borrar);
	
	}else if((nodo_a_borrar->h_izquierdo != NULL) && (nodo_a_borrar->h_derecho != NULL)){
		
		if(hijo_izquierdo_de_padre){
			padre_nodo_a_borrar->h_izquierdo = nodo_a_borrar->h_izquierdo;
		}else{
			padre_nodo_a_borrar->h_derecho = nodo_a_borrar->h_izquierdo;
		}
		nodo_abb_t *utltimo_mayor = buscar_ult_mayor(nodo_a_borrar->izquierdo);
		ultimo_mayor->h_derecho = nodo_a_borrar->h_derecho;
	
	}else{
		
		if(nodo_a_borrar->h_izquierdo != NULL){
			if(hijo_izquierdo_de_padre){
				padre_nodo_a_borrar->h_izquierdo = nodo_a_borrar->izquierdo;
			}else{
				padre_nodo_a_borrar->h_derecho = nodo_a_borrar->izquierdo;
			}
		}else{
			if(hijo_izquierdo_de_padre){
				padre_nodo_a_borrar->h_izquierdo = nodo_a_borrar->derecho;
			}else{
				padre_nodo_a_borrar->h_derecho = nodo_a_borrar->derecho;
			}
			
		}
	
	}
	
	free(nodo_a_borrar);
	return dato;
}
