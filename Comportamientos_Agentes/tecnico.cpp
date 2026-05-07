#include "tecnico.hpp"
#include "motorlib/util.h"
#include <iostream>
#include <queue>
#include <set>
#include <cstdlib>

using namespace std;

// =========================================================================
// ÁREA DE IMPLEMENTACIÓN DEL ESTUDIANTE
// =========================================================================

Action ComportamientoTecnico::think(Sensores sensores)
{
  Action accion = IDLE;

  // Decisión del agente según el nivel
  switch (sensores.nivel)
  {
  case 0:
    accion = ComportamientoTecnicoNivel_0(sensores);
    break;
  case 1:
    accion = ComportamientoTecnicoNivel_1(sensores);
    break;
  case 2:
    accion = ComportamientoTecnicoNivel_2(sensores);
    break;
  case 3:
    accion = ComportamientoTecnicoNivel_3(sensores);
    // accion = ComportamientoTecnicoNivel_E(sensores);

    break;
  case 4:
    accion = ComportamientoTecnicoNivel_4(sensores);
    break;
  case 5:
    accion = ComportamientoTecnicoNivel_5(sensores);
    break;
  case 6:
    accion = ComportamientoTecnicoNivel_6(sensores);
    break;
  }

  return accion;
}

//_________________________________________________________
// Nivel 0
//_________________________________________________________

char ViablePorAlturaT(char casilla, int dif) // No la uso hay opciones mejores pero ahi esta
{
  if (abs(dif) <= 1)
    return casilla;
  else
    return 'P';
}

// Guardamoa el instante actual en la posición donde estamos.
void ComportamientoTecnico::RegistrarVisitaT(ubicacion actual)
{
  mapaUltimoPaso[actual.f][actual.c] = instanteActual;
}

bool ComportamientoTecnico::EsCaminoLimpioT(ubicacion destino, int idx_sensor, const Sensores &sensores)
{
  char terreno = sensores.superficie[idx_sensor];
  bool es_pisable = (terreno == 'C' || terreno == 'D' || terreno == 'U');
  if (tiene_zapatillas && terreno == 'B') // Es una particularidad del tecnico el puede ir por el bosque si tiene zapas como si fuese un camino es muy Tecnico
    es_pisable = true;

  if (!es_pisable)
    return false;
  if (sensores.agentes[idx_sensor] != '_')
    return false; // Con esto marcamos que la casilla esta desocupada _ por tanto miramos si no esta desocupada

  return true;
}

bool ComportamientoTecnico::SePuedeCaminarT(ubicacion origen, int idx_sensor, const Sensores &sensores)
{
  ubicacion destino = Delante(origen);
  if (!EsCaminoLimpioT(destino, idx_sensor, sensores))
    return false;

  int desnivel = abs(mapaCotas[destino.f][destino.c] - mapaCotas[origen.f][origen.c]);
  return (desnivel <= 1);
}

// Evalúa la casilla basándose en cuántos instantes hace que no pasamos.
int ComportamientoTecnico::EvaluarLadoT(ubicacion origen, int idx_sensor, const Sensores &sensores)
{
  if (!SePuedeCaminarT(origen, idx_sensor, sensores))
  {
    return -999999; // Si no es transitable nonono
  }

  ubicacion destino = Delante(origen);
  char terreno = sensores.superficie[idx_sensor];

  int puntos = 100;

  // La prioridad de esta mision es ir a los sitios de residuos U son muy importantes por eso le damos gran valor
  if (terreno == 'U')
    puntos = 10000;
  else if (terreno == 'D' && !tiene_zapatillas) // Las zapas para el Tecnico son muy muy importantes que pueda ir por el bosque esta rotisimo
    puntos = 5000;

  // Lo mismo q con el ingeniero  //De esta forma gracias a los instantes una casilla que he pisado al principio valdra 10 y se me hace ams suculenta que una que acabo de pisar y alomejor vale 500
  int ultimo_instante = mapaUltimoPaso[destino.f][destino.c];
  puntos -= ultimo_instante;

  return puntos;
}

Action ComportamientoTecnico::ComportamientoTecnicoNivel_0(Sensores sensores)
{
  // Aumentamos los instantes
  instanteActual++;

  ActualizarMapa(sensores);

  // Por si acaso ya que deberia de inicializarse bien pero por si acaso debido a q lo he probado al hacer el proyecto de nuevo prefiero que sea redundante a que de fallo
  if (nVisitas.size() != mapaResultado.size() || nVisitas.empty())
  {
    nVisitas.assign(mapaResultado.size(), vector<int>(mapaResultado[0].size(), 0));
  }
  if (mapaUltimoPaso.size() != mapaResultado.size() || mapaUltimoPaso.empty())
  {
    mapaUltimoPaso.assign(mapaResultado.size(), vector<int>(mapaResultado[0].size(), 0));
  }

  ubicacion posicion_actual = {sensores.posF, sensores.posC, sensores.rumbo}; // guardo mi posicion actual rumbo me da la orientacion
  RegistrarVisitaT(posicion_actual);

  if (sensores.superficie[0] == 'D')
    tiene_zapatillas = true;

  if (sensores.superficie[1] == 'U' && SePuedeCaminarT(posicion_actual, 1, sensores))
    return TURN_SL;

  if (sensores.superficie[2] == 'U' && SePuedeCaminarT(posicion_actual, 2, sensores))
    return WALK;

  // if (sensores.superficie[3] == 'U')
  // return TURN_SR;

  if (sensores.superficie[0] == 'U') // En cuanto lo encuentro y estoy en el ya he terminado
    return IDLE;

  // Esto es cine lo que hago es simular los movimientos sin hacerlo para ver que me sale mas rentable
  ubicacion mirar_frente = posicion_actual;
  ubicacion mirar_izq = posicion_actual;
  mirar_izq.brujula = (Orientacion)((posicion_actual.brujula + 7) % 8);
  ubicacion mirar_der = posicion_actual;
  mirar_der.brujula = (Orientacion)((posicion_actual.brujula + 1) % 8);

  // Puntuaciones de los lados
  int nota_izq = EvaluarLadoT(mirar_izq, 1, sensores);
  int nota_frente = EvaluarLadoT(mirar_frente, 2, sensores);
  int nota_der = EvaluarLadoT(mirar_der, 3, sensores);

  // Encontrar al ganador
  int ganador = nota_frente;
  if (nota_izq > ganador)
    ganador = nota_izq;
  if (nota_der > ganador)
    ganador = nota_der;

  Action accion_elegida = IDLE;

  if (ganador <= -999999)
  {
    if (rand() % 2 == 0)
    {
      return TURN_SL;
    }
    else
    {
      return TURN_SR;
    }
  }

  if (ganador == nota_frente)
  {
    return WALK;
  }
  else if (ganador == nota_izq)
  {
    return TURN_SL;
  }

  return TURN_SR;
}

/**
 * @brief Comprueba si una celda es de tipo camino transitable.
 * @param c Carácter que representa el tipo de superficie.
 * @return true si es camino ('C'), zapatillas ('D') o meta ('U').
 */
bool ComportamientoTecnico::es_camino(unsigned char c) const
{
  return (c == 'C' || c == 'D' || c == 'U');
}

