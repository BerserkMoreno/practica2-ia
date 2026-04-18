#include "ingeniero.hpp"
#include "motorlib/util.h"
#include <iostream>
#include <queue>
#include <set>

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



/**
 * @brief Guarda el momento exacto (instante) en el que pisamos esta casilla.
 */
void ComportamientoIngeniero::RegistrarPasoEnElReloj(ubicacion actual) {
  
    mapaUltimoPaso[actual.f][actual.c] = instanteActual;
}

/**
 * @brief Comprueba si una casilla es suelo firme y no hay nadie estorbando.
 */
bool ComportamientoIngeniero::EsCasillaDespejada(ubicacion destino, int idx_sensor, const Sensores& sensores) {


  
    // Usamos la función de la UGR para ver si el suelo es 'C', 'D' o 'U'
    if (!EsCasillaTransitableLevel0(destino.f, destino.c, tiene_zapatillas)) return false;
    
    // Miramos si hay un aldeano o el técnico en esa casilla
    if (sensores.agentes[idx_sensor] != '_') return false; //Con esto marcamos que la casilla esta desocupada _ por tanto miramos si no esta desocupada
    
    return true;
}

/**
 * @brief Evalúa si el Ingeniero puede avanzar un paso (WALK).
 */
bool ComportamientoIngeniero::SePuedeCaminar(ubicacion origen, int idx_sensor, const Sensores& sensores) {
    ubicacion destino = Delante(origen);

    if (!EsCasillaDespejada(destino, idx_sensor, sensores)) return false;
    
    // Comprobamos ley de altura (1 o 2 dependiendo de zapatillas)
    if (!EsAccesiblePorAltura(origen, tiene_zapatillas)) return false;

    return true;
}

/**
 * @brief Evalúa si el Ingeniero puede dar un salto largo (JUMP).
 */
bool ComportamientoIngeniero::SePuedeSaltar(ubicacion origen, int idx_intermedio, int idx_destino, const Sensores& sensores) {
    ubicacion ubi_intermedia = Delante(origen);
    ubicacion ubi_destino = Delante(ubi_intermedia);

    // 1. Casilla intermedia: pisable, vacía y no más alta que nosotros
    if (!EsCasillaDespejada(ubi_intermedia, idx_intermedio, sensores)) return false;
    if (mapaCotas[ubi_intermedia.f][ubi_intermedia.c] > mapaCotas[origen.f][origen.c]) return false;

    // 2. Casilla destino: pisable y vacía
    if (!EsCasillaDespejada(ubi_destino, idx_destino, sensores)) return false;
    
    // 3. Altura final
    int desnivel = abs(mapaCotas[ubi_destino.f][ubi_destino.c] - mapaCotas[origen.f][origen.c]);
    int max_salto = tiene_zapatillas ? 2 : 1;
    
    return (desnivel <= max_salto);
}

// =========================================================================
// 2. LÓGICA DE PUNTUACIÓN (¿Qué camino me gusta más?)
// =========================================================================

/**
 * @brief Calcula el interés de una casilla basándose en la "frescura" (hace cuánto no vamos).
 */
int ComportamientoIngeniero::CalcularInteres(ubicacion ubi_destino, char terreno) {
    int puntos = 1000; // Nota base (más alta para manejar mejor las restas)

    if (terreno == 'U') puntos += 10000;
    else if (terreno == 'D' && !tiene_zapatillas) puntos += 5000;
    
    // LÓGICA DE INSTANTES: Restamos el instante de la última visita.
    // Una casilla visitada en el paso 10 restará solo 10 (Puntos altos = Interesante).
    // Una casilla visitada en el paso 500 restará 500 (Puntos bajos = Aburrida).
    puntos -= mapaUltimoPaso[ubi_destino.f][ubi_destino.c];
    
    return puntos;
}

/**
 * @brief Compara en una dirección si es mejor Caminar o Saltar.
 */
void ComportamientoIngeniero::EvaluarRuta(ubicacion mirada, int idx_W, int idx_J, const Sensores& sensores, int& mejor_puntos, bool& debo_saltar) {
    mejor_puntos = -999999;
    debo_saltar = false;

    ubicacion ubi_cerca = Delante(mirada);
    ubicacion ubi_lejos = Delante(ubi_cerca);

    int puntos_W = -999999;
    int puntos_J = -999999;

    if (SePuedeCaminar(mirada, idx_W, sensores)) {
        puntos_W = CalcularInteres(ubi_cerca, sensores.superficie[idx_W]);
    }

    if (SePuedeSaltar(mirada, idx_W, idx_J, sensores)) {
        puntos_J = CalcularInteres(ubi_lejos, sensores.superficie[idx_J]);
        
        // Si nunca hemos pisado la casilla intermedia, penalizamos el salto para explorar.
        if (mapaUltimoPaso[ubi_cerca.f][ubi_cerca.c] == 0) puntos_J -= 200;
        else puntos_J += 50; 
    }

    if (puntos_J > puntos_W) {
        mejor_puntos = puntos_J;
        debo_saltar = true;
    } else {
        mejor_puntos = puntos_W;
        debo_saltar = false;
    }
}

