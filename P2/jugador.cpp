#include "../Comportamientos_Jugador/jugador.hpp"
#include "motorlib/util.h"

#include <iostream>
#include <cmath>
#include <set>
#include <stack>
#include <queue>
#include <algorithm>


// Este es el método principal que debe contener los 4 Comportamientos_Jugador
// que se piden en la práctica. Tiene como entrada la información de los
// sensores y devuelve la acción a realizar.
Action ComportamientoJugador::think(Sensores sensores) {
Action accion = actIDLE;
	// Estoy en el nivel 1
	if (sensores.nivel != 4 && hayplan == false){
		actual.fila        = sensores.posF;
		actual.columna     = sensores.posC;
		actual.orientacion = sensores.sentido;

		cout << "Fila: " << actual.fila << endl;
		cout << "Col : " << actual.columna << endl;
		cout << "Ori : " << actual.orientacion << endl;
		
		destino.fila       = sensores.destinoF;
		destino.columna    = sensores.destinoC;
		hayplan = pathFinding (sensores.nivel, actual, destino, plan);
	}
	// Estoy en el nivel 2
	else if (sensores.nivel == 4){
		
		if (inicio){
			inicio = false;
			brujula = sensores.sentido;
		}

		if (lastm == actTURN_R){
			brujula = (brujula + 1)%4;
		}
		if (lastm == actTURN_L){
			brujula = (brujula + 3)%4;
		}
		
		fil = sensores.posF;
		col = sensores.posC;

		PintarMapa(sensores);
		RellenarMapa(sensores);

		cout << "Fil: " << fil << endl;
		cout << "Col : " << col << endl;
		cout << "Ori : " << brujula << endl;

		cout << "Fil: " << destino.fila << endl;
		cout << "Col : " << destino.columna << endl;

		destino.fila       = sensores.destinoF;
		destino.columna    = sensores.destinoC;
		
		actual.fila = fil;
		actual.columna = col;
		actual.orientacion = brujula;

		if (zapatillas and bikini)
			recarga = 300;

		if (!hayplan and sensores.bateria > recarga){
			hayplan = pathFinding_CostoUniforme_Nivel2(actual, destino, plan);
		}
		else if ((find and sensores.bateria <= recarga) or (find and descanso < 90 and descanso !=0)){
			if (descanso == 90 )
				descanso = 0;
			if (actual.fila == posObj.first and actual.columna == posObj.second ){
				descanso +=1;
				lastm = actIDLE;
				return actIDLE;
			}
			if (!hayplanb){
				hayplanb = pathFinding_bateria_Nivel2(actual, plan);
			}
		}
		else if (!hayplan){
			hayplan = pathFinding_CostoUniforme_Nivel2(actual, destino, plan);
		}
				
		if (hayplan && plan.size()>0){
			accion = *plan.begin();
			plan.pop_front();
			if (accion == actFORWARD){
				if(sensores.terreno[2]=='P' or sensores.terreno[2]=='M' or sensores.superficie[2]=='a'){
					accion = actIDLE;
					hayplan = false;
				}

				if (sensores.terreno[2]=='K')
					bikini = true;

				if (sensores.terreno[2]=='D')
					zapatillas = true;
			}

			lastm = accion;
			return accion;		
		} else if(hayplanb && plan.size()>0){
			accion = *plan.begin();
			plan.pop_front();	
			lastm = accion;
			return accion;	
		}
		else {
			hayplan = false;
			hayplanb = false;
		}
	}
	if (hayplan && plan.size()>0){
		accion = *plan.begin();
		plan.pop_front();			
	}
	else 
		cout << "Algo falla" << endl;
	return accion;
	
}

void ComportamientoJugador::HayObjetivo(char a,int b,int c){	
	if (a == 'X' and !find){
		posObj.first = b;
		posObj.second = c;
		find = true;
	}
}