//_________________________________________________________
// Nivel 1
//_________________________________________________________

bool ComportamientoTecnico::es_transitable_nivel_1_T(unsigned char c) const
{

  if (n6T)
  {
    if ((c == 'C' || c == 'S' || c == 'D' || c == 'U' || c == 'H'))
      return true;
  }

  if (c == 'C' || c == 'S' || c == 'D' || c == 'U')
    return true;

  // Esto esta rotisimo si tiene zapas puede pisar el bosque como si fuese un camino
  if (tiene_zapatillas && c == 'B')
    return true;

  return false;
}

bool ComportamientoTecnico::EsCaminoLimpioN1_T(ubicacion destino, int idx_sensor, const Sensores &sensores)
{
  // Evitar salir del mapa
  if (destino.f < 0 || destino.f >= mapaResultado.size() || destino.c < 0 || destino.c >= mapaResultado[0].size())
    return false;

  char terreno = mapaResultado[destino.f][destino.c];
  if (!es_transitable_nivel_1_T(terreno))
    return false;

  if (sensores.agentes[idx_sensor] != '_') // Recordar _ te dice q esta desocupada
    return false;

  return true;
}

bool ComportamientoTecnico::SePuedeCaminarN1_T(ubicacion origen, int idx_sensor, const Sensores &sensores)
{
  ubicacion destino = Delante(origen);

  if (!EsCaminoLimpioN1_T(destino, idx_sensor, sensores))
    return false;

  int desnivel = abs(mapaCotas[destino.f][destino.c] - mapaCotas[origen.f][origen.c]);
  return (desnivel <= 1);
}

int ComportamientoTecnico::ContarDesconocidosT(const ubicacion &centro)
{
  // Busco la ? mas cercana a mi usando distancia Manhattan
  int min_dist = 999999;
  for (int f = 0; f < mapaResultado.size(); f++)
  {
    for (int c = 0; c < mapaResultado[0].size(); c++)
    {
      if (mapaResultado[f][c] == '?')
      {
        int dist = abs(f - centro.f) + abs(c - centro.c);
        if (dist < min_dist)
        {
          min_dist = dist;
          if (min_dist == 1)
            return 1; // Para que no tarde demasiado en cuanto encontramos 1 casilla desconocida a una distacia de 1 q es lo minimo terminamos
        }
      }
    }
  }
  return min_dist;
}

int ComportamientoTecnico::EvaluarLadoN1_T(ubicacion origen, int idx_sensor, const Sensores &sensores)
{

  if (!SePuedeCaminarN1_T(origen, idx_sensor, sensores))
    return -999999;

  ubicacion destino = Delante(origen);
  int puntos = 100000;

  // Evaluamos tambien como seria el siguiente paso para evitar perder tiempo por un sitio que no vamos a poder continuar
  ubicacion paso2 = Delante(destino);

  if (paso2.f >= 0 && paso2.f < mapaResultado.size() && paso2.c >= 0 && paso2.c < mapaResultado[0].size())
  {
    char t2 = mapaResultado[paso2.f][paso2.c];

    if (t2 == 'M' || t2 == 'P' || t2 == 'A')
      puntos -= 80000;
    if (t2 == 'B' && !tiene_zapatillas)
      puntos -= 80000;

    int desnivel2 = abs(mapaCotas[paso2.f][paso2.c] - mapaCotas[destino.f][destino.c]);
    if (desnivel2 > 1)
      puntos -= 80000;
  }

  int visitas = nVisitas[destino.f][destino.c];
  puntos -= (visitas * 5000);

  // RADAR MANHATTAN  ME LLEVA ANTE LO DESCONOCIDO haciendo que elija la que tiene menor distancia con lo que no conozco
  int dist_niebla = ContarDesconocidosT(destino);
  puntos -= (dist_niebla * 500);

  return puntos;
}

Action ComportamientoTecnico::ComportamientoTecnicoNivel_1(Sensores sensores)
{
  instanteActual++;
  ActualizarMapa(sensores);

  // Por si acaso ya que deberia de inicializarse bien pero por si acaso debido a q lo he probado al hacer el proyecto de nuevo prefiero que sea redundante a que de fallo
  if (nVisitas.size() != mapaResultado.size() || nVisitas.empty())
  {
    nVisitas.assign(mapaResultado.size(), vector<int>(mapaResultado[0].size(), 0));
  }
  if (mapaUltimoPaso.size() != mapaResultado.size() || mapaUltimoPaso.empty())
  {
    mapaUltimoPaso.assign(mapaResultado.size(), vector<int>(mapaResultado[0].size(), 0));
  }

  ubicacion yo = {sensores.posF, sensores.posC, sensores.rumbo};

  // Reutilizo la funcion que he hecho para el nivel 0
  RegistrarVisitaT(yo);

  // Aumentamos en la casilla donde estamos para evitar pasar varias veces por el mismo sitio
  nVisitas[yo.f][yo.c]++;

  if (sensores.superficie[0] == 'D')
    tiene_zapatillas = true;

  ubicacion mirar_frente = yo;
  ubicacion mirar_izq = yo;
  mirar_izq.brujula = (Orientacion)((yo.brujula + 7) % 8);
  ubicacion mirar_der = yo;
  mirar_der.brujula = (Orientacion)((yo.brujula + 1) % 8);

  // Miramos que la es el q mas nos interesa
  int nota_izq = EvaluarLadoN1_T(mirar_izq, 1, sensores);
  int nota_frente = EvaluarLadoN1_T(mirar_frente, 2, sensores);
  int nota_der = EvaluarLadoN1_T(mirar_der, 3, sensores);

  int ganador = nota_frente;
  if (nota_izq > ganador)
    ganador = nota_izq;
  if (nota_der > ganador)
    ganador = nota_der;

  Action accion_elegida = IDLE;

  if (ganador <= -999999)
  {
    accion_elegida = TURN_SL; // Movimiento por defecto
  }
  else if (ganador == nota_frente)
  {
    accion_elegida = WALK;
  }
  else if (ganador == nota_izq)
  {
    accion_elegida = TURN_SL;
  }
  else
  {
    accion_elegida = TURN_SR;
  }

  last_action = accion_elegida;
  return accion_elegida;
}

/**
 * @brief Comportamiento del técnico para el Nivel INVENTADO
 * @param sensores Datos actuales de los sensores.
 * @return Acción a realizar.
 */

list<Action> AvanzaASaltosDeCaballo()
{
  list<Action> secuencia;
  secuencia.push_back(WALK);
  secuencia.push_back(WALK);
  secuencia.push_back(TURN_SR);
  secuencia.push_back(TURN_SR);
  secuencia.push_back(WALK);

  return secuencia;
}

EstadoT NextCasillaTécnico(const EstadoT &st)
{
  EstadoT siguiente = st;
  switch (st.site.brujula)
  {
  case norte:
    siguiente.site.f = st.site.f - 1;
    break;
  case noreste:
    siguiente.site.f = st.site.f - 1;
    siguiente.site.c = st.site.c + 1;
    break;
  case este:
    siguiente.site.c = st.site.c + 1;
    break;
  case sureste:
    siguiente.site.f = st.site.f + 1;
    siguiente.site.c = st.site.c + 1;
    break;
  case sur:
    siguiente.site.f = st.site.f + 1;
    break;
  case suroeste:
    siguiente.site.f = st.site.f + 1;
    siguiente.site.c = st.site.c - 1;
    break;
  case oeste:
    siguiente.site.c = st.site.c - 1;
    break;
  case noroeste:
    siguiente.site.f = st.site.f - 1;
    siguiente.site.c = st.site.c - 1;
  }
  return siguiente;
}

