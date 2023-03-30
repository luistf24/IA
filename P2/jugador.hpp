#ifndef COMPORTAMIENTOJUGADOR_H
#define COMPORTAMIENTOJUGADOR_H

#include "comportamientos/comportamiento.hpp"

#include <list>
#include <iostream>
#include <vector>
#include <set>

struct estado {
  int fila;
  int columna;
  int orientacion;
};

class ComportamientoJugador : public Comportamiento {
  public:
    ComportamientoJugador(unsigned int size) : Comportamiento(size), mapaNivel2(2*size+4, vector<unsigned char>(2*size+4, 'O')) {
      // Inicializar Variables de Estado
      fil = col = size -1;
      brujula = 0; // 0: Norte, 1:Este, 2:Sur, 3:Oeste
      destino.fila = -1;
      destino.columna = -1;
      destino.orientacion = -1;
      hayplan=false;
    }
    ComportamientoJugador(std::vector< std::vector< unsigned char> > mapaR) : Comportamiento(mapaR), mapaNivel2(2*mapaR.size()+4, vector<unsigned char>(2*mapaR.size()+4, 'O')) {
      // Inicializar Variables de Estado
      fil = col = mapaR.size()-1;
      brujula = 0; // 0: Norte, 1:Este, 2:Sur, 3:Oeste
      destino.fila = -1;
      destino.columna = -1;
      destino.orientacion = -1;
      hayplan=false;
    }
    ComportamientoJugador(const ComportamientoJugador & comport) : Comportamiento(comport){}
    ~ComportamientoJugador(){}

    Action think(Sensores sensores);
    int interact(Action accion, int valor);
    void VisualizaPlan(const estado &st, const list<Action> &plan);
    ComportamientoJugador * clone(){return new ComportamientoJugador(*this);}

  private:
    // Declarar Variables de Estado
    int fil, col, brujula;
    estado actual, destino,siguiente;
    list<Action> plan;
    bool hayplan=false;
    bool hayplanb=false;
    //reactivo
    vector<vector<unsigned char>> mapaNivel2;
    bool inicio = true;
    Action lastm = actIDLE;
    bool ubicado ;
    bool zapatillas = false;
    bool bikini = false;
    bool find = false;
    int recarga = 500;
    pair<int,int> posObj;
    int descanso = 0;

    // Métodos privados de la clase
    bool pathFinding(int level, const estado &origen, const estado &destino, list<Action> &plan);
    bool pathFinding_Profundidad(const estado &origen, const estado &destino, list<Action> &plan);
    bool pathFinding_Anchura(const estado &origen, const estado &destino, list<Action> &plan);
    bool pathFinding_CostoUniforme(const estado &origen, const estado &destino, list<Action> &plan);
    bool pathFinding_CostoUniforme_Nivel2(const estado &origen, const estado &destino, list<Action> &plan);
    bool pathFinding_bateria_Nivel2(const estado &origen,list<Action> &plan);
    bool HayObstaculoDelanteNivel2(estado &st,const vector<vector<unsigned char>> &mapa);

    

    void PintaPlan(list<Action> plan);
    bool HayObstaculoDelante(estado &st);

    bool testmap(int fil,int col){
      return (fil>0 or fil<=mapaResultado.size()) &&  (col>0 or col<=mapaResultado[0].size());
    }

    void RellenarMapa(Sensores sensores);
    void PintarMapa(Sensores sensores);
    int costeCasilla(char a);
    bool ObstaculoDelanteDelante(estado &st);
    void HayObjetivo(char a, int b, int c);

};

#endif
