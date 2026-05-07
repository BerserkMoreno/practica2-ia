#include "ingeniero.hpp"
#include "motorlib/util.h"
#include <iostream>
#include <queue>
#include <set>
#include <cstdlib>

using namespace std;

// =========================================================================
// ÁREA DE IMPLEMENTACIÓN DEL ESTUDIANTE
// =========================================================================

Action ComportamientoIngeniero::think(Sensores sensores)
{
  Action accion = IDLE;

  // Decisión del agente según el nivel
  switch (sensores.nivel)
  {
  case 0:
    accion = ComportamientoIngenieroNivel_0(sensores);
    break;
  case 1:
    accion = ComportamientoIngenieroNivel_1(sensores);
    break;
  case 2:
    accion = ComportamientoIngenieroNivel_2(sensores);
    break;
  case 3:
    accion = ComportamientoIngenieroNivel_3(sensores);
    break;
  case 4:
    accion = ComportamientoIngenieroNivel_4(sensores);
    break;
  case 5:
    accion = ComportamientoIngenieroNivel_5(sensores);
    break;
  case 6:
    accion = ComportamientoIngenieroNivel_6(sensores);
    break;
  }

  return accion;
}

//_________________________________________________________
// Nivel 0
//_________________________________________________________

// Guardamos el momento exacto (el instante) en el que pisamos esta casilla.
void ComportamientoIngeniero::RegistrarPasoEnElReloj(ubicacion actual)
{
  mapaUltimoPaso[actual.f][actual.c] = instanteActual; // Este instante lo actualizamos en el ComportamientoIngenieroNivel_0
  nVisitas[actual.f][actual.c]++;
}

// Comprueba si una casilla es pisable y no hay nadie estorbando.
bool ComportamientoIngeniero::EsCasillaDespejada(ubicacion destino, int idx_sensor, const Sensores &sensores)
{

  // EsCasillaTransitableLevel0 ya es una funcion que nos proporcionan y que hace justo lo que necesitamos
  if (!EsCasillaTransitableLevel0(destino.f, destino.c, tiene_zapatillas))
    return false;

  if (sensores.agentes[idx_sensor] != '_')
    return false; // Con esto marcamos que la casilla esta desocupada _ por tanto miramos si no esta desocupada

  return true;
}

// Evalúa si el Ingeniero puede caminar.
bool ComportamientoIngeniero::SePuedeCaminar(ubicacion origen, int idx_sensor, const Sensores &sensores)
{
  ubicacion destino = Delante(origen); // Gracias a la función delante podemos ver la casilla que tenemos delante sin complicarnos la vida

  if (!EsCasillaDespejada(destino, idx_sensor, sensores))
    return false;

  // Comprobamos si es Accesible por altura esta funcion que nos dan lo que hace es coger la posicion donde estamos y usar Delante para comparar las alturas y decidir si es o no posible por la altura
  if (!EsAccesiblePorAltura(origen, tiene_zapatillas))
    return false;

  return true;
}

// Evalúa si el Ingeniero puede dar un salto largo.
bool ComportamientoIngeniero::SePuedeSaltar(ubicacion origen, int idx_intermedio, int idx_destino, const Sensores &sensores)
{
  ubicacion ubi_intermedia = Delante(origen);
  ubicacion ubi_destino = Delante(ubi_intermedia);

  // Casilla intermedia pisable vacia y no más alta que nosotros
  if (!EsCasillaDespejada(ubi_intermedia, idx_intermedio, sensores))
    return false;
  // if (mapaCotas[ubi_intermedia.f][ubi_intermedia.c] > mapaCotas[origen.f][origen.c])
  //   return false;

  // Casilla destino pisable y vacia
  if (!EsCasillaDespejada(ubi_destino, idx_destino, sensores))
    return false;

  // Altura final tengo q teenr en cuenta el origen y el destino porq si tengo en cuenta la intermedia puede pasar que en cada una suba 1 o 2 este permitido pero desde la origen la altura no seria permitida
  int desnivel = abs(mapaCotas[ubi_destino.f][ubi_destino.c] - mapaCotas[origen.f][origen.c]);
  int max_salto = tiene_zapatillas ? 2 : 1;

  return (desnivel <= max_salto);
}

// Calcula el interés de una casilla basándose en las instancias
int ComportamientoIngeniero::CalcularInteres(ubicacion ubi_destino, char terreno)
{
  int puntos = 1000;

  if (terreno == 'U') // Si es una planta de residuos le doy mas 10000 para que vaya de una que es lo que nos interesa
    puntos += 10000;
  else if (terreno == 'D' && !tiene_zapatillas) // Si no tiene zapatillas me interesa tambien bastante que las pille
    puntos += 5000;

  // De esta forma gracias a los instantes una casilla que he pisado al principio valdra 10 y se me hace ams suculenta que una que acabo de pisar y alomejor vale 500
  puntos -= mapaUltimoPaso[ubi_destino.f][ubi_destino.c];

  return puntos;
}

// Compara en una dirección si es mejor Caminar o Saltar.
void ComportamientoIngeniero::EvaluarRuta(ubicacion mirada, int idx_W, int idx_J, const Sensores &sensores, int &mejor_puntos, bool &debo_saltar)
{
  mejor_puntos = -999999;
  debo_saltar = false;

  ubicacion ubi_cerca = Delante(mirada);
  ubicacion ubi_lejos = Delante(ubi_cerca);

  int puntos_W = -999999;
  int puntos_J = -999999;

  if (SePuedeCaminar(mirada, idx_W, sensores))
  {
    puntos_W = CalcularInteres(ubi_cerca, sensores.superficie[idx_W]);
  }

  if (SePuedeSaltar(mirada, idx_W, idx_J, sensores))
  {
    puntos_J = CalcularInteres(ubi_lejos, sensores.superficie[idx_J]);

    // Si nunca hemos pisado la casilla intermedia, penalizamos el salto para explorar porque sino nos podemos saltar caminos que nos llevan a la solucion.
    if (mapaUltimoPaso[ubi_cerca.f][ubi_cerca.c] == 0)
      puntos_J -= 200;
    else
      puntos_J += 50;
  }

  if (puntos_J > puntos_W)
  {
    mejor_puntos = puntos_J;
    debo_saltar = true;
  }
  else
  {
    mejor_puntos = puntos_W;
    debo_saltar = false;
  }
}