bool CasillaAccesibleTécnico(const EstadoT &st, const vector<vector<unsigned char>> &terreno, const vector<vector<unsigned char>> &altura)
{
  EstadoT next = NextCasillaTécnico(st);

  if (next.site.f < 0 || next.site.f >= terreno.size() ||
      next.site.c < 0 || next.site.c >= terreno[0].size())
  {
    return false;
  }

  bool check1 = false, check2 = false, check3 = false;
  check1 = terreno[next.site.f][next.site.c] != 'P' and terreno[next.site.f][next.site.c] != 'M';
  check2 = terreno[next.site.f][next.site.c] != 'B' or (terreno[next.site.f][next.site.c] == 'B' and st.zapatillas);
  check3 = abs(altura[next.site.f][next.site.c] - altura[st.site.f][st.site.c]) <= 1;

  if (terreno[st.site.f][st.site.c] == '?' || terreno[next.site.f][next.site.c] == '?')
  { // Solo me afecta en el nivel 6 La niebla esta a una altura diferente por eso no me iba el camino y no lo buscaba ERA POR ESA RAZON IBA A PERDER LA CABEZA ERA ESO
    check3 = true;
  }

  return check1 and check2 and check3;
}

EstadoT applyT(Action accion, const EstadoT &st, const vector<vector<unsigned char>> &terreno, const vector<vector<unsigned char>> &altura)
{
  EstadoT next = st;
  switch (accion)
  {
  case WALK:
    if (CasillaAccesibleTécnico(st, terreno, altura))
    {
      next = NextCasillaTécnico(st);
    }
    break;
  case TURN_SR:
    next.site.brujula = (Orientacion)((next.site.brujula + 1) % 8);
    break;
  case TURN_SL:
    next.site.brujula = (Orientacion)((next.site.brujula + 7) % 8);
    break;
  }
  return next;
}

bool Find(const NodoT &st, const list<NodoT> &lista)
{
  auto it = lista.begin();
  while (it != lista.end() and !((*it) == st))
  {
    it++;
  }
  return (it != lista.end());
}

list<Action> ComportamientoTecnico::B_Anchura(const EstadoT &inicio, const EstadoT &final, const vector<vector<unsigned char>> &terreno, const vector<vector<unsigned char>> &altura)
{

  NodoT current_node;
  list<NodoT> frontier;
  list<NodoT> explored;
  list<Action> path;

  current_node.estado = inicio;
  frontier.push_back(current_node);
  bool SolutionFound = (current_node.estado.site.f == final.site.f and current_node.estado.site.c == final.site.c);

  while (!SolutionFound and !frontier.empty())
  {
    frontier.pop_front();
    explored.push_back(current_node);

    // Compruebo si estoy en una casilla que da las zapatillas
    if (terreno[current_node.estado.site.f][current_node.estado.site.c] == 'D')
    {
      current_node.estado.zapatillas = true;
    }

    // Genero el hijo resultante de aplicar la accion WALK
    NodoT child_Walk = current_node;
    child_Walk.estado = applyT(WALK, current_node.estado, terreno, altura);
    if (child_Walk.estado.site.f == final.site.f and child_Walk.estado.site.c == final.site.c)
    {
      // El hijo generado es solucion
      child_Walk.secuencia.push_back(WALK);
      current_node = child_Walk;
      SolutionFound = true;
    }
    else if (!Find(child_Walk, frontier) and !Find(child_Walk, explored))
    {
      // Se mete en la lista de frontier después de añadir a secuencia la acción
      child_Walk.secuencia.push_back(WALK);
      frontier.push_back(child_Walk);
    }

    if (!SolutionFound)
    {
      // El hijo resultante de aplicar la accion TURN_SR
      NodoT child_TurnSR = current_node;
      child_TurnSR.estado = applyT(TURN_SR, current_node.estado, terreno, altura);
      if (!Find(child_TurnSR, frontier) and !Find(child_TurnSR, explored))
      {
        child_TurnSR.secuencia.push_back(TURN_SR);
        frontier.push_back(child_TurnSR);
      }

      // El hijo resultante de aplicar la accion TURN_SL
      NodoT child_TurnSL = current_node;
      child_TurnSL.estado = applyT(TURN_SL, current_node.estado, terreno, altura);
      if (!Find(child_TurnSL, frontier) and !Find(child_TurnSL, explored))
      {
        child_TurnSL.secuencia.push_back(TURN_SL);
        frontier.push_back(child_TurnSL);
      }
    }

    // Paso a evaluar el siguiente nodo en la lista "frontier"
    if (!SolutionFound and !frontier.empty())
    {
      current_node = frontier.front();
    }
  }

  // Devuelvo el camino encontrado.
  if (SolutionFound)
  {
    path = current_node.secuencia;
  }

  return path;
}

list<Action> ComportamientoTecnico::B_Anchura_V2(const EstadoT &inicio, const EstadoT &final, const vector<vector<unsigned char>> &terreno, const vector<vector<unsigned char>> &altura)
{
  NodoT current_node;
  list<NodoT> frontier;
  set<NodoT> explored;
  list<Action> path;

  current_node.estado = inicio;
  frontier.push_back(current_node);
  bool SolutionFound = (current_node.estado.site.f == final.site.f and current_node.estado.site.c == final.site.c);

  while (!SolutionFound and !frontier.empty())
  {
    frontier.pop_front();
    explored.insert(current_node);

    // Compruebo si estoy en una casilla que da las zapatillas
    if (terreno[current_node.estado.site.f][current_node.estado.site.c] == 'D')
    {
      current_node.estado.zapatillas = true;
    }

    // Genero el hijo resultante de aplicar la accion WALK
    NodoT child_Walk = current_node;
    child_Walk.estado = applyT(WALK, current_node.estado, terreno, altura);
    if (child_Walk.estado.site.f == final.site.f and child_Walk.estado.site.c == final.site.c)
    {
      // El hijo generado es solucion
      child_Walk.secuencia.push_back(WALK);
      current_node = child_Walk;
      SolutionFound = true;
    }
    else if (explored.find(child_Walk) == explored.end())
    {
      // Se mete en la lista de frontier después de añadir a secuencia la acción
      child_Walk.secuencia.push_back(WALK);
      frontier.push_back(child_Walk);
    }

    if (!SolutionFound)
    {
      // El hijo resultante de aplicar la accion TURN_SR
      NodoT child_TurnSR = current_node;
      child_TurnSR.estado = applyT(TURN_SR, current_node.estado, terreno, altura);
      if (explored.find(child_TurnSR) == explored.end())
      {
        child_TurnSR.secuencia.push_back(TURN_SR);
        frontier.push_back(child_TurnSR);
      }

      // El hijo resultante de aplicar la accion TURN_SL
      NodoT child_TurnSL = current_node;
      child_TurnSL.estado = applyT(TURN_SL, current_node.estado, terreno, altura);
      if (explored.find(child_TurnSL) == explored.end())
      {
        child_TurnSL.secuencia.push_back(TURN_SL);
        frontier.push_back(child_TurnSL);
      }
    }

    // Paso a evaluar el siguiente nodo en la lista "frontier"
    if (!SolutionFound and !frontier.empty())
    {
      current_node = frontier.front();
      while (explored.find(current_node) != explored.end() and !frontier.empty())
      {
        frontier.pop_front();
        if (!frontier.empty())
        {
          current_node = frontier.front();
        }
      }
    }
  }

  // Devuelvo el camino encontrado.
  if (SolutionFound)
  {
    path = current_node.secuencia;
  }

  return path;
}

