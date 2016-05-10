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

	//HASTA ACA funcionan bien, habria que generalizarlo en unica funcion

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
	else { //el hijo derecho tiene sucesores menores
		nodo_abb_t** padre_reemplazante_aux = malloc(sizeof(nodo_abb_t));
		nodo_abb_t* reemplazante = reemplazante_borrado(nodo_borrar->h_derecho,padre_reemplazante_aux);
		nodo_abb_t* padre_reemplazante = padre_reemplazante_aux[0];
		padre_reemplazante->h_izquierdo = NULL;
		if (reemplazante->h_derecho){ //el reemplazante tiene hijo derecho
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