Action ComportamientoIngeniero::ComportamientoIngenieroNivel_0(Sensores sensores)
{
  instanteActual++; // Actualizo los instantes
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

  ubicacion yo = {sensores.posF, sensores.posC, sensores.rumbo}; // Guardo mi posicion actual  rumbo me da la orientacion
  RegistrarPasoEnElReloj(yo);

  if (sensores.superficie[0] == 'D')
    tiene_zapatillas = true;

  if (sensores.superficie[1] == 'U' && SePuedeCaminar(yo, 1, sensores))
    return TURN_SL;

  if (sensores.superficie[2] == 'U' && SePuedeCaminar(yo, 2, sensores))
    return WALK;

  // if (sensores.superficie[3] == 'U')
  // return TURN_SR;

  if (sensores.superficie[0] == 'U') // Si estamos en una planta de residuos hemos terminado
    return IDLE;

  // Miraditas para ver por donde me interesa ams
  ubicacion frente = yo;
  ubicacion izq = yo;
  izq.brujula = (Orientacion)((yo.brujula + 7) % 8); // Esto es precioso a partir de mi posicion actual simulo un giro de 45 grados para ver q tal seria por ese camino
  ubicacion der = yo;
  der.brujula = (Orientacion)((yo.brujula + 1) % 8);

  int pts_F, pts_I, pts_D;
  bool saltar_F, saltar_I, saltar_D;

  EvaluarRuta(frente, 2, 6, sensores, pts_F, saltar_F);
  EvaluarRuta(izq, 1, 4, sensores, pts_I, saltar_I);
  EvaluarRuta(der, 3, 8, sensores, pts_D, saltar_D);

  int ganador = pts_F;
  if (pts_I > ganador)
    ganador = pts_I;
  if (pts_D > ganador)
    ganador = pts_D;

  if (ganador <= -999999)
  { // Por fefecto tenia girar a la izquierda y funcionaba el 17 ahora lo que he hecho es tomar una decision aleatoria si pasa por la misma casilla mas de 5 veces
    if (nVisitas[yo.f][yo.c] > 5)
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
    return TURN_SL;
  }

  if (ganador == pts_F)
    return saltar_F ? JUMP : WALK;
  if (ganador == pts_I)
    return TURN_SL;

  return TURN_SR;
}

/**
 * @brief Comprueba si una celda es de tipo camino transitable.
 * @param c Carácter que representa el tipo de superficie.
 * @return true si es camino ('C'), zapatillas ('D') o meta ('U').
 */
bool ComportamientoIngeniero::es_camino(unsigned char c) const
{
  return (c == 'C' || c == 'D' || c == 'U');
}

//_________________________________________________________
// Nivel 1
//_________________________________________________________

// Para el nivel 1 los senderos tambien son transitables
bool ComportamientoIngeniero::es_transitable_nivel_1(unsigned char c) const
{
  // En el nivel 6 tambien me interesa que pueda ir por la hierva
  if (n6)
  {
    return (c == 'C' || c == 'S' || c == 'D' || c == 'U' || c == 'H'); // Super necesario para el 6
  }

  return (c == 'C' || c == 'S' || c == 'D' || c == 'U');
}

// Vemos si la casilla es valida y no hay nadie en ella
bool ComportamientoIngeniero::EsCasillaDespejadaN1(ubicacion destino, int idx_sensor, const Sensores &sensores)
{
  // Primero de todo comproabmos que el destino no se salga del mapa ya que si esto pasa el programa hace BOOM xD
  if (destino.f < 0 || destino.f >= mapaResultado.size() || destino.c < 0 || destino.c >= mapaResultado[0].size())
    return false;

  if (!es_transitable_nivel_1(mapaResultado[destino.f][destino.c]))
    return false;

  if (sensores.agentes[idx_sensor] != '_') // Comprobamos que esta despejada ya que _ nos dice q esta despejada
    return false;

  return true;
}

bool ComportamientoIngeniero::SePuedeCaminarN1(ubicacion origen, int idx_sensor, const Sensores &sensores)
{
  ubicacion destino = Delante(origen);
  if (!EsCasillaDespejadaN1(destino, idx_sensor, sensores))
    return false;
  // Usamos la funcion que ya nos dan para ver si es viable por altura
  if (!EsAccesiblePorAltura(origen, tiene_zapatillas))
    return false;

  return true;
}

bool ComportamientoIngeniero::SePuedeSaltarN1(ubicacion origen, int idx_intermedio, int idx_destino, const Sensores &sensores)
{
  ubicacion ubi_intermedia = Delante(origen);
  ubicacion ubi_destino = Delante(ubi_intermedia);

  // Comprobaciones con la casilla intermedia
  if (!EsCasillaDespejadaN1(ubi_intermedia, idx_intermedio, sensores))
    return false;
  // Comprobamos si es viable por altura la casilla intermedia
  // if (mapaCotas[ubi_intermedia.f][ubi_intermedia.c] > mapaCotas[origen.f][origen.c])
  //  return false;

  // Comprobaciones con la casilla destino
  if (!EsCasillaDespejadaN1(ubi_destino, idx_destino, sensores))
    return false;
  int desnivel = abs(mapaCotas[ubi_destino.f][ubi_destino.c] - mapaCotas[origen.f][origen.c]);
  int max_salto = tiene_zapatillas ? 2 : 1;

  return (desnivel <= max_salto);
}

int ComportamientoIngeniero::ContarDesconocidos(const ubicacion &centro)
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