Action ComportamientoTecnico::ComportamientoTecnicoNivel_E(Sensores sensores)
{
  Action accion = IDLE;
  if (!hayPlan)
  {
    EstadoT inicio, fin;
    inicio.site.f = sensores.posF;
    inicio.site.c = sensores.posC;
    inicio.site.brujula = sensores.rumbo;
    inicio.zapatillas = tiene_zapatillas;
    fin.site.f = sensores.BelPosF;
    fin.site.c = sensores.BelPosC;

    plan = B_Anchura_V2(inicio, fin, mapaResultado, mapaCotas);

    VisualizaPlan(inicio.site, plan);
    hayPlan = plan.size() != 0;
  }
  if (hayPlan and plan.size() > 0)
  {
    accion = plan.front();
    plan.pop_front();
  }
  if (plan.size() == 0)
  {
    hayPlan = false;
  }
  return accion;
}

/**
 * @brief Comportamiento del técnico para el Nivel 2.
 * @param sensores Datos actuales de los sensores.
 * @return Acción a realizar.
 */
//_________________________________________________________
// Nivel 2
//_________________________________________________________

Action ComportamientoTecnico::ComportamientoTecnicoNivel_2(Sensores sensores)
{
  int cota_actual = sensores.cota[0];
  int cota_frente = sensores.cota[2];
  int diff_altura = abs(cota_frente - cota_actual);

  // Comprobamos que el terreno sea seguro
  bool terreno_seguro = (sensores.superficie[2] != 'M' && sensores.superficie[2] != 'P' && sensores.superficie[2] != 'B');

  if (terreno_seguro && diff_altura <= 1)
  {
    // Si nos chocamos giramos para buscar otra ruta
    if (sensores.choque)
    {
      return TURN_SR;
    }
    else
    {
      return WALK; // Es seguro avanzar
    }
  }
  else
  {
    // Si hay muro, precipicio o mucho desnivel nos giramos
    return TURN_SR;
  }
}

//_________________________________________________________
// Nivel 3
//_________________________________________________________

int costoActionT(Action accion, const EstadoT &st, const vector<vector<unsigned char>> &terreno, const vector<vector<unsigned char>> &altura)
{
  char t = terreno[st.site.f][st.site.c];
  int costo = 0;

  if (accion == WALK)
  { // Costos por walk
    bool is_rest = false;
    if (t == 'A')
      costo = 60;
    else if (t == 'H')
      costo = 6;
    else if (t == 'S')
      costo = 3;
    else
    { // Resto de terrenos ('C', 'U', 'B' si tiene zapatos)
      costo = 1;
      is_rest = true;
    }

    // El incremento/decremento NO aplica al resto de casillas (es +0/-0)
    if (!is_rest)
    {
      EstadoT next_st = NextCasillaTécnico(st); // Miramos cual es la casilla a la que vamos a ir
      int alt_origen = altura[st.site.f][st.site.c];
      int alt_destino = altura[next_st.site.f][next_st.site.c];
      int diff = alt_destino - alt_origen;

      if (diff > 0)
        costo += 5; // Si sube es +5 y si baja -2
      else if (diff < 0)
        costo -= 2;
    }
  }
  else if (accion == TURN_SL || accion == TURN_SR)
  { // Costos por turn
    if (t == 'A')
      costo = 5;
    else if (t == 'H')
      costo = 2;
    else if (t == 'S')
      costo = 1;
    else
      costo = 1;
  }
  return costo;
}

int heuristicaAStarT(const EstadoT &actual, const EstadoT &final)
{
  // Distancia de Chebyshev:
  // Como el agente puede moverse en diagonal (8 direcciones),
  // avanzar 1 paso en diagonal avanza tanto en X como en Y a la vez.
  // Por eso, la distancia mínima garantizada a la meta es el valor máximo entre
  // la diferencia de filas y la diferencia de columnas.
  return max(abs(actual.site.f - final.site.f), abs(actual.site.c - final.site.c));
}

list<Action> ComportamientoTecnico::A_Star_Nivel3(const EstadoT &inicio, const EstadoT &final, const vector<vector<unsigned char>> &terreno, const vector<vector<unsigned char>> &altura)
{
  priority_queue<NodoAStarT> frontier; // Cola ordenada automáticamente por el coste (f = g + h)
  map<EstadoT, int> explored;

  NodoAStarT start_node;
  start_node.estado = inicio;
  start_node.g = 0;                               // Coste de energía acumulado desde el inicio
  start_node.h = heuristicaAStarT(inicio, final); // Estimacion hacia la meta

  frontier.push(start_node); // Mete el inicio en la cola
  explored[inicio] = 0;      // Registra que llegar al inicio cuesta 0

  while (!frontier.empty())
  {                                      // Seguimos hasta q nos quedemos sin caminos que explorar
    NodoAStarT current = frontier.top(); // Saca el nodo MÁS BARATO de la cola a diferencia del BFS q sacaba el mas antiguo
    frontier.pop();

    // Hemos llegado a una solucion
    if (current.estado.site.f == final.site.f && current.estado.site.c == final.site.c)
    {
      return current.secuencia;
    }

    // Si este estado ya lo visitamos antes por un camino MÁS BARATO, lo ignoramos.
    if (explored.count(current.estado) && explored[current.estado] < current.g)
    {
      continue;
    }

    // zapatillas
    bool zapas = current.estado.zapatillas;
    if (terreno[current.estado.site.f][current.estado.site.c] == 'D')
    {
      zapas = true;
    }

    Action actions[] = {WALK, TURN_SR, TURN_SL}; // Para cada una de las acciones
    for (Action act : actions)
    {
      EstadoT temp_st = current.estado;
      temp_st.zapatillas = zapas;

      if (act == WALK && !CasillaAccesibleTécnico(temp_st, terreno, altura))
      {
        continue; // Si queremos caminar y no se puede se ignora este hijo
      }

      // SIMULAMOS LA ACCION
      EstadoT child_st = applyT(act, temp_st, terreno, altura);    // Donde acabamos
      int move_cost = costoActionT(act, temp_st, terreno, altura); // Cuanta energia nos ha costado esa accion
      int new_g = current.g + move_cost;                           // Calculamos g que es la energia acumulada total

      // Si va a una casilla con zapas, actualizamos su estado para futuros turnos
      if (terreno[child_st.site.f][child_st.site.c] == 'D')
      {
        child_st.zapatillas = true;
      }

      // Vemos si es un nuevo estado o si este estado gasta MENOS ENERGIA
      if (!explored.count(child_st) || new_g < explored[child_st])
      {
        explored[child_st] = new_g;

        // Creamos el nuevo nodo hijo
        NodoAStarT child_node;
        child_node.estado = child_st;
        child_node.secuencia = current.secuencia;
        child_node.secuencia.push_back(act);
        child_node.g = new_g;
        child_node.h = heuristicaAStarT(child_st, final);

        frontier.push(child_node); // Metemos este nuevo nodo en la cola
      }
    }
  }

  return list<Action>(); // Si llegamos aki quiere decir q no hemos encontardo ninguna solucion y ademas se han acabado todos los posibles caminos
}