// =========================================================================
// 3. CEREBRO PRINCIPAL
// =========================================================================


Action ComportamientoIngeniero::ComportamientoIngenieroNivel_0(Sensores sensores)
{
    instanteActual++; // El tiempo vuela
    ActualizarMapa(sensores);
    
    ubicacion yo = {sensores.posF, sensores.posC, sensores.rumbo};
    RegistrarPasoEnElReloj(yo);

    if (sensores.superficie[0] == 'D') tiene_zapatillas = true;
    if (sensores.superficie[0] == 'U') return IDLE;

    // Miradas virtuales
    ubicacion frente = yo;
    ubicacion izq = yo; izq.brujula = (Orientacion)((yo.brujula + 7) % 8);
    ubicacion der = yo; der.brujula = (Orientacion)((yo.brujula + 1) % 8);

    // Evaluamos las 3 direcciones
    int pts_F, pts_I, pts_D;
    bool saltar_F, saltar_I, saltar_D;

    EvaluarRuta(frente, 2, 6, sensores, pts_F, saltar_F);
    EvaluarRuta(izq,    1, 4, sensores, pts_I, saltar_I);
    EvaluarRuta(der,    3, 8, sensores, pts_D, saltar_D);

    // Torneo de puntuaciones
    int ganador = pts_F;
    if (pts_I > ganador) ganador = pts_I;
    if (pts_D > ganador) ganador = pts_D;

    // Decisión final
    if (ganador <= -999999) return TURN_SL;

    if (ganador == pts_F) return saltar_F ? JUMP : WALK;
    if (ganador == pts_I) return TURN_SL;
    
    return TURN_SR;
}