int ComportamientoIngeniero::CalcularInteresN1(ubicacion ubi_destino, char terreno)
{
  int puntos = 100000;

  // Comprobamos los 2 siguientes pasos en la dirección que vamos a tomar
  ubicacion paso2 = Delante(ubi_destino);
  ubicacion paso3 = Delante(paso2);

  // Si vemos que adelante hay un muro o precipicio restamos puntos para que el agente NO quiera ir en esa direccion ya que no nos interesa y perderiamos tiempo al ir alli
  if (mapaResultado[paso2.f][paso2.c] == 'M' || mapaResultado[paso2.f][paso2.c] == 'P')
    puntos -= 80000;
  if (mapaResultado[paso3.f][paso3.c] == 'M' || mapaResultado[paso3.f][paso3.c] == 'P')
    puntos -= 40000;

  // Consulta mi mapa de memoria (nVisitas) para saber cuantas veces ha pisado esa casilla ya que sin esto no toma riesgos siempre se va por la que ha pasado hace mas tiempo y al final tenemos un bucle con esta convinacion hacemos que deje de dar vueltas y sea mas valiente o al menos eso es el intento
  int visitas = nVisitas[ubi_destino.f][ubi_destino.c];
  puntos -= (visitas * 5000);

  // RADAR MANHATTAN lo que me interesa realmente esque sea valiente y quiera explorarlo todo para poder llegar a caminos y senderos escondidos entoces lo que mas me interesa es explorar lo desconocido con esto priorizo las casillas que estan mas cerca de lo desconocido.
  int dist_niebla = ContarDesconocidos(ubi_destino);
  puntos -= (dist_niebla * 500);

  return puntos;
}

void ComportamientoIngeniero::EvaluarRutaN1(ubicacion mirada, int idx_W, int idx_J, const Sensores &sensores, int &mejor_puntos, bool &debo_saltar)
{
  mejor_puntos = -999999;
  debo_saltar = false;

  ubicacion ubi_cerca = Delante(mirada);
  ubicacion ubi_lejos = Delante(ubi_cerca);

  int puntos_W = -999999;
  int puntos_J = -999999;

  if (SePuedeCaminarN1(mirada, idx_W, sensores))
    puntos_W = CalcularInteresN1(ubi_cerca, sensores.superficie[idx_W]);

  if (SePuedeSaltarN1(mirada, idx_W, idx_J, sensores))
  {
    puntos_J = CalcularInteresN1(ubi_lejos, sensores.superficie[idx_J]);

    // Consultamos visitas en la casilla que saltaríamos por encima
    int visitas_intermedio = 0;
    if (!nVisitas.empty() && ubi_cerca.f >= 0 && ubi_cerca.f < nVisitas.size() && ubi_cerca.c >= 0 && ubi_cerca.c < nVisitas[0].size())
    {
      visitas_intermedio = nVisitas[ubi_cerca.f][ubi_cerca.c];
    }

    // Penalizams si intentamos saltar sobre una casilla que nunca hemos pisado puede ser una entrada a un pasillo lateral que dejariamos sin descubrir
    if (visitas_intermedio == 0)
      puntos_J -= 500; // He probado con varios valores y este es el que mejor resulatdos me ha dado en la exploración
    else
      puntos_J += 50;
  }

  if (puntos_J > puntos_W)
  {
    mejor_puntos = puntos_J;
    debo_saltar = true;
  }
  else
  {
    mejor_puntos = puntos_W;
    debo_saltar = false;
  }
}

Action ComportamientoIngeniero::ComportamientoIngenieroNivel_1(Sensores sensores)
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

  RegistrarPasoEnElReloj(yo); // Reusamos la funcion que habiamos hecho para el nivel 0

  // Registramos la visita en la casilla que estamos ahora mismo
  // nVisitas[yo.f][yo.c]++; //Ya la cuento en RegistrarPasoEnElReloj

  if (sensores.superficie[0] == 'D')
    tiene_zapatillas = true;

  ubicacion frente = yo;
  ubicacion izq = yo;
  izq.brujula = (Orientacion)((yo.brujula + 7) % 8);
  ubicacion der = yo;
  der.brujula = (Orientacion)((yo.brujula + 1) % 8);

  int pts_F, pts_I, pts_D;
  bool saltar_F, saltar_I, saltar_D;

  EvaluarRutaN1(frente, 2, 6, sensores, pts_F, saltar_F);
  EvaluarRutaN1(izq, 1, 4, sensores, pts_I, saltar_I);
  EvaluarRutaN1(der, 3, 8, sensores, pts_D, saltar_D);

  int ganador = pts_F;
  if (pts_I > ganador)
    ganador = pts_I;
  if (pts_D > ganador)
    ganador = pts_D;

  if (ganador <= -999999)
    return TURN_SL; // si nos hemos encontrado co algo q nos bloquea giramos

  if (ganador == pts_F)
    return saltar_F ? JUMP : WALK;
  if (ganador == pts_I)
    return TURN_SL;

  return TURN_SR;
}

//_________________________________________________________
// Nivel 2
//_________________________________________________________