void ComportamientoJugador::PintarMapa(Sensores sensores){
	int posRejex = brujula%2;
	int sentido = -1;
	
	if (brujula < 2)
		sentido = 1;
	
	mapaNivel2[fil][col] = mapaNivel2[fil][col];
	switch(posRejex){
		case 0:
			for (int i=1;i<4;i++)
				mapaResultado[fil-sentido][col+sentido*(i-2)] = mapaNivel2[fil-sentido][col+sentido*(i-2)];
			for (int i=4;i<9;i++)
				mapaResultado[fil-sentido*2][col+sentido*(i-6)] = mapaNivel2[fil-sentido*2][col+sentido*(i-6)];
			for (int i=9;i<16;i++)
				mapaResultado[fil-sentido*3][col+sentido*(i-12)] = 	mapaNivel2[fil-sentido*3][col+sentido*(i-12)];
			break;
		case 1:
			for (int i=1;i<4;i++)
				mapaResultado[fil+sentido*(i-2)][col+sentido] = mapaNivel2[fil+sentido*(i-2)][col+sentido];
			for (int i=4;i<9;i++)
				mapaResultado[fil+sentido*(i-6)][col+sentido*2] = mapaNivel2[fil+sentido*(i-6)][col+sentido*2];
			for (int i=9;i<16;i++)
				mapaResultado[fil+sentido*(i-12)][col+sentido*3] = mapaNivel2[fil+sentido*(i-12)][col+sentido*3];
			break;
	}
}

void ComportamientoJugador::RellenarMapa(Sensores sensores){
	int posRejex = brujula%2;
	int sentido = -1;
	
	if (brujula < 2)
		sentido = 1;
	
	mapaNivel2[fil][col] = sensores.terreno[0];
	switch(posRejex){
		case 0:
			for (int i=1;i<4;i++){
				mapaNivel2[fil-sentido][col+sentido*(i-2)] = sensores.terreno[i];
				HayObjetivo(sensores.terreno[i],fil-sentido,col+sentido*(i-2));
			}
			for (int i=4;i<9;i++){
				mapaNivel2[fil-sentido*2][col+sentido*(i-6)] = sensores.terreno[i];
				HayObjetivo(sensores.terreno[i],fil-sentido*2,col+sentido*(i-6));
			}
			for (int i=9;i<16;i++){
				mapaNivel2[fil-sentido*3][col+sentido*(i-12)] = sensores.terreno[i];
				HayObjetivo(sensores.terreno[i],fil-sentido*3,col+sentido*(i-12));
			}
			break;
		case 1:
			for (int i=1;i<4;i++){
				mapaNivel2[fil+sentido*(i-2)][col+sentido] = sensores.terreno[i];
				HayObjetivo(sensores.terreno[i],fil+sentido*(i-2),col+sentido);
			}
			for (int i=4;i<9;i++){
				mapaNivel2[fil+sentido*(i-6)][col+sentido*2] = sensores.terreno[i];
				HayObjetivo(sensores.terreno[i],fil+sentido*(i-6),col+sentido*2);
			}
			for (int i=9;i<16;i++){
				mapaNivel2[fil+sentido*(i-12)][col+sentido*3] = sensores.terreno[i];
				HayObjetivo(sensores.terreno[i],fil+sentido*(i-12),col+sentido*3);
			}
			break;
	}
}

// Llama al algoritmo de busqueda que se usará en cada comportamiento del agente
// Level representa el comportamiento en el que fue iniciado el agente.
bool ComportamientoJugador::pathFinding (int level, const estado &origen, const estado &destino, list<Action> &plan){
	switch (level){
		case 1: cout << "Busqueda en profundad\n";
			    return pathFinding_Profundidad(origen,destino,plan);
					break;
		case 2: cout << "Busqueda en Anchura\n";
			    return pathFinding_Anchura(origen,destino,plan);
					break;
		case 3: cout << "Busqueda Costo Uniforme\n";
				return pathFinding_CostoUniforme(origen,destino,plan);
					break;
		case 4: cout << "Busqueda para el reto\n";
				return pathFinding_CostoUniforme_Nivel2(origen,destino,plan);
					break;
	}
	cout << "Comportamiento sin implementar\n";
	return false;
}


//---------------------- Implementación de la busqueda en profundidad ---------------------------