/*Antiguo 16 Niveles El tecnico le roba casilla
void ComportamientoIngeniero::RegistrarVisita(ubicacion actual) {
    if (mapaVisitas.empty()) {
        mapaVisitas.assign(mapaResultado.size(), vector<int>(mapaResultado[0].size(), 0));
    }
    mapaVisitas[actual.f][actual.c]++;
}


bool ComportamientoIngeniero::EsCaminoLimpio(ubicacion destino, int idx_sensor, const Sensores& sensores) {
    // Usamos tu función proporcionada para ver si es 'C', 'D' o 'U'
    if (!EsCasillaTransitableLevel0(destino.f, destino.c, tiene_zapatillas)) return false;
    
    // Comprobamos que no haya obstáculos móviles ('t' aldeano o 'i' ingeniero)
    if (sensores.agentes[idx_sensor] != '_') return false;
    
    return true;
}


bool ComportamientoIngeniero::SePuedeCaminar(ubicacion origen, int idx_sensor, const Sensores& sensores) {
    ubicacion destino = Delante(origen); // Función proporcionada

    if (!EsCaminoLimpio(destino, idx_sensor, sensores)) return false;
    
    // Usamos tu función proporcionada para comprobar las reglas estrictas de altura
    if (!EsAccesiblePorAltura(origen, tiene_zapatillas)) return false;

    return true;
}


bool ComportamientoIngeniero::SePuedeSaltar(ubicacion origen, int idx_int, int idx_dest, const Sensores& sensores) {
    ubicacion intermedia = Delante(origen);
    ubicacion destino = Delante(intermedia);

    // 1. La casilla que sobrevolamos debe ser transitable, vacía y no ser un muro alto
    if (!EsCaminoLimpio(intermedia, idx_int, sensores)) return false;
    if (mapaCotas[intermedia.f][intermedia.c] > mapaCotas[origen.f][origen.c]) return false; // Chocaríamos

    // 2. La casilla donde aterrizamos debe ser transitable, vacía y el desnivel total legal
    if (!EsCaminoLimpio(destino, idx_dest, sensores)) return false;
    
    int desnivel_total = abs(mapaCotas[destino.f][destino.c] - mapaCotas[origen.f][origen.c]);
    int limite_altura = tiene_zapatillas ? 2 : 1;
    if (desnivel_total > limite_altura) return false;

    return true;
}


int ComportamientoIngeniero::PuntuarOpcion(ubicacion destino, char terreno) {
    int puntos = 100; // Valor base por ser un camino

    if (terreno == 'U') puntos = 10000;
    else if (terreno == 'D' && !tiene_zapatillas) puntos = 5000;
    
    // Restamos 15 puntos por cada vez que hayamos pasado por aquí
    if (destino.f >= 0 && destino.f < mapaVisitas.size() && destino.c >= 0 && destino.c < mapaVisitas[0].size()) {
        puntos -= (mapaVisitas[destino.f][destino.c] * 15);
    }
    
    return puntos;
}


void ComportamientoIngeniero::EvaluarLado(ubicacion origen, int idx_walk, int idx_jump, const Sensores& sensores, int& mejor_nota, bool& prefiere_saltar) {
    mejor_nota = -9999;
    prefiere_saltar = false;

    ubicacion ubi_walk = Delante(origen);
    ubicacion ubi_jump = Delante(ubi_walk);

    int puntos_caminar = -9999;
    int puntos_saltar = -9999;

    if (SePuedeCaminar(origen, idx_walk, sensores)) {
        puntos_caminar = PuntuarOpcion(ubi_walk, sensores.superficie[idx_walk]);
    }

    if (SePuedeSaltar(origen, idx_walk, idx_jump, sensores)) {
        puntos_saltar = PuntuarOpcion(ubi_jump, sensores.superficie[idx_jump]);
        
        // Estrategia: ¿Conviene saltar?
        // Si el suelo que vamos a saltar no lo hemos pisado NUNCA, preferimos caminar para explorarlo.
        if (mapaVisitas[ubi_walk.f][ubi_walk.c] == 0) puntos_saltar -= 200;
        else puntos_saltar += 50; // Si ya lo pisamos, preferimos saltar para ir más rápido
    }

    if (puntos_saltar > puntos_caminar) {
        mejor_nota = puntos_saltar;
        prefiere_saltar = true;
    } else {
        mejor_nota = puntos_caminar;
        prefiere_saltar = false;
    }
}


// =========================================================================
// 2. EL CEREBRO PRINCIPAL
// =========================================================================

Action ComportamientoIngeniero::ComportamientoIngenieroNivel_0(Sensores sensores)
{
    // 1. Actualizar Entorno y Memoria
    ActualizarMapa(sensores); // Función proporcionada por la UGR
    
    ubicacion posicion_actual = {sensores.posF, sensores.posC, sensores.rumbo};
    RegistrarVisita(posicion_actual);

    // 2. Actualizar Objetivos
    if (sensores.superficie[0] == 'D') tiene_zapatillas = true;
    if (sensores.superficie[0] == 'U') return IDLE; // Victoria

    // 3. Crear coordenadas "virtuales" apuntando hacia donde miran nuestros sensores
    ubicacion mirar_frente = posicion_actual;
    
    ubicacion mirar_izq = posicion_actual; 
    mirar_izq.brujula = (Orientacion)((posicion_actual.brujula + 7) % 8); // Girar -45 grados
    
    ubicacion mirar_der = posicion_actual; 
    mirar_der.brujula = (Orientacion)((posicion_actual.brujula + 1) % 8); // Girar +45 grados

    // 4. Evaluar las 3 direcciones (Frontal, Izquierda, Derecha)
    int nota_frente, nota_izq, nota_der;
    bool salta_frente, salta_izq, salta_der;

    // Pasamos el índice del sensor de caminar y el del salto
    EvaluarLado(mirar_frente, 2, 6, sensores, nota_frente, salta_frente);
    EvaluarLado(mirar_izq,    1, 4, sensores, nota_izq,    salta_izq);
    EvaluarLado(mirar_der,    3, 8, sensores, nota_der,    salta_der);

    // 5. Comparar notas y decidir la mejor acción
    int ganador = nota_frente;
    if (nota_izq > ganador) ganador = nota_izq;
    if (nota_der > ganador) ganador = nota_der;

    Action accion_elegida = IDLE;

    if (ganador <= -9999) {
        // Bloqueo total: Giramos a la izquierda para escanear nuevas áreas
        accion_elegida = TURN_SL;
    } 
    else if (ganador == nota_frente) {
        // El camino recto es el mejor
        accion_elegida = salta_frente ? JUMP : WALK;
    } 
    else if (ganador == nota_izq) {
        // Es mejor ir por la izquierda (giramos primero)
        accion_elegida = TURN_SL;
    } 
    else {
        // Es mejor ir por la derecha (giramos primero)
        accion_elegida = TURN_SR;
    }

    last_action = accion_elegida;
    return accion_elegida;
}
*/