/**
 * @brief Comportamiento del técnico para el Nivel 3.
 * @param sensores Datos actuales de los sensores.
 * @return Acción a realizar.
 */
Action ComportamientoTecnico::ComportamientoTecnicoNivel_3(Sensores sensores)
{
  // Si estoy encima de las zapas
  if (sensores.superficie[0] == 'D')
  {
    tiene_zapatillas = true;
  }

  Action accion = IDLE;

  if (!hayPlan)
  {
    EstadoT inicio, fin;
    inicio.site.f = sensores.posF;
    inicio.site.c = sensores.posC;
    inicio.site.brujula = sensores.rumbo;
    inicio.zapatillas = tiene_zapatillas;
    fin.site.f = sensores.BelPosF;
    fin.site.c = sensores.BelPosC;

    plan = A_Star_Nivel3(inicio, fin, mapaResultado, mapaCotas);

    VisualizaPlan(inicio.site, plan);
    hayPlan = plan.size() > 0;
  }

  if (hayPlan && plan.size() > 0)
  {
    Action siguiente_accion_del_plan = plan.front(); // Miramos la siguiente accion que vamos a hacer sin borrarla

    // Si nos toca caminar y el Ingeniero está justo delante
    if (siguiente_accion_del_plan == WALK && sensores.agentes[2] == 'i')
    {
      // NOS QUEDAMOS QUIETOS ESPERANDO IDLE no gasta energia
      // Como no hemos sacado la acción del plan, lo intentaremos de nuevo en el próximo turno
      accion = IDLE;
    }
    else
    {
      // Si el camino está libre o es un giro. Ejecutamos y la sacamos del plan(Aki no tenemos problemas).
      accion = siguiente_accion_del_plan;
      plan.pop_front();
    }
  }

  return accion;
}

/**
 * @brief Comportamiento del técnico para el Nivel 4.
 * @param sensores Datos actuales de los sensores.
 * @return Acción a realizar.
 */

//_________________________________________________________
// Nivel 4
//_________________________________________________________

Action ComportamientoTecnico::ComportamientoTecnicoNivel_4(Sensores sensores)
{
  return IDLE;
}

/**
 * @brief Comportamiento del técnico para el Nivel 5.
 * @param sensores Datos actuales de los sensores.
 * @return Acción a realizar.
 */

//_________________________________________________________
// Nivel 5
//_________________________________________________________

Action ComportamientoTecnico::MoverA_Tecnico(int f, int c, const Sensores &sensores)
{

  if (sensores.choque)
    n5_plan_movimiento.clear();

  // Si ya estamos en el sitio, limpiamos el plan y nos quedamos quietos
  if (sensores.posF == f && sensores.posC == c)
  {
    n5_plan_movimiento.clear();
    return IDLE;
  }

  // Si el A* falla esperamos para no saturarnos
  if (n5_espera_path > 0)
  {
    n5_espera_path--;
    return IDLE;
  }

  // Si no tenemos plan lo preparamos
  if (n5_plan_movimiento.empty())
  {
    EstadoT inicio, fin;
    inicio.site.f = sensores.posF;
    inicio.site.c = sensores.posC;
    inicio.site.brujula = sensores.rumbo;
    inicio.zapatillas = tiene_zapatillas;
    fin.site.f = f;
    fin.site.c = c;

    vector<vector<unsigned char>> mapa_modificado = mapaResultado;

    // Si el ingeniero nos bloquea el paso lo marcamos como MURO ('M') solo si no es casilla de destino, que si no no llegamos nunca :(
    if (n5_obs_f != -1 && n5_obs_c != -1)
    {
      if (n5_obs_f != f || n5_obs_c != c)
      {
        mapa_modificado[n5_obs_f][n5_obs_c] = 'M';
      }
    }

    n5_plan_movimiento = A_Star_Nivel3(inicio, fin, mapa_modificado, mapaCotas);
    n5_obs_f = -1;
    n5_obs_c = -1;

    // Funcion que nos proporcionan para dibujar el camino en el mapa
    VisualizaPlan(inicio.site, n5_plan_movimiento);

    // El A* ha fallado entoces activamos nuestra espera
    if (n5_plan_movimiento.empty())
    {
      n5_espera_path = 5;
      return IDLE;
    }
  }

  if (!n5_plan_movimiento.empty())
  {
    Action a = n5_plan_movimiento.front();

    if (a == WALK && sensores.agentes[2] == 'i')
    {
      n5_espera++;
      // Si el ingeniero no se quita en 3 turnos lo marcamos como obstáculo y recalculamos ruta
      if (n5_espera > 3)
      {
        ubicacion enfrente = Delante({sensores.posF, sensores.posC, sensores.rumbo});
        n5_obs_f = enfrente.f;
        n5_obs_c = enfrente.c;
        n5_plan_movimiento.clear();
        n5_espera = 0;
      }
      return IDLE;
    }

    n5_espera = 0;
    n5_plan_movimiento.pop_front();
    return a;
  }
  return IDLE;
}

Action ComportamientoTecnico::ComportamientoTecnicoNivel_5(Sensores sensores)
{
  if (sensores.superficie[0] == 'D')
    tiene_zapatillas = true;

  switch (n5_estado)
  {

  case 0: // Esperamos a que nos llame el ingeniero
  {
    if (sensores.venpaca)
    {
      n5_target_f = sensores.GotoF;
      n5_target_c = sensores.GotoC;
      n5_plan_movimiento.clear();
      n5_estado = 1;
    }
    return IDLE;
  }

  case 1: // Vamos a donde nos han llamado
  {
    if (sensores.posF != n5_target_f || sensores.posC != n5_target_c)
    {
      return MoverA_Tecnico(n5_target_f, n5_target_c, sensores);
    }
    n5_estado = 2;
  }

  case 2: // Miramos al ingeniero para sincronizarnos e instalar la tuberia 
  {
    if (sensores.agentes[2] == 'i')
    {
      if (sensores.enfrente)
      {
        n5_estado = 0;
        return INSTALL;
      }
      return IDLE;
    }

    return TURN_SR;
  }
  }

  return IDLE;
}

/**
 * @brief Comportamiento del técnico para el Nivel 6.
 * @param sensores Datos actuales de los sensores.
 * @return Acción a realizar.
 */
//_________________________________________________________
// Nivel 6
//_________________________________________________________