// Dado el código en carácter de una casilla del mapa dice si se puede
// pasar por ella sin riegos de morir o chocar.
bool EsObstaculo(unsigned char casilla){
	if (casilla=='P' or casilla=='M')
		return true;
	else
	  return false;
}


// Comprueba si la casilla que hay delante es un obstaculo. Si es un
// obstaculo devuelve true. Si no es un obstaculo, devuelve false y
// modifica st con la posición de la casilla del avance.
bool ComportamientoJugador::HayObstaculoDelante(estado &st){
	int fil=st.fila, col=st.columna;

  // calculo cual es la casilla de delante del agente
	switch (st.orientacion) {
		case 0: fil--; break;
		case 1: col++; break;
		case 2: fil++; break;
		case 3: col--; break;
	}

	// Compruebo que no me salgo fuera del rango del mapa
	if (fil<0 or fil>=mapaResultado.size()) return true;
	if (col<0 or col>=mapaResultado[0].size()) return true;

	// Miro si en esa casilla hay un obstaculo infranqueable
	if (!EsObstaculo(mapaResultado[fil][col])){
		// No hay obstaculo, actualizo el parámetro st poniendo la casilla de delante.
  	 	st.fila = fil;
		st.columna = col;
		return false;
	}
	else{
	  return true;
	}
}

struct nodo{
	estado st;
	list<Action> secuencia;
};

struct nodoP{
	estado st;
	list<Action> secuencia;
	int costo=0;
};

struct ComparaEstados{
	bool operator()(const estado &a, const estado &n) const{
		if ((a.fila > n.fila) or (a.fila == n.fila and a.columna > n.columna) or
	      (a.fila == n.fila and a.columna == n.columna and a.orientacion > n.orientacion))
			return true;
		else
			return false;
	}
};


// Implementación de la búsqueda en profundidad.
// Entran los puntos origen y destino y devuelve la
// secuencia de acciones en plan, una lista de acciones.
bool ComportamientoJugador::pathFinding_Profundidad(const estado &origen, const estado &destino, list<Action> &plan) {
	//Borro la lista
	cout << "Calculando plan\n";
	plan.clear();
	set<estado,ComparaEstados> generados; // Lista de Cerrados
	stack<nodo> pila;											// Lista de Abiertos

  	nodo current;
	current.st = origen;
	current.secuencia.empty();

	pila.push(current);

  while (!pila.empty() and (current.st.fila!=destino.fila or current.st.columna != destino.columna)){

		pila.pop();
		generados.insert(current.st);

		// Generar descendiente de girar a la derecha
		nodo hijoTurnR = current;
		hijoTurnR.st.orientacion = (hijoTurnR.st.orientacion+1)%4;
		if (generados.find(hijoTurnR.st) == generados.end()){
			hijoTurnR.secuencia.push_back(actTURN_R);
			pila.push(hijoTurnR);

		}

		// Generar descendiente de girar a la izquierda
		nodo hijoTurnL = current;
		hijoTurnL.st.orientacion = (hijoTurnL.st.orientacion+3)%4;
		if (generados.find(hijoTurnL.st) == generados.end()){
			hijoTurnL.secuencia.push_back(actTURN_L);
			pila.push(hijoTurnL);
		}

		// Generar descendiente de avanzar
		nodo hijoForward = current;
		if (!HayObstaculoDelante(hijoForward.st)){
			if (generados.find(hijoForward.st) == generados.end()){
				hijoForward.secuencia.push_back(actFORWARD);
				pila.push(hijoForward);
			}
		}

		// Tomo el siguiente valor de la pila
		if (!pila.empty()){
			current = pila.top();
		}
	}

  cout << "Terminada la busqueda\n";

	if (current.st.fila == destino.fila and current.st.columna == destino.columna){
		cout << "Cargando el plan\n";
		plan = current.secuencia;
		cout << "Longitud del plan: " << plan.size() << endl;
		PintaPlan(plan);
		// ver el plan en el mapa
		VisualizaPlan(origen, plan);
		return true;
	}
	else {
		cout << "No encontrado plan\n";
	}

	return false;
}