/*
//17 NIVEELS PASADOS 

// =========================================================================
// FUNCIONES AUXILIARES PARA EL NIVEL 0 
// =========================================================================


static bool EsAccesibleWalk(const Sensores& sensores, int dest_idx, bool zap) {
    char sup = sensores.superficie[dest_idx];
    char ag = sensores.agentes[dest_idx];
    int cota_origen = sensores.cota[0];
    int cota_dest = sensores.cota[dest_idx];

    // Transitable y sin obstaculos
    if (sup != 'C' && sup != 'D' && sup != 'U') return false;
    if (ag == 't' || ag == 'i') return false;
    
    // Reglas de altura
    int dif = abs(cota_dest - cota_origen);
    if (zap) return dif <= 2;
    return dif <= 1;
}


static bool EsAccesibleJump(const Sensores& sensores, int int_idx, int dest_idx, bool zap) {
    char sup_int = sensores.superficie[int_idx];
    char ag_int = sensores.agentes[int_idx];
    int cota_int = sensores.cota[int_idx];

    char sup_dest = sensores.superficie[dest_idx];
    char ag_dest = sensores.agentes[dest_idx];
    int cota_origen = sensores.cota[0];
    int cota_dest = sensores.cota[dest_idx];

    // Reglas de la casilla intermedia (Debe ser transitable, libre y no superar la altura origen)
    if (sup_int != 'C' && sup_int != 'D' && sup_int != 'U') return false;
    if (ag_int != '_') return false;
    if (cota_int > cota_origen) return false;

    // Reglas de la casilla destino
    if (sup_dest != 'C' && sup_dest != 'D' && sup_dest != 'U') return false;
    if (ag_dest != '_') return false; 
    
    // Desnivel hasta el destino
    int dif = abs(cota_dest - cota_origen);
    if (zap) return dif <= 2;
    return dif <= 1;
}


static float PuntuacionCasilla(int idx, const ubicacion& ub, const Sensores& sensores, const vector<vector<int> >& mapaVisitas, bool zap) {
    // Si la ubicación se sale de nuestro mapa conocido (por seguridad)
    if (ub.f < 0 || ub.f >= (int)mapaVisitas.size() || ub.c < 0 || ub.c >= (int)mapaVisitas[0].size()) {
        return -9999.0f;
    }
    
    char sup = sensores.superficie[idx];
    float valor = 100.0f; // Puntuación base
    
    // Preferencias fuertes
    if (sup == 'U') valor = 10000.0f;
    else if (sup == 'D' && !zap) valor = 5000.0f;
    
    // Penalizamos las veces que hayamos pisado esa casilla para forzar la exploración
    valor -= (mapaVisitas[ub.f][ub.c] * 15.0f);
    
    return valor;
}



Action ComportamientoIngeniero::ComportamientoIngenieroNivel_0(Sensores sensores)
{
  ActualizarMapa(sensores);

  // Asegurarnos de que mapaVisitas tiene el tamaño adecuado para evitar errores
  if (mapaVisitas.empty() || mapaVisitas.size() != mapaResultado.size() || mapaVisitas[0].size() != mapaResultado[0].size()) {
      mapaVisitas.assign(mapaResultado.size(), vector<int>(mapaResultado[0].size(), 0));
  }

  // Registramos que hemos estado en la casilla actual
  mapaVisitas[sensores.posF][sensores.posC]++;

  if (sensores.superficie[0] == 'D')
    tiene_zapatillas = true;

  if (sensores.superficie[0] == 'U')
    return IDLE; // Objetivo conseguido, nos quedamos quietos

  // --- 1. Calcular Coordenadas (ubicacion) de los índices de visión clave ---
  ubicacion ub_0 = {sensores.posF, sensores.posC, sensores.rumbo};
  
  // Línea Central (Índices 2 y 6)
  ubicacion ub_2 = Delante(ub_0);
  ubicacion ub_6 = Delante(ub_2);
  
  // Línea Izquierda (Índices 1 y 4)
  ubicacion ub_izq_dir = ub_0; 
  ub_izq_dir.brujula = (Orientacion)((ub_0.brujula + 7) % 8); // Girar -45 grados (izq)
  ubicacion ub_1 = Delante(ub_izq_dir);
  ubicacion ub_4 = Delante(ub_1);
  
  // Línea Derecha (Índices 3 y 8)
  ubicacion ub_der_dir = ub_0;
  ub_der_dir.brujula = (Orientacion)((ub_0.brujula + 1) % 8); // Girar +45 grados (der)
  ubicacion ub_3 = Delante(ub_der_dir);
  ubicacion ub_8 = Delante(ub_3);

  // --- 2. Evaluar opciones en cada dirección ---

  // Evaluación de la ruta FRENTE (Centro)
  float score_W2 = -9999.0f;
  if (EsAccesibleWalk(sensores, 2, tiene_zapatillas)) score_W2 = PuntuacionCasilla(2, ub_2, sensores, mapaVisitas, tiene_zapatillas);
  
  float score_J6 = -9999.0f;
  if (EsAccesibleJump(sensores, 2, 6, tiene_zapatillas)) {
      score_J6 = PuntuacionCasilla(6, ub_6, sensores, mapaVisitas, tiene_zapatillas);
      // Lógica de Exploración: Si la casilla 2 no se ha pisado nunca, preferimos caminar.
      if (mapaVisitas[ub_2.f][ub_2.c] == 0) score_J6 -= 200.0f;
      else score_J6 += 50.0f; // Si ya se pisó, premiamos saltar para ahorrar tiempo.
  }

  // Evaluación de la ruta IZQUIERDA
  float score_W1 = -9999.0f;
  if (EsAccesibleWalk(sensores, 1, tiene_zapatillas)) score_W1 = PuntuacionCasilla(1, ub_1, sensores, mapaVisitas, tiene_zapatillas);
  
  float score_J4 = -9999.0f;
  if (EsAccesibleJump(sensores, 1, 4, tiene_zapatillas)) {
      score_J4 = PuntuacionCasilla(4, ub_4, sensores, mapaVisitas, tiene_zapatillas);
      if (mapaVisitas[ub_1.f][ub_1.c] == 0) score_J4 -= 200.0f;
      else score_J4 += 50.0f;
  }

  // Evaluación de la ruta DERECHA
  float score_W3 = -9999.0f;
  if (EsAccesibleWalk(sensores, 3, tiene_zapatillas)) score_W3 = PuntuacionCasilla(3, ub_3, sensores, mapaVisitas, tiene_zapatillas);
  
  float score_J8 = -9999.0f;
  if (EsAccesibleJump(sensores, 3, 8, tiene_zapatillas)) {
      score_J8 = PuntuacionCasilla(8, ub_8, sensores, mapaVisitas, tiene_zapatillas);
      if (mapaVisitas[ub_3.f][ub_3.c] == 0) score_J8 -= 200.0f;
      else score_J8 += 50.0f;
  }

  // --- 3. Encontrar la mejor dirección general ---
  float max_frente = (score_W2 > score_J6) ? score_W2 : score_J6;
  float max_izq    = (score_W1 > score_J4) ? score_W1 : score_J4;
  float max_der    = (score_W3 > score_J8) ? score_W3 : score_J8;

  float max_overall = max_frente;
  if (max_izq > max_overall) max_overall = max_izq;
  if (max_der > max_overall) max_overall = max_der;

  // --- 4. Tomar decisión final ---
  Action accion = IDLE;

  if (max_overall <= -9999.0f) {
      // Callejón sin salida aparente o ruta bloqueada, rotamos para explorar nueva visión
      accion = TURN_SL; 
  } else if (max_overall == max_frente) {
      // El mejor camino está directamente delante de nosotros
      if (score_J6 > score_W2) accion = JUMP;
      else accion = WALK;
  } else if (max_overall == max_izq) {
      // El mejor camino está a la izquierda, rotamos primero
      accion = TURN_SL;
  } else {
      // El mejor camino está a la derecha, rotamos primero
      accion = TURN_SR;
  }

  last_action = accion;
  return accion;
}
*/




