Action ComportamientoTecnico::ComportamientoTecnicoNivel_6(Sensores sensores)
{

  ActualizarMapa(sensores); // Para que descubra el mapa mientras se mueve

  if (sensores.superficie[0] == 'D')
    tiene_zapatillas = true;

  // Primero de todo nos ponemos a explorar 
  if (n6_estado == 0)
  {
    if (sensores.venpaca)
    {
      n5_target_f = sensores.GotoF;
      n5_target_c = sensores.GotoC;
      n5_plan_movimiento.clear();
      n5_estado = 1;
      n6_estado = 1;
      return IDLE;
    }
    return ComportamientoTecnicoNivel_1(sensores);
  }

  // En cuanto nos llama el ingeniero vamos corriendo
  else
  {
    if (sensores.venpaca)
    {
      n5_target_f = sensores.GotoF;
      n5_target_c = sensores.GotoC;
      n5_plan_movimiento.clear();
      n5_estado = 1;
    }

    bool abortar_por_peligro = false;

    if (sensores.choque)
      abortar_por_peligro = true;

    // Detección visual miramos antes de hacer la accion para ver si es valida
    // Si el siguiente paso es caminar, revisamos la casilla de enfrente
    if (!n5_plan_movimiento.empty() && n5_plan_movimiento.front() == WALK)
    {
      char suelo_frente = sensores.superficie[2];
      int altura_frente = sensores.cota[2];
      int mi_altura = sensores.cota[0];
      int desnivel = abs(altura_frente - mi_altura);

      bool inviable = (suelo_frente == 'M' || suelo_frente == 'P' || desnivel > 1);
      if (suelo_frente == 'B' && !tiene_zapatillas)
        inviable = true;

      if (inviable)
        abortar_por_peligro = true;
    }

    if (abortar_por_peligro)
    {
      ubicacion yo = {sensores.posF, sensores.posC, sensores.rumbo};
      ubicacion peligro = Delante(yo);

      // Registramos la casilla peligrosa como obstáculo para el A*
      if (peligro.f >= 0 && peligro.f < mapaResultado.size() &&
          peligro.c >= 0 && peligro.c < mapaResultado[0].size())
      {
        n5_obs_f = peligro.f;
        n5_obs_c = peligro.c;
      }
      n5_plan_movimiento.clear(); // Borramos el plan para recalcular
    }

    // Ejecutamos el Nivel 5 (MoverA + Sincronización)
    Action a = ComportamientoTecnicoNivel_5(sensores);

    if (a == INSTALL)
    {
      n6_estado = 0;
    }

    // Si se queda atrapado volvemos a explorar para buscar otra vía
    if (a == IDLE && n5_espera_path > 0)
    {
      return ComportamientoTecnicoNivel_1(sensores);
    }

    return a;
  }
}

// =========================================================================
// FUNCIONES PROPORCIONADAS
// =========================================================================

/**
 * @brief Actualiza el mapaResultado y mapaCotas con la información de los sensores.
 * @param sensores Datos actuales de los sensores.
 */