bool ComportamientoJugador::pathFinding_Anchura(const estado &origen, const estado &destino, list<Action> &plan){
	cout << "Calculando plan\n";
	plan.clear();
	set<estado,ComparaEstados> generados;
	queue<nodo> cola;

	nodo current;
	current.st = origen;
	current.secuencia.empty();
	
	cola.push(current);

	while(!cola.empty() && (current.st.fila != destino.fila || current.st.columna != destino.columna)){
		cola.pop();
		generados.insert(current.st);

		nodo hijoTurnR = current;
		hijoTurnR.st.orientacion = (hijoTurnR.st.orientacion+1)%4;
		if (generados.find(hijoTurnR.st) == generados.end()){
			hijoTurnR.secuencia.push_back(actTURN_R);
			cola.push(hijoTurnR);
		}

		nodo hijoTurnL = current;
		hijoTurnL.st.orientacion = (hijoTurnL.st.orientacion+3)%4;
		if (generados.find(hijoTurnL.st) == generados.end()){
			hijoTurnL.secuencia.push_back(actTURN_L);
			cola.push(hijoTurnL);
		}

		nodo hijoForward = current;
		if (!HayObstaculoDelante(hijoForward.st)){
			if (generados.find(hijoForward.st) == generados.end()){
				hijoForward.secuencia.push_back(actFORWARD);
				cola.push(hijoForward);
			}
		}

		if (!cola.empty()){
			current = cola.front();
		}
	}
	
	cout << "Terminada la busqueda\n";

		if (current.st.fila == destino.fila and current.st.columna == destino.columna){
			cout << "Cargando el plan\n";
			plan = current.secuencia;
			cout << "Longitud del plan: " << plan.size() << endl;
			PintaPlan(plan);
			VisualizaPlan(origen, plan);
			return true;
		}
		else {
			cout << "No encontrado plan\n";
		}

		return false;
}

int calcularcosto(int a,int b,int c,int d,int e){
	return e*(abs(a-b)+abs(c-d));
}

int ComportamientoJugador::costeCasilla(char a){
	int ret = 1;
	switch (a){
		case 'A': 
			if (zapatillas) 
				ret = 10; 
			else 
				ret = 100;
			break;
		case 'B': 
			if (bikini)
				ret = 5; 
			else
				ret = 50;
			break;
		case 'T': ret = 2; break;
		case '?': ret = 1; break;
 	}
	 return ret;
}

bool ComportamientoJugador::pathFinding_CostoUniforme(const estado &origen, const estado &destino, list<Action> &plan){
	cout << "Calculando plan\n";
	plan.clear();
	set<estado,ComparaEstados> generados;

	nodoP current;
	current.st = origen;
	current.secuencia.empty();
	current.costo = 0;
	int cost;

	list<nodoP> abiertos;
	abiertos.push_front(current);
	list<nodoP>::iterator it,comp=abiertos.begin();
	generados.insert(current.st);

	while(!abiertos.empty() and (current.st.fila!=destino.fila or current.st.columna != destino.columna)){
	
		nodoP hijoTurnR = current;
		hijoTurnR.st.orientacion = (hijoTurnR.st.orientacion+1)%4;
		if (generados.find(hijoTurnR.st) == generados.end()){
			hijoTurnR.secuencia.push_back(actTURN_R);
			hijoTurnR.costo += costeCasilla(mapaResultado[hijoTurnR.st.fila][hijoTurnR.st.columna]);
			abiertos.push_front(hijoTurnR);
		}

		nodoP hijoTurnL = current;
		hijoTurnL.st.orientacion = (hijoTurnL.st.orientacion+3)%4;
		if (generados.find(hijoTurnL.st) == generados.end()){
			hijoTurnL.secuencia.push_back(actTURN_L);
			hijoTurnL.costo += costeCasilla(mapaResultado[hijoTurnL.st.fila][hijoTurnL.st.columna]);
			abiertos.push_front(hijoTurnL);
		}

		nodoP hijoForward = current;
		if (!HayObstaculoDelante(hijoForward.st)){
			if (generados.find(hijoForward.st) == generados.end()){
				hijoForward.secuencia.push_back(actFORWARD);
				hijoForward.costo += costeCasilla(mapaResultado[hijoForward.st.fila][hijoForward.st.columna]);
				abiertos.push_front(hijoForward);
			}
		}

		int temp;
		if (!abiertos.empty()){
			cost = (*comp).costo;
			comp = abiertos.begin();
			for (it=abiertos.begin();it!=abiertos.end();it++){
				temp=(*it).costo;
				if(temp<cost){
					comp = it;
					cost = temp;
				}
			}
		}
		current = *comp;
		abiertos.erase(comp);
		generados.insert(current.st);
	}

	cout << "Terminada la busqueda\n";

		if (current.st.fila == destino.fila and current.st.columna == destino.columna){
			cout << "Cargando el plan\n";
			plan = current.secuencia;
			cout << "Longitud del plan: " << plan.size() << endl;
			PintaPlan(plan);
			VisualizaPlan(origen, plan);
			return true;
		}
		else {
			cout << "No encontrado plan\n";
		}
		return false;
}