/* VeoCasillaInteresanteI del pdf tutorial lvl0
int VeoCasillaInteresanteI(char i, char c, char d, bool zap)
{
  if (c == 'U')
    return 2;
  else if (i == 'U')
    return 1;
  else if (d == 'U')
    return 3;
  else if (!zap)
  {
    if (c == 'D')
      return 2;
    else if (i == 'D')
      return 1;
    else if (d == 'D')
      return 3;
  }

  if (c == 'C')
    return 2;
  else if (i == 'C')
    return 1;
  else if (d == 'C')
    return 3;
  else
    return 0;
}
*/




/*


bool PuedoSaltar(Sensores sensores) {
  // 1. Necesitamos saber la posición de la casilla a distancia 1 y distancia 2
  ubicacion actual = {sensores.posF, sensores.posC, (Orientacion)sensores.rumbo};
  
  ubicacion casilla1 = Delante(actual);
  ubicacion casilla2 = Delante(casilla1); // Delante de la anterior

  // 2. Verificar que no se salgan del mapa
  if (casilla2.f < 0 || casilla2.f >= mapaResultado.size() || 
      casilla2.c < 0 || casilla2.c >= mapaResultado[0].size()) {
    return false;
  }

  // 3. Obtener tipos de terreno y alturas
  char t1 = mapaResultado[casilla1.f][casilla1.c];
  char t2 = mapaResultado[casilla2.f][casilla2.c];
  int h0 = sensores.cota[0];
  int h1 = mapaCotas[casilla1.f][casilla1.c];
  int h2 = mapaCotas[casilla2.f][casilla2.c];

  // 4. Reglas de Transitabilidad (No saltar sobre muros o precipicios)
  // Nota: es_camino es tu función que acepta 'C', 'D', 'U'
  if (!es_camino(t1) || !es_camino(t2)) return false;

  // 5. Reglas de Altura (Crucial para el Ingeniero)
  // El salto requiere que AMBOS pasos sean válidos por altura
  int limite = tiene_zapatillas ? 2 : 1;
  
  bool paso1_ok = (abs(h1 - h0) <= limite);
  bool paso2_ok = (abs(h2 - h1) <= limite);

  // 6. Verificar que no haya agentes estorbando (Técnicos o Aldeanos)
  // En sensores.agentes, el índice 2 es frente dist 1, el 6 es frente dist 2
  if (sensores.agentes[2] != '_' || sensores.agentes[6] != '_') return false;

  return (paso1_ok && paso2_ok);
}


*/