// Calcula las coordenadas de la siguiente casilla según la orientación actual.
EstadoI NextCasillaIngeniero(const EstadoI &st)
{
  EstadoI siguiente = st; // Crea una copia del estado actual
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

// Comprobamos si podemos caminar
bool CasillaAccesibleIngeniero(const EstadoI &st, const vector<vector<unsigned char>> &terreno, const vector<vector<unsigned char>> &altura)
{
  // Calculamos dónde acabaríamos si caminamos
  EstadoI next = NextCasillaIngeniero(st);

  // Comprobamos que no nos salimos del mapa (evitar Segmentation Fault por salir de la matriz)
  if (next.site.f < 0 || next.site.f >= terreno.size() ||
      next.site.c < 0 || next.site.c >= terreno[0].size())
    return false; // Necesario para el nivel 6

  // casillas no pisables
  if (terreno[next.site.f][next.site.c] == 'P' ||
      terreno[next.site.f][next.site.c] == 'M' ||
      terreno[next.site.f][next.site.c] == 'B')
    return false;

  // Comprobacion de la altura(desnivel)
  int diff = abs(altura[next.site.f][next.site.c] - altura[st.site.f][st.site.c]);
  if (st.zapatillas)
    return diff <= 2;
  else
    return diff <= 1;
}

// Comprobamos si podemos saltar
bool SaltoValidoIngeniero(const EstadoI &st, const vector<vector<unsigned char>> &terreno, const vector<vector<unsigned char>> &altura)
{
  // calculamos la casilla intermedia y la siguiente a esta la que nos interesa la casilla final
  EstadoI intermedia = NextCasillaIngeniero(st);
  EstadoI final = NextCasillaIngeniero(intermedia);

  // Comprobamos que no nos salimos del mapa (evitar Segmentation Fault por salir de la matriz)
  if (intermedia.site.f < 0 || intermedia.site.f >= terreno.size() ||
      intermedia.site.c < 0 || intermedia.site.c >= terreno[0].size())
    return false; // Necesario para el nivel 6
  if (final.site.f < 0 || final.site.f >= terreno.size() ||
      final.site.c < 0 || final.site.c >= terreno[0].size())
    return false;

  // casillas no pisables
  if (terreno[intermedia.site.f][intermedia.site.c] == 'P' ||
      terreno[intermedia.site.f][intermedia.site.c] == 'M' ||
      terreno[intermedia.site.f][intermedia.site.c] == 'B')
    return false;

  if (terreno[final.site.f][final.site.c] == 'P' ||
      terreno[final.site.f][final.site.c] == 'M' ||
      terreno[final.site.f][final.site.c] == 'B')
    return false;

  // Comprobacion de la altura(desnivel)
  int diff = abs(altura[final.site.f][final.site.c] - altura[st.site.f][st.site.c]);
  if (st.zapatillas)
    return diff <= 2;
  else
    return diff <= 1;
}

// Con esta funcion simulamos que es lo que pasa al hacer un movimiento
// Devuelve el estado en el que quedará el agente tras hacerla
// Solo modifica el estado si el movimiento es valido
EstadoI applyI(Action accion, const EstadoI &st, const vector<vector<unsigned char>> &terreno, const vector<vector<unsigned char>> &altura)
{

  EstadoI next = st;

  switch (accion)
  {

  case WALK:
    if (CasillaAccesibleIngeniero(st, terreno, altura))
    {
      next = NextCasillaIngeniero(st);
    }
    break;

  case JUMP:
    if (SaltoValidoIngeniero(st, terreno, altura))
    {
      EstadoI intermedia = NextCasillaIngeniero(st);
      next = NextCasillaIngeniero(intermedia);
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

// Encuentra el camino más corto en numero de acciones desde el inicio hasta el final
list<Action> ComportamientoIngeniero::B_Anchura_Ingeniero(const EstadoI &inicio, const EstadoI &final, const vector<vector<unsigned char>> &mapa, const vector<vector<unsigned char>> &cotas)
{

  NodoI current_node;   // Nodo que se está evaluando actualmente
  list<NodoI> frontier; // lista de nodos por explorar (FIFO)
  set<NodoI> explored;  // Conjunto de nodos ya visitados
  list<Action> path;    // Aquí es donde guardamos la solución final

  current_node.estado = inicio;     // Inicia con el estado actual del agente
  frontier.push_back(current_node); // Mete el estado inicial a la cola

  bool SolutionFound = false;

  while (!SolutionFound && !frontier.empty())
  {

    current_node = frontier.front(); // Sacamos el primer nodo de la cola (como es por anchura sacamos el más antiguo)
    frontier.pop_front();

    if (explored.find(current_node) != explored.end()) // Si ese estado exacto ya lo hemos recorrido nos lo saltamos (si no teniamos zapatillas y ahora si son estados diferentes)
      continue;

    explored.insert(current_node); // Si es la primera vez que estamos en este estado pues lo pasamos a la lista de explorados

    // zapatillas
    if (mapa[current_node.estado.site.f][current_node.estado.site.c] == 'D')
    {
      current_node.estado.zapatillas = true;
    }

    // comprobar si es solución
    if (current_node.estado.site.f == final.site.f && // Si es solucion guardamos las acciones que nos han llevado hasta esta solucion y salimos a la fuerza
        current_node.estado.site.c == final.site.c)
    {

      SolutionFound = true;
      path = current_node.secuencia;
      break;
    }

    // EVALUAMOS LAS 4 POSIBLES ACCIONES

    // WALK
    NodoI child_Walk = current_node;
    child_Walk.estado = applyI(WALK, current_node.estado, mapa, cotas);

    if (child_Walk.estado != current_node.estado &&  // Si se ha modificado la accion es decir no nos hemos chocado y
        explored.find(child_Walk) == explored.end()) // este nodo no lo hemos explorado antes añadimos su accion y lo metemos en la cola
    {

      child_Walk.secuencia.push_back(WALK);
      frontier.push_back(child_Walk);
    }

    // JUMP
    NodoI child_Jump = current_node;
    child_Jump.estado = applyI(JUMP, current_node.estado, mapa, cotas);

    if (child_Jump.estado != current_node.estado &&
        explored.find(child_Jump) == explored.end())
    {

      child_Jump.secuencia.push_back(JUMP);
      frontier.push_back(child_Jump);
    }

    // TURN RIGHT
    NodoI child_TurnSR = current_node;
    child_TurnSR.estado = applyI(TURN_SR, current_node.estado, mapa, cotas);

    if (explored.find(child_TurnSR) == explored.end()) // Los giros NUNCA fallan por colisión, así que solo comprobamos si ya exploramos ese estado
    {
      child_TurnSR.secuencia.push_back(TURN_SR);
      frontier.push_back(child_TurnSR);
    }

    // TURN LEFT
    NodoI child_TurnSL = current_node;
    child_TurnSL.estado = applyI(TURN_SL, current_node.estado, mapa, cotas);

    if (explored.find(child_TurnSL) == explored.end())
    {
      child_TurnSL.secuencia.push_back(TURN_SL);
      frontier.push_back(child_TurnSL);
    }
  }

  return path;
}

Action ComportamientoIngeniero::ComportamientoIngenieroNivel_2(Sensores sensores)
{
  Action accion = IDLE;

  // Si no sabemos que hacer pues lo planificamos
  if (!hayPlan)
  {

    EstadoI inicio, fin;

    inicio.site.f = sensores.posF;
    inicio.site.c = sensores.posC;
    inicio.site.brujula = sensores.rumbo; // Rumbo nos da la orientacion
    inicio.zapatillas = tiene_zapatillas;

    fin.site.f = sensores.BelPosF;
    fin.site.c = sensores.BelPosC;

    plan = B_Anchura_Ingeniero(inicio, fin, mapaResultado, mapaCotas);

    VisualizaPlan(inicio.site, plan); // Funcion que ya nos dan para pintar el camino que hemos calculado en el mapa

    hayPlan = !plan.empty();
  }

  // Si ya tenemos plan pues lo realizamos
  if (hayPlan && !plan.empty())
  {
    accion = plan.front();
    plan.pop_front();
  }

  // Si la lista esta vacia ha pasado algo y tenemos que calcular un nuevo plan
  if (plan.empty())
  {
    hayPlan = false;
  }

  return accion;
}

//_________________________________________________________
// Nivel 3
//_________________________________________________________

/**
 * @brief Comportamiento del ingeniero para el Nivel 3.
 * @param sensores Datos actuales de los sensores.
 * @return Acción a realizar.
 */
// Para que el ingeniero se aparte del camino y no moleste
Action ComportamientoIngeniero::ComportamientoIngenieroNivel_3(Sensores sensores)
{

  if (sensores.posF < 0 || sensores.posF >= mapaResultado.size() || // Comprobamos si estamos fuera del mapa de ser asi no hacemos nada
      sensores.posC < 0 || sensores.posC >= mapaResultado[0].size())
  {
    return IDLE;
  }

  int cota_actual = sensores.cota[0];
  int cota_frente = sensores.cota[2];
  int diff_altura = abs(cota_frente - cota_actual);

  // Comprobamos que el terreno sea seguro para andar
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
      return WALK;
    }
  }
  else
  {
    // Si hay muro, precipicio o mucho desnivel, nos giramos
    return TURN_SR;
  }
}

//_________________________________________________________
// Nivel 4
//_________________________________________________________

/**
 * @brief Comportamiento del ingeniero para el Nivel 4.
 * @param sensores Datos actuales de los sensores.
 * @return Acción a realizar.
 */

// Busca en todo el mapa dónde están las Plantas de Residuos U
vector<pair<int, int>> buscarPlantasResiduos(const vector<vector<unsigned char>> &mapaResultado)
{
  vector<pair<int, int>> plantas;
  for (int r = 0; r < mapaResultado.size(); ++r)
  {
    for (int c = 0; c < mapaResultado[0].size(); ++c)
    {
      if (mapaResultado[r][c] == 'U')
      {
        plantas.push_back({r, c});
      }
    }
  }
  return plantas;
}

// Calcula la distancia Manhattan a la Planta de residuos más cercana usamos esta ya que las tuberias no pueden colocarse diagonalmente
int calcularDistanciaManhattan(int f, int c, const vector<pair<int, int>> &plantas)
{
  int min_dist = 999999;
  for (const auto &p : plantas)
  {
    int dist = abs(f - p.first) + abs(c - p.second);
    if (dist < min_dist)
    {
      min_dist = dist;
    }
  }
  return min_dist;
}

// Devuelve lo que contamina poner un tubo en un tipo de terreno
int costeInstalacionTuberias(char terreno)
{
  if (terreno == 'A')
    return 50;
  if (terreno == 'H')
    return 45;
  if (terreno == 'S')
    return 25;
  if (terreno == 'C' || terreno == 'U')
    return 15;
  return 30; // Resto de terrenos
}

// Devuelve lo que contamina usar el pico -1 o la pala +1 (No hacer nada es 0)
int costeModificarTerreno(char terreno, int operacion)
{
  if (operacion == 1)
  { // RAISE (Elevar terreno)
    if (terreno == 'H')
      return 55;
    if (terreno == 'S')
      return 30;
    if (terreno == 'C' || terreno == 'U')
      return 10;
    return 40;
  }
  else if (operacion == -1)
  { // DIG (Excavar terreno)
    if (terreno == 'H')
      return 65;
    if (terreno == 'S')
      return 40;
    if (terreno == 'C' || terreno == 'U')
      return 25;
    return 50;
  }
  return 0; // Si operacion es 0 es decir no hacer nada esto cuesta 0
}

// Revisa que no intentemos modificar el agua, ni pasarnos de los límites de altura 1-9
bool esOperacionDeAlturaValida(char terreno, int alturaOriginal, int operacion)
{
  if (operacion == 1 && (terreno == 'A' || alturaOriginal >= 9))
    return false;
  if (operacion == -1 && (terreno == 'A' || alturaOriginal <= 1))
    return false;
  return true;
}

// Compruebamos la regla fundamental de las tuberias: El fluido solo avanza en llano o cayendo 1 nivel
bool fluidoValido(int alturaOrigen, int alturaDestino)
{
  return (alturaOrigen == alturaDestino) || (alturaOrigen == alturaDestino + 1); // Tambien lo podemos ver como // Origen - 1 == Destino
}

Action ComportamientoIngeniero::ComportamientoIngenieroNivel_4(Sensores sensores)
{
  // Si ya hemos generado el plan, nos quedamos quietos
  if (!listaCanalizacionTuberias.empty())
  {
    return IDLE;
  }

  // Localizamos la fuga (punto de partida) y los límites del mapa
  int start_f = sensores.BelPosF;
  int start_c = sensores.BelPosC;
  int filas = mapaResultado.size();
  int columnas = mapaResultado[0].size();

  // Control de seguridad pa que no pete si la Belkanita está fuera del mapa
  if (start_f < 0 || start_f >= filas || start_c < 0 || start_c >= columnas)
  {
    return IDLE;
  }

  // Todo esto son preparativos q necesitamos para aplicar A*

  // Calculamos cuanto es el gasto ecologico que podemos hacer
  int presupuesto_eco = sensores.max_ecologico - sensores.ecologico;
  vector<pair<int, int>> plantas = buscarPlantasResiduos(mapaResultado);

  priority_queue<NodoAStarN4> frontera; // Cola de prioridad para el algoritmo A*  // Aquí guardamos los nodos para el A*

  // Esta matriz la usamos para podar   //Es un array 3D: [filas][columnas][operacion].
  // Si llegamos a una casilla con la misma operación
  // pero habiendo gastado más que antes(mayor gasto ecologico), descartamos esa rama.
  vector<vector<vector<int>>> impacto_minimo(filas, vector<vector<int>>(columnas, vector<int>(3, 999999)));

  char terreno_inicio = mapaResultado[start_f][start_c];

  // Comprobacion de Seguridad antes de hacer nada que la fuga no este en una casilla no pisable
  if (terreno_inicio != 'M' && terreno_inicio != 'P' && terreno_inicio != 'B')
  {

    // Probamos las 3 operaciones a la casilla Pico (-1), Nada (0) y Pala (1)
    for (int operacion = -1; operacion <= 1; ++operacion)
    {

      int altura_inicial = mapaCotas[start_f][start_c];

      if (!esOperacionDeAlturaValida(terreno_inicio, altura_inicial, operacion)) // Si no es valida pasamos a la siguiente operacion
      {
        continue;
      }

      int coste_eco_inicial = costeModificarTerreno(terreno_inicio, operacion);

      // Si nos llega el presupuesto, creamos el primer nodo y lo metemos a la cola
      if (coste_eco_inicial <= presupuesto_eco)
      {
        impacto_minimo[start_f][start_c][operacion + 1] = coste_eco_inicial;

        NodoAStarN4 nodo_inicial;
        nodo_inicial.f = start_f;
        nodo_inicial.c = start_c;
        nodo_inicial.op = operacion;
        nodo_inicial.g = 0; // Aun no hemos puesto tuberias
        nodo_inicial.h = calcularDistanciaManhattan(start_f, start_c, plantas);
        nodo_inicial.eco = coste_eco_inicial;
        nodo_inicial.plan.push_back({start_f, start_c, operacion});

        frontera.push(nodo_inicial);
      }
    }
  }

  // Direcciones para movernos: Arriba, Abajo, Izquierda, Derecha (Sin diagonales no estan permiticas en la colocacion de las tuberias)
  int dir_filas[] = {-1, 1, 0, 0};
  int dir_cols[] = {0, 0, -1, 1};

  list<Paso> plan_ganador; // Aki es donde vamos a guardar el plan final

  // Bucle Principal del Algoritmo A*
  while (!frontera.empty())
  {

    // Sacamos el nodo con mejor prioridad (camino más corto y menos contaminante)
    NodoAStarN4 actual = frontera.top();
    frontera.pop();

    // Hemos tocado una Planta de residuos TERMINAMOS
    if (mapaResultado[actual.f][actual.c] == 'U')
    {
      plan_ganador = actual.plan;
      break; // El primero q llegaa a la meta U es la mejor solucion en A*
    }

    // Si una ruta anterior ya llegó aquí contaminando menos, desechamos esta
    if (actual.eco > impacto_minimo[actual.f][actual.c][actual.op + 1])
    {
      continue;
    }

    // Miramos a los 4 vecinos posibles (N, S, E, O)
    for (int i = 0; i < 4; ++i)
    {
      int vecino_f = actual.f + dir_filas[i];
      int vecino_c = actual.c + dir_cols[i];

      // Fuera del mapa o terreno prohibido NO SIRVE NOS LA SALTAMOS
      if (vecino_f < 0 || vecino_f >= filas || vecino_c < 0 || vecino_c >= columnas)
        continue;
      char terreno_vecino = mapaResultado[vecino_f][vecino_c];
      if (terreno_vecino == 'M' || terreno_vecino == 'P' || terreno_vecino == 'B' || terreno_vecino == '?') // PARA Q NO PETE NIVEL 6
        continue;

      // Probar las 3 operaciones en la casilla vecina (-1, 0, 1)
      for (int op_vecino = -1; op_vecino <= 1; ++op_vecino)
      {
        int altura_vecino_original = mapaCotas[vecino_f][vecino_c];

        if (!esOperacionDeAlturaValida(terreno_vecino, altura_vecino_original, op_vecino))
        {
          continue;
        }

        // Alturas finales de ambas casillas tras aplicar las OPERACIONES
        int altura_mia_final = mapaCotas[actual.f][actual.c] + actual.op;
        int altura_vecino_final = altura_vecino_original + op_vecino;

        // Si el fluido es correcto seguimos si no nada
        if (fluidoValido(altura_mia_final, altura_vecino_final))
        {

          // Una tubería exige 2 instalaciones (una en cada punta) IMPORTANTISISIMO + el coste de modificar el suelo
          int coste_tramo = costeInstalacionTuberias(mapaResultado[actual.f][actual.c]) + costeInstalacionTuberias(terreno_vecino) + costeModificarTerreno(terreno_vecino, op_vecino);

          int coste_eco_total = actual.eco + coste_tramo;

          // Si no nos salimos del presupuesto y mejoramos el récord de esa casilla
          if (coste_eco_total <= presupuesto_eco && coste_eco_total < impacto_minimo[vecino_f][vecino_c][op_vecino + 1])
          {

            impacto_minimo[vecino_f][vecino_c][op_vecino + 1] = coste_eco_total;

            // Creamos el nuevo nodo y lo guardamos
            NodoAStarN4 nuevo_nodo;
            nuevo_nodo.f = vecino_f;
            nuevo_nodo.c = vecino_c;
            nuevo_nodo.op = op_vecino;
            nuevo_nodo.g = actual.g + 1; // Le sumamos 1 tramo de tubería
            nuevo_nodo.h = calcularDistanciaManhattan(vecino_f, vecino_c, plantas);
            nuevo_nodo.eco = coste_eco_total;

            // Arrastramos el historial del plan y le sumamos el nuevo bloque
            nuevo_nodo.plan = actual.plan;
            nuevo_nodo.plan.push_back({vecino_f, vecino_c, op_vecino});

            frontera.push(nuevo_nodo);
          }
        }
      }
    }
  }

  // Enviar el plan ganador al simulador
  if (!plan_ganador.empty())
  {
    VisualizaRedTuberias(plan_ganador);
  }

  return IDLE;
}

//_________________________________________________________
// Nivel 5
//_________________________________________________________

/**
 * @brief Comportamiento del ingeniero para el Nivel 5.
 * @param sensores Datos actuales de los sensores.
 * @return Acción a realizar.
 */

// Movemos al ingeniero a una celda específica usando BFS
Action ComportamientoIngeniero::MoverA_Ingeniero(int f, int c, const Sensores &sensores)
{
  // Si nos chocamos, borramos el plan para recalcularlo
  if (sensores.choque)
    n5_plan_movimiento.clear();

  if (sensores.posF == f && sensores.posC == c)
    return IDLE;

  // Si no hay plan, lo generamos
  if (n5_plan_movimiento.empty())
  {
    EstadoI inicio, fin;
    inicio.site.f = sensores.posF;
    inicio.site.c = sensores.posC;
    inicio.site.brujula = sensores.rumbo;
    inicio.zapatillas = tiene_zapatillas;
    fin.site.f = f;
    fin.site.c = c;

    // Si el tecnico nos bloquea el paso lo marcamos como MURO ('M')
    // temporalmente en una copia del mapa para que el BFS lo rodee.
    vector<vector<unsigned char>> mapa_modificado = mapaResultado;
    if (n5_obs_f != -1 && n5_obs_c != -1)
    {
      mapa_modificado[n5_obs_f][n5_obs_c] = 'M';
    }

    n5_plan_movimiento = B_Anchura_Ingeniero(inicio, fin, mapa_modificado, mapaCotas);

    // Reset de los obstáculos temporales tras planificar
    n5_obs_f = -1;
    n5_obs_c = -1;
  }

  if (!n5_plan_movimiento.empty())
  {
    Action a = n5_plan_movimiento.front();

    if ((a == WALK || a == JUMP) && sensores.agentes[2] == 't')
    {
      n5_espera++;
      // Si llevamos más de 3 turnos esperando a que se quite, lo marcamos como muro y recalculamos ruta
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

    n5_espera = 0; // Si el camino esta libre reseteamos la espera
    n5_plan_movimiento.pop_front();
    return a;
  }
  return IDLE;
}

// Calcula hacia qué punto debemos mirar para ir de A a B
Orientacion ComportamientoIngeniero::GetDireccion(int f_origen, int c_origen, int f_destino, int c_destino)
{
  if (f_destino < f_origen)
    return norte;
  if (f_destino > f_origen)
    return sur;
  if (c_destino < c_origen)
    return oeste;
  if (c_destino > c_origen)
    return este;
  return norte;
}

// Decide si es más rápido girar a la derecha o a la izquierda para alcanzar una orientación
Action ComportamientoIngeniero::GetTurnAction(Orientacion actual, Orientacion deseada)
{
  int diff = (deseada - actual + 8) % 8;
  if (diff == 0)
    return IDLE;
  if (diff <= 4)
    return TURN_SR;
  else
    return TURN_SL;
}

Action ComportamientoIngeniero::ComportamientoIngenieroNivel_5(Sensores sensores)
{
  if (sensores.superficie[0] == 'D')
    tiene_zapatillas = true;

  switch (n5_estado)
  {

  case 0: // Planificamos todo
  {
    if (listaCanalizacionTuberias.empty())
      ComportamientoIngenieroNivel_4(sensores);
    if (!listaCanalizacionTuberias.empty())
    {
      n5_plan_tuberia.assign(listaCanalizacionTuberias.begin(), listaCanalizacionTuberias.end());
      n5_tramo = 0;
      n5_estado = 1;
    }
    return IDLE;
  }

  case 1: // Vamos al nodo a lo preparamos y llamamos al tecnico
  {
    if (n5_tramo + 1 >= (int)n5_plan_tuberia.size())
      return IDLE;

    Paso targetA = n5_plan_tuberia[n5_tramo];

    if (sensores.posF != targetA.fil || sensores.posC != targetA.col)
    {
      return MoverA_Ingeniero(targetA.fil, targetA.col, sensores);
    }

    if (targetA.op != 0)
    {
      Action operacion = (targetA.op == 1) ? RAISE : DIG;
      n5_plan_tuberia[n5_tramo].op = 0;
      return operacion;
    }

    n5_estado = 2;
    return COME; // Mandamos al Técnico a la casilla A (preparada)
  }

  case 2: // Vamos al nodo b y lo preparamos
  {
    Paso targetB = n5_plan_tuberia[n5_tramo + 1];

    if (sensores.posF != targetB.fil || sensores.posC != targetB.col)
    {
      return MoverA_Ingeniero(targetB.fil, targetB.col, sensores);
    }

    if (targetB.op != 0)
    {
      Action operacion = (targetB.op == 1) ? RAISE : DIG;
      n5_plan_tuberia[n5_tramo + 1].op = 0;
      return operacion;
    }

    n5_estado = 3;
    return IDLE;
  }

  case 3: // Miramos a la casilla a nos sincronizamos con el tecnico e instalamos
  {
    Paso targetA = n5_plan_tuberia[n5_tramo];
    Orientacion deseada = GetDireccion(sensores.posF, sensores.posC, targetA.fil, targetA.col);

    if (sensores.rumbo != deseada)
      return GetTurnAction(sensores.rumbo, deseada);

    if (sensores.enfrente)
    {
      n5_tramo++;
      n5_estado = 1;
      return INSTALL;
    }

    return IDLE;
  }
  }
  return IDLE;
}

//_________________________________________________________
// Nivel 6
//_________________________________________________________

/**
 * @brief Comportamiento del ingeniero para el Nivel 6.
 * @param sensores Datos actuales de los sensores.
 * @return Acción a realizar.
 */
Action ComportamientoIngeniero::ComportamientoIngenieroNivel_6(Sensores sensores)
{

  n6 = true; // Para en el nivel 1 ir por hierva

  ActualizarMapa(sensores); // Para ir descubriendo el mapa mientras avanzamos

  if (sensores.superficie[0] == 'D')
    tiene_zapatillas = true;

  // Guardamos si ya hemos alcanzado la fuga alguna vez
  if (sensores.posF == sensores.BelPosF && sensores.posC == sensores.BelPosC)
  {
    ha_llegado_fuga = true;
  }

  // Vamos directos a la fuga (Usando el Nivel 2)
  if (!ha_llegado_fuga)
  {
    bool abortar_plan = false;

    // A) Choque físico
    if (sensores.choque)
    {
      abortar_plan = true;
    }

    // Detección visual antes de moverse para comprobar que el movimiento es valido
    // En caso que no sea valido recalculamos el camino con los nuevos datos que tenemos
    if (hayPlan && !plan.empty())
    {
      Action accion_siguiente = plan.front();
      int max_desnivel = tiene_zapatillas ? 2 : 1;

      if (accion_siguiente == WALK)
      {
        char suelo_frente = sensores.superficie[2];
        int desnivel = abs(sensores.cota[2] - sensores.cota[0]);

        if (suelo_frente == 'M' || suelo_frente == 'P' || suelo_frente == 'B' || desnivel > max_desnivel)
        {
          abortar_plan = true;
        }
      }
      else if (accion_siguiente == JUMP)
      {
        char suelo_intermedio = sensores.superficie[2];
        char suelo_final = sensores.superficie[6];
        int desnivel = abs(sensores.cota[6] - sensores.cota[0]);

        if (suelo_intermedio == 'M' || suelo_intermedio == 'P' || suelo_intermedio == 'B' ||
            suelo_final == 'M' || suelo_final == 'P' || suelo_final == 'B' || desnivel > max_desnivel)
        {
          abortar_plan = true;
        }
      }
    }

    // Si detectamos peligro inminente, borramos el plan
    if (abortar_plan)
    {
      plan.clear();
      hayPlan = false;
    }

    // Ahora que sabemos que el plan es seguro (o que no hay plan) llamamos al Nivel 2
    Action a = ComportamientoIngenieroNivel_2(sensores);

    // Si el Nivel 2 no encuentra camino (devuelve IDLE y no hay plan)
    // significa que estamos bloqueados. Usamos el Nivel 1 para salir.
    if (a == IDLE && !hayPlan)
    {
      return ComportamientoIngenieroNivel_1(sensores);
    }

    return a;
  }

  // Ya hemos estado en la fuga 
  else
  {
    // Buscamos si ya hemos visto la planta de residuos 'U' en el mapa
    bool planta_encontrada = false;
    for (int r = 0; r < mapaResultado.size(); ++r)
    {
      for (int c = 0; c < mapaResultado[0].size(); ++c)
      {
        if (mapaResultado[r][c] == 'U')
        {
          planta_encontrada = true;
          break;
        }
      }
      if (planta_encontrada)
        break;
    }

    // Si sabemos dónde está la planta, intentamos trazar la red de tuberías (Nivel 4)
    if (planta_encontrada)
    {

      if (listaCanalizacionTuberias.empty())
      {
        ComportamientoIngenieroNivel_4(sensores);
      }

      // Si el Nivel 4 logró hacer el plan, construimos con el Nivel 5
      if (!listaCanalizacionTuberias.empty())
      {
        return ComportamientoIngenieroNivel_5(sensores);
      }
    }

    // Si no sabemos dónde está la planta, o el Nivel 4 falló
    // nos ponemos a explorar el mapa usando el Nivel 1
    return ComportamientoIngenieroNivel_1(sensores);
  }
}

// =========================================================================
// FUNCIONES PROPORCIONADAS
// =========================================================================

/**
 * @brief Actualiza el mapaResultado y mapaCotas con la información de los sensores.
 * @param sensores Datos actuales de los sensores.
 */
void ComportamientoIngeniero::ActualizarMapa(Sensores sensores)
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
 * @brief Determina si una casilla es transitable para el ingeniero.
 * @param f Fila de la casilla.
 * @param c Columna de la casilla.
 * @param tieneZapatillas Indica si el agente posee las zapatillas.
 * @return true si la casilla es transitable (no es muro ni precipicio).
 */
bool ComportamientoIngeniero::EsCasillaTransitableLevel0(int f, int c, bool tieneZapatillas)
{
  if (f < 0 || f >= mapaResultado.size() || c < 0 || c >= mapaResultado[0].size())
    return false;
  return es_camino(mapaResultado[f][c]); // Solo 'C', 'D', 'U' son transitables en Nivel 0
}

/**
 * @brief Comprueba si la casilla de delante es accesible por diferencia de altura.
 * Para el ingeniero: desnivel máximo 1 sin zapatillas, 2 con zapatillas.
 * @param actual Estado actual del agente (fila, columna, orientacion, zap).
 * @return true si el desnivel con la casilla de delante es admisible.
 */
bool ComportamientoIngeniero::EsAccesiblePorAltura(const ubicacion &actual, bool zap)
{
  ubicacion del = Delante(actual);
  if (del.f < 0 || del.f >= mapaCotas.size() || del.c < 0 || del.c >= mapaCotas[0].size())
    return false;
  int desnivel = abs(mapaCotas[del.f][del.c] - mapaCotas[actual.f][actual.c]);
  if (zap && desnivel > 2)
    return false;
  if (!zap && desnivel > 1)
    return false;
  return true;
}

/**
 * @brief Devuelve la posición (fila, columna) de la casilla que hay delante del agente.
 * Calcula la casilla frontal según la orientación actual (8 direcciones).
 * @param actual Estado actual del agente (fila, columna, orientacion).
 * @return Estado con la fila y columna de la casilla de enfrente.
 */
ubicacion ComportamientoIngeniero::Delante(const ubicacion &actual) const
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
void ComportamientoIngeniero::PintaPlan(const list<Action> &plan)
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
 * @brief Imprime las coordenadas y operaciones de un plan de tubería.
 *
 * @param plan  Lista de pasos (fila, columna, operación),
 *              donde operacion = -1 (DIG), operación = 1 (RAISE).
 */
void ComportamientoIngeniero::PintaPlan(const list<Paso> &plan)
{
  auto it = plan.begin();
  while (it != plan.end())
  {
    cout << it->fil << ", " << it->col << " (" << it->op << ")\n";
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
void ComportamientoIngeniero::VisualizaPlan(const ubicacion &st,
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

/**
 * @brief Convierte un plan de tubería en la lista de casillas usada
 *        por el sistema de visualización.
 *
 * @param st    Estado de partida (no utilizado directamente).
 * @param plan  Lista de pasos del plan de tubería.
 */
void ComportamientoIngeniero::VisualizaRedTuberias(const list<Paso> &plan)
{
  listaCanalizacionTuberias.clear();
  auto it = plan.begin();
  while (it != plan.end())
  {
    listaCanalizacionTuberias.push_back({it->fil, it->col, it->op});
    it++;
  }
}
