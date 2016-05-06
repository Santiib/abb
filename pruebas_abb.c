#include "abb.h"
#include "testing.h"
#include <stddef.h>
#include <stdio.h>

void pruebas_lista_alumno(){
	
	printf("%s","~~~ CREAR ~~~");
	abb_t *abb = abb_crear(abb_comparar_clave_t cmp, NULL);
	print_test("abb vacio fue creado",true);
	print_test("abb cantidad es igual 0", abb_cantidad(abb) == 0);
	
	printf("%s","~~~ ARBOL ~~~");
	int n1 = 7, n2 = 1, n3 = 3;
	const char* c1 = "c1", c2 = "c2", c3 = "c3";
	
	print_test("borrar c1 da NULL", abb_borrar(abb, c1) == NULL);
	print_test("obtener c1 da NULL", abb_obtener(abb, c1) == NULL);
	print_test("c1 pertenece es false", !abb_pertenece(abb, c1));
	print_test("guardar n1 en c1", abb_guardar(abb, c1, &n1));
	print_test("obteber c1 da n1", abb_obtener(abb, c1) == &n1);
	print_test("cantidad es igual a 1", abb_cantidad(abb) == 1);
	print_test("c1 pertenece es true", abb_pertenece(abb, c1));
	print_test("borrar c1", abb_borrar(abb, c1));
	print_test("obtener c1 da NULL", abb_obtener(abb, c1) == NULL);
	print_test("c1 pertenece es false", !abb_pertenece(abb, c1));
	print_test("cantidad es igual a 0", abb_cantidad(abb) == 0);
	print_test("guardar n1 en c1", abb_guardar(abb, c1, &n1));
	print_test("guardar n2 en c2", abb_guardar(abb, c2, &n2));
	print_test("guardar n3 en c3", abb_guardar(abb, c3, &n3));
	
	printf("%s","~~~ ITERADOR ~~~");
	abb_iter_t *iter = abb_iter_crear(abb);
	print_test("abb iter fue creado",true);
	
	print_test("iter avanzar", abb_iter_in_avanzar(iter));
	print_test("iter avanzar", abb_iter_in_avanzar(iter));
	print_test("iter avanzar", abb_iter_in_avanzar(iter));
	print_test("iter ver actual es c3", abb_iter_in_ver_actual(iter) == c3);
	print_test("iter al final es false", !abb_iter_in_al_final(iter));
	print_test("iter avanzar", abb_iter_in_avanzar(iter));
	print_test("iter ver actual es NULL", abb_iter_in_ver_actual(iter) == NULL);
	print_test("iter avanzar es false", !abb_iter_in_avanzar(iter));
	print_test("iter al final", abb_iter_in_al_final(iter));
	
	
	printf("%s","~~~ DESTRUIR ~~~");
	abb_iter_in_destruir(iter);
	print_test("El iterador fue destruido",true);
	void abb_destruir(abb);
	print_test("El arbol fue destruido",true);
	
	
}