/*



// Función auxiliar para darle una nota a cada casilla (El cerebro reactivo)
int PuntuacionCasilla(char terreno, int visitas, bool zap_necesaria)
{
  if (terreno == 'P') return -9999; // Precipicio, compañero o desnivel insalvable
  
  int puntos = 0;
  if (terreno == 'U') puntos = 10000; // La meta absoluta
  else if (terreno == 'D' && !zap_necesaria) puntos = 5000; // ¡Zapatillas!
  else if (terreno == 'C' || terreno == 'S' || (terreno == 'D' && zap_necesaria)) puntos = 1000; // Transitable normal
  else return -99999; // Muros, agua o árboles (si somos ingeniero)

  // La magia: le restamos atractivo por cada vez que la hemos pisado
  puntos -= (visitas * 100); 
  
  return puntos;
}

// Evaluamos frente, izquierda y derecha devolviendo 1 (izq), 2 (frente), 3 (der) o 0 (nada)
int VeoCasillaInteresanteI(char i, char c, char d, int vi, int vc, int vd, bool zap)
{
  int score_i = PuntuacionCasilla(i, vi, zap);
  int score_c = PuntuacionCasilla(c, vc, zap);
  int score_d = PuntuacionCasilla(d, vd, zap);

  // Si todas son malas, girar
  if (score_i == -9999 && score_c == -9999 && score_d == -9999) return 0;

  // Priorizamos la mayor puntuación (en caso de empate, frente > izq > der)
  if (score_c >= score_i && score_c >= score_d) return 2;
  if (score_i >= score_d) return 1;
  return 3;
}
*/


/*Version de superacion de 15 test
Action ComportamientoIngeniero::ComportamientoIngenieroNivel_0(Sensores sensores)
{
  Action accion = IDLE;
  ActualizarMapa(sensores);

  // 1. Apuntamos en nuestra memoria que hemos pisado esta casilla (Aumentamos visitas)
  mapaVisitas[sensores.posF][sensores.posC]++;

  if (sensores.superficie[0] == 'D')
    tiene_zapatillas = true;

  if (sensores.superficie[0] == 'U') {
    return IDLE; // Nivel completado
  }

  // 2. Extraer viabilidad por altura (terreno vs precipicio 'P')
  char i = ViablePorAlturaI(sensores.superficie[1], sensores.cota[1] - sensores.cota[0], tiene_zapatillas);
  char c = ViablePorAlturaI(sensores.superficie[2], sensores.cota[2] - sensores.cota[0], tiene_zapatillas);
  char d = ViablePorAlturaI(sensores.superficie[3], sensores.cota[3] - sensores.cota[0], tiene_zapatillas);

  // 3. Prevenir atropellar al técnico (si lo vemos de frente/lados, es un obstáculo)
  if (sensores.agentes[1] == 't') i = 'P';
  if (sensores.agentes[2] == 't') c = 'P';
  if (sensores.agentes[3] == 't') d = 'P';

  // 4. Calcular coordenadas exactas de las casillas a mi Izq, Frente y Der
  ubicacion actual;
  actual.f = sensores.posF;
  actual.c = sensores.posC;
  actual.brujula = (Orientacion)sensores.rumbo;

  ubicacion izq = actual;
  izq.brujula = (Orientacion)((actual.brujula + 7) % 8); 
  ubicacion c_izq = Delante(izq);

  ubicacion cen = actual;
  ubicacion c_cen = Delante(cen);

  ubicacion der = actual;
  der.brujula = (Orientacion)((actual.brujula + 1) % 8); 
  ubicacion c_der = Delante(der);

  // 5. Mirar nuestro "mapa de visitas" en esas coordenadas
  int vi = ObtenerVisitasSeguro(c_izq.f, c_izq.c, mapaVisitas);
  int vc = ObtenerVisitasSeguro(c_cen.f, c_cen.c, mapaVisitas);
  int vd = ObtenerVisitasSeguro(c_der.f, c_der.c, mapaVisitas);

  // 6. Decidir la mejor dirección combinando Terreno + Visitas
  int pos = VeoCasillaInteresanteI(i, c, d, vi, vc, vd, tiene_zapatillas);

  switch (pos)
  {
  case 2:
    accion = WALK;
    break;
  case 1:
    accion = TURN_SL;
    break;
  case 3:
    accion = TURN_SR;
    break;
  default:
    // Si estamos atrapados o todo es muro/precipicio, giramos
    accion = TURN_SL;
    break;
  }

  last_action = accion;
  return accion;
}

*/













/*
// Niveles iniciales (Comportamientos reactivos simples)  ANTIGUO
Action ComportamientoIngeniero::ComportamientoIngenieroNivel_0(Sensores sensores)
{
 
 Action accion = IDLE;

  ActualizarMapa(sensores);

  if (sensores.superficie[0] == 'D')
    tiene_zapatillas = true;

  if (sensores.superficie[0] == 'U')
  {
    return IDLE;
  }


  char i = ViablePorAlturaI(sensores.superficie[1], sensores.cota[1] - sensores.cota[0], tiene_zapatillas);
  char c = ViablePorAlturaI(sensores.superficie[2], sensores.cota[2] - sensores.cota[0], tiene_zapatillas);
  char d = ViablePorAlturaI(sensores.superficie[3], sensores.cota[3] - sensores.cota[0], tiene_zapatillas);



  if (sensores.agentes[1] == 't') i = 'P';
  if (sensores.agentes[2] == 't') c = 'P';
  if (sensores.agentes[3] == 't') d = 'P';


  int pos = VeoCasillaInteresanteI(i, c, d, tiene_zapatillas);

  switch (pos)
  {
  case 2:
    accion = WALK;
    break;
  case 1:
    accion = TURN_SL;
    break;
  case 3:
    accion = TURN_SR;
    break;
  default:
    accion = TURN_SL;
    break;
  }

  last_action = accion;
  return accion;
  
}


*/