void ComportamientoTecnico::ActualizarMapa(Sensores sensores)
{
  mapaResultado[sensores.posF][sensores.posC] = sensores.superficie[0];
  mapaCotas[sensores.posF][sensores.posC] = sensores.cota[0];

  int pos = 1;
  switch (sensores.rumbo)
  {
  case norte:
    for (int j = 1; j < 4; j++)
      for (int i = -j; i <= j; i++)
      {
        mapaResultado[sensores.posF - j][sensores.posC + i] = sensores.superficie[pos];
        mapaCotas[sensores.posF - j][sensores.posC + i] = sensores.cota[pos++];
      }
    break;
  case noreste:
    mapaResultado[sensores.posF - 1][sensores.posC] = sensores.superficie[1];
    mapaCotas[sensores.posF - 1][sensores.posC] = sensores.cota[1];
    mapaResultado[sensores.posF - 1][sensores.posC + 1] = sensores.superficie[2];
    mapaCotas[sensores.posF - 1][sensores.posC + 1] = sensores.cota[2];
    mapaResultado[sensores.posF][sensores.posC + 1] = sensores.superficie[3];
    mapaCotas[sensores.posF][sensores.posC + 1] = sensores.cota[3];
    mapaResultado[sensores.posF - 2][sensores.posC] = sensores.superficie[4];
    mapaCotas[sensores.posF - 2][sensores.posC] = sensores.cota[4];
    mapaResultado[sensores.posF - 2][sensores.posC + 1] = sensores.superficie[5];
    mapaCotas[sensores.posF - 2][sensores.posC + 1] = sensores.cota[5];
    mapaResultado[sensores.posF - 2][sensores.posC + 2] = sensores.superficie[6];
    mapaCotas[sensores.posF - 2][sensores.posC + 2] = sensores.cota[6];
    mapaResultado[sensores.posF - 1][sensores.posC + 2] = sensores.superficie[7];
    mapaCotas[sensores.posF - 1][sensores.posC + 2] = sensores.cota[7];
    mapaResultado[sensores.posF][sensores.posC + 2] = sensores.superficie[8];
    mapaCotas[sensores.posF][sensores.posC + 2] = sensores.cota[8];
    mapaResultado[sensores.posF - 3][sensores.posC] = sensores.superficie[9];
    mapaCotas[sensores.posF - 3][sensores.posC] = sensores.cota[9];
    mapaResultado[sensores.posF - 3][sensores.posC + 1] = sensores.superficie[10];
    mapaCotas[sensores.posF - 3][sensores.posC + 1] = sensores.cota[10];
    mapaResultado[sensores.posF - 3][sensores.posC + 2] = sensores.superficie[11];
    mapaCotas[sensores.posF - 3][sensores.posC + 2] = sensores.cota[11];
    mapaResultado[sensores.posF - 3][sensores.posC + 3] = sensores.superficie[12];
    mapaCotas[sensores.posF - 3][sensores.posC + 3] = sensores.cota[12];
    mapaResultado[sensores.posF - 2][sensores.posC + 3] = sensores.superficie[13];
    mapaCotas[sensores.posF - 2][sensores.posC + 3] = sensores.cota[13];
    mapaResultado[sensores.posF - 1][sensores.posC + 3] = sensores.superficie[14];
    mapaCotas[sensores.posF - 1][sensores.posC + 3] = sensores.cota[14];
    mapaResultado[sensores.posF][sensores.posC + 3] = sensores.superficie[15];
    mapaCotas[sensores.posF][sensores.posC + 3] = sensores.cota[15];
    break;
  case este:
    for (int j = 1; j < 4; j++)
      for (int i = -j; i <= j; i++)
      {
        mapaResultado[sensores.posF + i][sensores.posC + j] = sensores.superficie[pos];
        mapaCotas[sensores.posF + i][sensores.posC + j] = sensores.cota[pos++];
      }
    break;
  case sureste:
    mapaResultado[sensores.posF][sensores.posC + 1] = sensores.superficie[1];
    mapaCotas[sensores.posF][sensores.posC + 1] = sensores.cota[1];
    mapaResultado[sensores.posF + 1][sensores.posC + 1] = sensores.superficie[2];
    mapaCotas[sensores.posF + 1][sensores.posC + 1] = sensores.cota[2];
    mapaResultado[sensores.posF + 1][sensores.posC] = sensores.superficie[3];
    mapaCotas[sensores.posF + 1][sensores.posC] = sensores.cota[3];
    mapaResultado[sensores.posF][sensores.posC + 2] = sensores.superficie[4];
    mapaCotas[sensores.posF][sensores.posC + 2] = sensores.cota[4];
    mapaResultado[sensores.posF + 1][sensores.posC + 2] = sensores.superficie[5];
    mapaCotas[sensores.posF + 1][sensores.posC + 2] = sensores.cota[5];
    mapaResultado[sensores.posF + 2][sensores.posC + 2] = sensores.superficie[6];
    mapaCotas[sensores.posF + 2][sensores.posC + 2] = sensores.cota[6];
    mapaResultado[sensores.posF + 2][sensores.posC + 1] = sensores.superficie[7];
    mapaCotas[sensores.posF + 2][sensores.posC + 1] = sensores.cota[7];
    mapaResultado[sensores.posF + 2][sensores.posC] = sensores.superficie[8];
    mapaCotas[sensores.posF + 2][sensores.posC] = sensores.cota[8];
    mapaResultado[sensores.posF][sensores.posC + 3] = sensores.superficie[9];
    mapaCotas[sensores.posF][sensores.posC + 3] = sensores.cota[9];
    mapaResultado[sensores.posF + 1][sensores.posC + 3] = sensores.superficie[10];
    mapaCotas[sensores.posF + 1][sensores.posC + 3] = sensores.cota[10];
    mapaResultado[sensores.posF + 2][sensores.posC + 3] = sensores.superficie[11];
    mapaCotas[sensores.posF + 2][sensores.posC + 3] = sensores.cota[11];
    mapaResultado[sensores.posF + 3][sensores.posC + 3] = sensores.superficie[12];
    mapaCotas[sensores.posF + 3][sensores.posC + 3] = sensores.cota[12];
    mapaResultado[sensores.posF + 3][sensores.posC + 2] = sensores.superficie[13];
    mapaCotas[sensores.posF + 3][sensores.posC + 2] = sensores.cota[13];
    mapaResultado[sensores.posF + 3][sensores.posC + 1] = sensores.superficie[14];
    mapaCotas[sensores.posF + 3][sensores.posC + 1] = sensores.cota[14];
    mapaResultado[sensores.posF + 3][sensores.posC] = sensores.superficie[15];
    mapaCotas[sensores.posF + 3][sensores.posC] = sensores.cota[15];
    break;
  case sur:
    for (int j = 1; j < 4; j++)
      for (int i = -j; i <= j; i++)
      {
        mapaResultado[sensores.posF + j][sensores.posC - i] = sensores.superficie[pos];
        mapaCotas[sensores.posF + j][sensores.posC - i] = sensores.cota[pos++];
      }
    break;
  case suroeste:
    mapaResultado[sensores.posF + 1][sensores.posC] = sensores.superficie[1];
    mapaCotas[sensores.posF + 1][sensores.posC] = sensores.cota[1];
    mapaResultado[sensores.posF + 1][sensores.posC - 1] = sensores.superficie[2];
    mapaCotas[sensores.posF + 1][sensores.posC - 1] = sensores.cota[2];
    mapaResultado[sensores.posF][sensores.posC - 1] = sensores.superficie[3];
    mapaCotas[sensores.posF][sensores.posC - 1] = sensores.cota[3];
    mapaResultado[sensores.posF + 2][sensores.posC] = sensores.superficie[4];
    mapaCotas[sensores.posF + 2][sensores.posC] = sensores.cota[4];
    mapaResultado[sensores.posF + 2][sensores.posC - 1] = sensores.superficie[5];
    mapaCotas[sensores.posF + 2][sensores.posC - 1] = sensores.cota[5];
    mapaResultado[sensores.posF + 2][sensores.posC - 2] = sensores.superficie[6];
    mapaCotas[sensores.posF + 2][sensores.posC - 2] = sensores.cota[6];
    mapaResultado[sensores.posF + 1][sensores.posC - 2] = sensores.superficie[7];
    mapaCotas[sensores.posF + 1][sensores.posC - 2] = sensores.cota[7];
    mapaResultado[sensores.posF][sensores.posC - 2] = sensores.superficie[8];
    mapaCotas[sensores.posF][sensores.posC - 2] = sensores.cota[8];
    mapaResultado[sensores.posF + 3][sensores.posC] = sensores.superficie[9];
    mapaCotas[sensores.posF + 3][sensores.posC] = sensores.cota[9];
    mapaResultado[sensores.posF + 3][sensores.posC - 1] = sensores.superficie[10];
    mapaCotas[sensores.posF + 3][sensores.posC - 1] = sensores.cota[10];
    mapaResultado[sensores.posF + 3][sensores.posC - 2] = sensores.superficie[11];
    mapaCotas[sensores.posF + 3][sensores.posC - 2] = sensores.cota[11];
    mapaResultado[sensores.posF + 3][sensores.posC - 3] = sensores.superficie[12];
    mapaCotas[sensores.posF + 3][sensores.posC - 3] = sensores.cota[12];
    mapaResultado[sensores.posF + 2][sensores.posC - 3] = sensores.superficie[13];
    mapaCotas[sensores.posF + 2][sensores.posC - 3] = sensores.cota[13];
    mapaResultado[sensores.posF + 1][sensores.posC - 3] = sensores.superficie[14];
    mapaCotas[sensores.posF + 1][sensores.posC - 3] = sensores.cota[14];
    mapaResultado[sensores.posF][sensores.posC - 3] = sensores.superficie[15];
    mapaCotas[sensores.posF][sensores.posC - 3] = sensores.cota[15];
    break;
  case oeste:
    for (int j = 1; j < 4; j++)
      for (int i = -j; i <= j; i++)
      {
        mapaResultado[sensores.posF - i][sensores.posC - j] = sensores.superficie[pos];
        mapaCotas[sensores.posF - i][sensores.posC - j] = sensores.cota[pos++];
      }
    break;
  case noroeste:
    mapaResultado[sensores.posF][sensores.posC - 1] = sensores.superficie[1];
    mapaCotas[sensores.posF][sensores.posC - 1] = sensores.cota[1];
    mapaResultado[sensores.posF - 1][sensores.posC - 1] = sensores.superficie[2];
    mapaCotas[sensores.posF - 1][sensores.posC - 1] = sensores.cota[2];
    mapaResultado[sensores.posF - 1][sensores.posC] = sensores.superficie[3];
    mapaCotas[sensores.posF - 1][sensores.posC] = sensores.cota[3];
    mapaResultado[sensores.posF][sensores.posC - 2] = sensores.superficie[4];
    mapaCotas[sensores.posF][sensores.posC - 2] = sensores.cota[4];
    mapaResultado[sensores.posF - 1][sensores.posC - 2] = sensores.superficie[5];
    mapaCotas[sensores.posF - 1][sensores.posC - 2] = sensores.cota[5];
    mapaResultado[sensores.posF - 2][sensores.posC - 2] = sensores.superficie[6];
    mapaCotas[sensores.posF - 2][sensores.posC - 2] = sensores.cota[6];
    mapaResultado[sensores.posF - 2][sensores.posC - 1] = sensores.superficie[7];
    mapaCotas[sensores.posF - 2][sensores.posC - 1] = sensores.cota[7];
    mapaResultado[sensores.posF - 2][sensores.posC] = sensores.superficie[8];
    mapaCotas[sensores.posF - 2][sensores.posC] = sensores.cota[8];
    mapaResultado[sensores.posF][sensores.posC - 3] = sensores.superficie[9];
    mapaCotas[sensores.posF][sensores.posC - 3] = sensores.cota[9];
    mapaResultado[sensores.posF - 1][sensores.posC - 3] = sensores.superficie[10];
    mapaCotas[sensores.posF - 1][sensores.posC - 3] = sensores.cota[10];
    mapaResultado[sensores.posF - 2][sensores.posC - 3] = sensores.superficie[11];
    mapaCotas[sensores.posF - 2][sensores.posC - 3] = sensores.cota[11];
    mapaResultado[sensores.posF - 3][sensores.posC - 3] = sensores.superficie[12];
    mapaCotas[sensores.posF - 3][sensores.posC - 3] = sensores.cota[12];
    mapaResultado[sensores.posF - 3][sensores.posC - 2] = sensores.superficie[13];
    mapaCotas[sensores.posF - 3][sensores.posC - 2] = sensores.cota[13];
    mapaResultado[sensores.posF - 3][sensores.posC - 1] = sensores.superficie[14];
    mapaCotas[sensores.posF - 3][sensores.posC - 1] = sensores.cota[14];
    mapaResultado[sensores.posF - 3][sensores.posC] = sensores.superficie[15];
    mapaCotas[sensores.posF - 3][sensores.posC] = sensores.cota[15];
    break;
  }
}