bool ComportamientoJugador::pathFinding_CostoUniforme_Nivel2(const estado &origen, const estado &destino, list<Action> &plan){
	cout << "Calculando plan\n";
	plan.clear();
	set<estado,ComparaEstados> generados;

	nodoP current;
	current.st = origen;
	current.secuencia.empty();
	current.costo = 0;
	int cost;

	list<nodoP> abiertos;
	abiertos.push_front(current);
	list<nodoP>::iterator it,comp=abiertos.begin();
	generados.insert(current.st);

	while(!abiertos.empty() and (current.st.fila!=destino.fila or current.st.columna != destino.columna)){
		nodoP hijoTurnR = current;
		hijoTurnR.st.orientacion = (hijoTurnR.st.orientacion+1)%4;
		if (generados.find(hijoTurnR.st) == generados.end()){
			hijoTurnR.secuencia.push_back(actTURN_R);
			hijoTurnR.costo += costeCasilla(mapaResultado[hijoTurnR.st.fila][hijoTurnR.st.columna]);
			abiertos.push_front(hijoTurnR);
		}

		nodoP hijoTurnL = current;
		hijoTurnL.st.orientacion = (hijoTurnL.st.orientacion+3)%4;
		if (generados.find(hijoTurnL.st) == generados.end()){
			hijoTurnL.secuencia.push_back(actTURN_L);
			hijoTurnL.costo += costeCasilla(mapaResultado[hijoTurnL.st.fila][hijoTurnL.st.columna]);
			abiertos.push_front(hijoTurnL);
		}

		nodoP hijoForward = current;
		if (!HayObstaculoDelante(hijoForward.st)){
			if (generados.find(hijoForward.st) == generados.end()){
				hijoForward.secuencia.push_back(actFORWARD);
				hijoForward.costo += costeCasilla(mapaResultado[hijoForward.st.fila][hijoForward.st.columna]);
				abiertos.push_front(hijoForward);
			}
		}

		int temp;
		cost = calcularcosto(current.st.fila,destino.fila,current.st.columna,destino.columna,current.costo);
		comp = abiertos.begin();
		for (it=abiertos.begin();it!=abiertos.end();it++){
			temp=calcularcosto((*it).st.fila,destino.fila,(*it).st.columna,destino.columna,(*it).costo);
			if(temp<cost){
				comp = it;
				cost = temp;
			}
		}
		current = *comp;
		abiertos.erase(comp);
		generados.insert(current.st);
	}

	cout << "Terminada la busqueda\n";

		if (current.st.fila == destino.fila and current.st.columna == destino.columna){
			cout << "Cargando el plan\n";
			plan = current.secuencia;
			cout << "Longitud del plan: " << plan.size() << endl;
			PintaPlan(plan);
			VisualizaPlan(origen, plan);
			return true;
		}
		else {
			cout << "No encontrado plan\n";
		}
		return false;
}