/**
 * @brief Comprueba si una celda es de tipo camino transitable.
 * @param c Carácter que representa el tipo de superficie.
 * @return true si es camino ('C'), zapatillas ('D') o meta ('U').
 */
bool ComportamientoIngeniero::es_camino(unsigned char c) const
{
  return (c == 'C' || c == 'D' || c == 'U');
}

/**
 * @brief Comportamiento reactivo del ingeniero para el Nivel 1.
 * @param sensores Datos actuales de los sensores.
 * @return Acción a realizar.
 */









/*


int PuntuacionExploracion(char terreno, int visitas, bool tiene_zap) {
  if (terreno == 'P' || terreno == 'M' || terreno == 'A' || terreno == 'B') return -9999;
  
  int puntos = 0;
  if (terreno == 'U') puntos = 10000; // Meta
  else if (terreno == 'D' && !tiene_zap) puntos = 8000; // Prioridad a las zapatillas
  else if (terreno == 'C' || terreno == 'S' || terreno == 'D') puntos = 2000; // Caminos/Senderos
  else return -9999;

  // PENALIZACIÓN AGRESIVA: Cada visita resta 500 puntos.
  // Esto obliga al agente a ir SIEMPRE a lo que tenga 0 visitas si puede.
  puntos -= (visitas * 500); 
  
  return puntos;
}


Action ComportamientoIngeniero::ComportamientoIngenieroNivel_1(Sensores sensores) {
  Action accion = IDLE;

  // 1. Actualizar mapas y memoria
  ActualizarMapa(sensores);
  mapaVisitas[sensores.posF][sensores.posC]++; // Marcamos donde estamos

  if (sensores.superficie[0] == 'D') tiene_zapatillas = true;

  // 2. Comprobar alturas de las 3 casillas frente a nosotros
  char i_ter = ViablePorAlturaI(sensores.superficie[1], sensores.cota[1] - sensores.cota[0], tiene_zapatillas);
  char c_ter = ViablePorAlturaI(sensores.superficie[2], sensores.cota[2] - sensores.cota[0], tiene_zapatillas);
  char d_ter = ViablePorAlturaI(sensores.superficie[3], sensores.cota[3] - sensores.cota[0], tiene_zapatillas);

  // 3. Evitar al Técnico (lo tratamos como obstáculo insalvable)
  if (sensores.agentes[1] == 't') i_ter = 'P';
  if (sensores.agentes[2] == 't') c_ter = 'P';
  if (sensores.agentes[3] == 't') d_ter = 'P';

  // 4. Calcular coordenadas de las 3 casillas para ver cuántas visitas tienen
  ubicacion actual = {sensores.posF, sensores.posC, (Orientacion)sensores.rumbo};
  
  // Izquierda (Giro virtual de -45º y avanzar)
  int v_izq = ObtenerVisitasSeguro(Delante({actual.f, actual.c, (Orientacion)((actual.brujula+7)%8)}).f, 
                                   Delante({actual.f, actual.c, (Orientacion)((actual.brujula+7)%8)}).c, mapaVisitas);
  // Frente
  int v_cen = ObtenerVisitasSeguro(Delante(actual).f, Delante(actual).c, mapaVisitas);
  
  // Derecha (Giro virtual de +45º y avanzar)
  int v_der = ObtenerVisitasSeguro(Delante({actual.f, actual.c, (Orientacion)((actual.brujula+1)%8)}).f, 
                                   Delante({actual.f, actual.c, (Orientacion)((actual.brujula+1)%8)}).c, mapaVisitas);

  // 5. Calcular puntos de interés
  int score_i = PuntuacionExploracion(i_ter, v_izq, tiene_zapatillas);
  int score_c = PuntuacionExploracion(c_ter, v_cen, tiene_zapatillas);
  int score_d = PuntuacionExploracion(d_ter, v_der, tiene_zapatillas);

  // 6. Decidir acción (Prioridad: mayor puntuación. Si hay empate, preferimos frente)
  if (score_c >= score_i && score_c >= score_d && score_c > -5000) {
    accion = WALK;
  } else if (score_i >= score_d && score_i > -5000) {
    accion = TURN_SL;
  } else if (score_d > -5000) {
    accion = TURN_SR;
  } else {
    // Si todo es peligroso o está muy visitado, giramos para buscar aire nuevo
    accion = TURN_SR;//Le he cambiado contrario q tecnico para q cada uno vaya a una direccion probar TURN_SL para ver cual funciona mejor.
  }

  last_action = accion;
  return accion;
}

*/


 /*Nivel 1 ANTIGUO
 
 

int VeoCasillaInteresanteNivel1I(char i, char c, char d)
{
  // Prioridad 1: Seguir de frente (c) si es Camino, Sendero, Zapatilla o Meta
  if (c == 'C' || c == 'S' || c == 'D' || c == 'U')
    return 2;
  // Prioridad 2: Mirar a la izquierda (i)
  else if (i == 'C' || i == 'S' || i == 'D' || i == 'U')
    return 1;
  // Prioridad 3: Mirar a la derecha (d)
  else if (d == 'C' || d == 'S' || d == 'D' || d == 'U')
    return 3;
  // Si no hay nada interesante a la vista, devolvemos 0
  else
    return 0;
}



Action ComportamientoIngeniero::ComportamientoIngenieroNivel_1(Sensores sensores)
{
  Action accion = IDLE;

  // 1. Dibujar el mapa en la memoria (súper importante para la nota)
  ActualizarMapa(sensores);

  // 2. Comprobar si hemos pisado unas zapatillas
  if (sensores.superficie[0] == 'D') {
    tiene_zapatillas = true;
  }

  // 3. Mecanismo de supervivencia: si chocamos, giramos a la derecha para salir
  if (sensores.choque) {
    accion = TURN_SR;
    last_action = accion;
    return accion;
  }

  // 4. Usar la función de altura del Nivel 0. Nos devuelve la letra del suelo 
  // si podemos pasar, o una 'P' (precipicio) si es muy alto/bajo.
  char i = ViablePorAlturaI(sensores.superficie[1], sensores.cota[1] - sensores.cota[0], tiene_zapatillas);
  char c = ViablePorAlturaI(sensores.superficie[2], sensores.cota[2] - sensores.cota[0], tiene_zapatillas);
  char d = ViablePorAlturaI(sensores.superficie[3], sensores.cota[3] - sensores.cota[0], tiene_zapatillas);

  // 5. Comprobar que no nos chocamos con NADIE. 
  // '_' significa vacio. Si hay cualquier otra letra, engañamos al agente
  // poniéndole una 'P' para que no pise a su compañero.
  if (sensores.agentes[1] != '_') i = 'P';
  if (sensores.agentes[2] != '_') c = 'P';
  if (sensores.agentes[3] != '_') d = 'P';

  // 6. Decidir la mejor dirección
  int pos = VeoCasillaInteresanteNivel1I(i, c, d);

  switch (pos)
  {
  case 2:
    accion = WALK;
    break;
  case 1:
    accion = TURN_SL;
    break;
  case 3:
    accion = TURN_SR;
    break;
  default:
    // Si no vemos ningún camino (pos == 0), giramos a la derecha.
    // Esto hará que giremos sobre nosotros mismos hasta ver un camino nuevo.
    accion = TURN_SR;
    break;
  }

  last_action = accion;
  return accion;
}

*/