/**
 * @brief Determina si una casilla es transitable para el técnico.
 * En esta práctica, si el técnico tiene zapatillas, el bosque ('B') es transitable.
 * @param f Fila de la casilla.
 * @param c Columna de la casilla.
 * @param tieneZapatillas Indica si el agente posee las zapatillas.
 * @return true si la casilla es transitable.
 */
bool ComportamientoTecnico::EsCasillaTransitableLevel0(int f, int c, bool tieneZapatillas)
{
  if (f < 0 || f >= mapaResultado.size() || c < 0 || c >= mapaResultado[0].size())
    return false;
  return es_camino(mapaResultado[f][c]); // Solo 'C', 'S', 'D', 'U' son transitables en Nivel 0
}

/**
 * @brief Comprueba si la casilla de delante es accesible por diferencia de altura.
 * Para el técnico: desnivel máximo siempre 1.
 * @param actual Estado actual del agente (fila, columna, orientacion).
 * @return true si el desnivel con la casilla de delante es admisible.
 */
bool ComportamientoTecnico::EsAccesiblePorAltura(const ubicacion &actual)
{
  ubicacion del = Delante(actual);
  if (del.f < 0 || del.f >= mapaCotas.size() || del.c < 0 || del.c >= mapaCotas[0].size())
    return false;
  int desnivel = abs(mapaCotas[del.f][del.c] - mapaCotas[actual.f][actual.c]);
  if (desnivel > 1)
    return false;
  return true;
}

/**
 * @brief Devuelve la posición (fila, columna) de la casilla que hay delante del agente.
 * Calcula la casilla frontal según la orientación actual (8 direcciones).
 * @param actual Estado actual del agente (fila, columna, orientacion).
 * @return Estado con la fila y columna de la casilla de enfrente.
 */
ubicacion ComportamientoTecnico::Delante(const ubicacion &actual) const
{
  ubicacion delante = actual;
  switch (actual.brujula)
  {
  case 0:
    delante.f--;
    break; // norte
  case 1:
    delante.f--;
    delante.c++;
    break; // noreste
  case 2:
    delante.c++;
    break; // este
  case 3:
    delante.f++;
    delante.c++;
    break; // sureste
  case 4:
    delante.f++;
    break; // sur
  case 5:
    delante.f++;
    delante.c--;
    break; // suroeste
  case 6:
    delante.c--;
    break; // oeste
  case 7:
    delante.f--;
    delante.c--;
    break; // noroeste
  }
  return delante;
}

/**
 * @brief Imprime por consola la secuencia de acciones de un plan.
 *
 * @param plan  Lista de acciones del plan.
 */
void ComportamientoTecnico::PintaPlan(const list<Action> &plan)
{
  auto it = plan.begin();
  while (it != plan.end())
  {
    if (*it == WALK)
    {
      cout << "W ";
    }
    else if (*it == JUMP)
    {
      cout << "J ";
    }
    else if (*it == TURN_SR)
    {
      cout << "r ";
    }
    else if (*it == TURN_SL)
    {
      cout << "l ";
    }
    else if (*it == COME)
    {
      cout << "C ";
    }
    else if (*it == IDLE)
    {
      cout << "I ";
    }
    else
    {
      cout << "-_ ";
    }
    it++;
  }
  cout << "( longitud " << plan.size() << ")" << endl;
}

/**
 * @brief Convierte un plan de acciones en una lista de casillas para
 *        su visualización en el mapa 2D.
 *
 * @param st    Estado de partida.
 * @param plan  Lista de acciones del plan.
 */
void ComportamientoTecnico::VisualizaPlan(const ubicacion &st,
                                          const list<Action> &plan)
{
  listaPlanCasillas.clear();
  ubicacion cst = st;

  listaPlanCasillas.push_back({cst.f, cst.c, WALK});
  auto it = plan.begin();
  while (it != plan.end())
  {

    switch (*it)
    {
    case JUMP:
      switch (cst.brujula)
      {
      case 0:
        cst.f--;
        break;
      case 1:
        cst.f--;
        cst.c++;
        break;
      case 2:
        cst.c++;
        break;
      case 3:
        cst.f++;
        cst.c++;
        break;
      case 4:
        cst.f++;
        break;
      case 5:
        cst.f++;
        cst.c--;
        break;
      case 6:
        cst.c--;
        break;
      case 7:
        cst.f--;
        cst.c--;
        break;
      }
      if (cst.f >= 0 && cst.f < mapaResultado.size() &&
          cst.c >= 0 && cst.c < mapaResultado[0].size())
        listaPlanCasillas.push_back({cst.f, cst.c, JUMP});
    case WALK:
      switch (cst.brujula)
      {
      case 0:
        cst.f--;
        break;
      case 1:
        cst.f--;
        cst.c++;
        break;
      case 2:
        cst.c++;
        break;
      case 3:
        cst.f++;
        cst.c++;
        break;
      case 4:
        cst.f++;
        break;
      case 5:
        cst.f++;
        cst.c--;
        break;
      case 6:
        cst.c--;
        break;
      case 7:
        cst.f--;
        cst.c--;
        break;
      }
      if (cst.f >= 0 && cst.f < mapaResultado.size() &&
          cst.c >= 0 && cst.c < mapaResultado[0].size())
        listaPlanCasillas.push_back({cst.f, cst.c, WALK});
      break;
    case TURN_SR:
      cst.brujula = (Orientacion)(((int)cst.brujula + 1) % 8);
      break;
    case TURN_SL:
      cst.brujula = (Orientacion)(((int)cst.brujula + 7) % 8);
      break;
    }
    it++;
  }
}