bool ComportamientoJugador::pathFinding_bateria_Nivel2(const estado &origen, list<Action> &plan){
	cout << "Calculando plan\n";
	plan.clear();
	set<estado,ComparaEstados> generados;

	nodoP current;
	current.st = origen;
	current.secuencia.empty();
	current.costo = 0;
	int cost;

	list<nodoP> abiertos;
	abiertos.push_front(current);
	list<nodoP>::iterator it,comp=abiertos.begin();
	generados.insert(current.st);

	while(!abiertos.empty() and (current.st.fila!=posObj.first or current.st.columna != posObj.second)){
		

		nodoP hijoTurnR = current;
		hijoTurnR.st.orientacion = (hijoTurnR.st.orientacion+1)%4;
		if (generados.find(hijoTurnR.st) == generados.end()){
			hijoTurnR.secuencia.push_back(actTURN_R);
			hijoTurnR.costo += costeCasilla(mapaResultado[hijoTurnR.st.fila][hijoTurnR.st.columna]);
			abiertos.push_front(hijoTurnR);
		}

		nodoP hijoTurnL = current;
		hijoTurnL.st.orientacion = (hijoTurnL.st.orientacion+3)%4;
		if (generados.find(hijoTurnL.st) == generados.end()){
			hijoTurnL.secuencia.push_back(actTURN_L);
			hijoTurnL.costo += costeCasilla(mapaResultado[hijoTurnL.st.fila][hijoTurnL.st.columna]);
			abiertos.push_front(hijoTurnL);
		}

		nodoP hijoForward = current;
		if (!HayObstaculoDelante(hijoForward.st)){
			if (generados.find(hijoForward.st) == generados.end()){
				hijoForward.secuencia.push_back(actFORWARD);
				hijoForward.costo += costeCasilla(mapaResultado[hijoForward.st.fila][hijoForward.st.columna]);
				abiertos.push_front(hijoForward);
			}
		}
		int temp;
		cost = calcularcosto(current.st.fila,destino.fila,current.st.columna,destino.columna,current.costo);
		comp = abiertos.begin();
		for (it=abiertos.begin();it!=abiertos.end();it++){
			temp=calcularcosto((*it).st.fila,destino.fila,(*it).st.columna,destino.columna,(*it).costo);
			if(temp<cost){
				comp = it;
				cost = temp;
			}
		}
		current = *comp;
		abiertos.erase(comp);
		generados.insert(current.st);
	}

	cout << "Terminada la busqueda\n";

		if (current.st.fila == posObj.first and current.st.columna == posObj.second){
			cout << "Cargando el plan\n";
			plan = current.secuencia;
			cout << "Longitud del plan: " << plan.size() << endl;
			PintaPlan(plan);
			VisualizaPlan(origen, plan);
			return true;
		}
		else {
			cout << "No encontrado plan\n";
		}
		return false;
}


// Sacar por la términal la secuencia del plan obtenido
void ComportamientoJugador::PintaPlan(list<Action> plan) {
	auto it = plan.begin();
	while (it!=plan.end()){
		if (*it == actFORWARD){
			cout << "A ";
		}
		else if (*it == actTURN_R){
			cout << "D ";
		}
		else if (*it == actTURN_L){
			cout << "I ";
		}
		else {
			cout << "- ";
		}
		it++;
	}
	cout << endl;
}



void AnularMatriz(vector<vector<unsigned char> > &m){
	for (int i=0; i<m[0].size(); i++){
		for (int j=0; j<m.size(); j++){
			m[i][j]=0;
		}
	}
}


// Pinta sobre el mapa del juego el plan obtenido
void ComportamientoJugador::VisualizaPlan(const estado &st, const list<Action> &plan){
  AnularMatriz(mapaConPlan);
	estado cst = st;

	auto it = plan.begin();
	while (it!=plan.end()){
		if (*it == actFORWARD){
			switch (cst.orientacion) {
				case 0: cst.fila--; break;
				case 1: cst.columna++; break;
				case 2: cst.fila++; break;
				case 3: cst.columna--; break;
			}
			mapaConPlan[cst.fila][cst.columna]=1;
		}
		else if (*it == actTURN_R){
			cst.orientacion = (cst.orientacion+1)%4;
		}
		else {
			cst.orientacion = (cst.orientacion+3)%4;
		}
		it++;
	}
}



int ComportamientoJugador::interact(Action accion, int valor){
  return false;
}