Action ComportamientoIngeniero::ComportamientoIngenieroNivel_1(Sensores sensores)
{
  Action accion = IDLE;
  return accion;
}



// Niveles avanzados (Uso de búsqueda)
/**
 * @brief Comportamiento del ingeniero para el Nivel 2 (búsqueda).
 * @param sensores Datos actuales de los sensores.
 * @return Acción a realizar.
 */
Action ComportamientoIngeniero::ComportamientoIngenieroNivel_2(Sensores sensores)
{
  // TODO: Implementar búsqueda para el Nivel 2.
  return IDLE;
}

/**
 * @brief Comportamiento del ingeniero para el Nivel 3.
 * @param sensores Datos actuales de los sensores.
 * @return Acción a realizar.
 */
Action ComportamientoIngeniero::ComportamientoIngenieroNivel_3(Sensores sensores)
{
  return IDLE;
}

/**
 * @brief Comportamiento del ingeniero para el Nivel 4.
 * @param sensores Datos actuales de los sensores.
 * @return Acción a realizar.
 */
Action ComportamientoIngeniero::ComportamientoIngenieroNivel_4(Sensores sensores)
{
  return IDLE;
}

/**
 * @brief Comportamiento del ingeniero para el Nivel 5.
 * @param sensores Datos actuales de los sensores.
 * @return Acción a realizar.
 */
Action ComportamientoIngeniero::ComportamientoIngenieroNivel_5(Sensores sensores)
{
  return IDLE;
}

/**
 * @brief Comportamiento del ingeniero para el Nivel 6.
 * @param sensores Datos actuales de los sensores.
 * @return Acción a realizar.
 */
Action ComportamientoIngeniero::ComportamientoIngenieroNivel_6(Sensores sensores)
{
  return IDLE;
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
