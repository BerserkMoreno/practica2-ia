#include "tecnico.hpp"
#include "motorlib/util.h"
#include <iostream>
#include <queue>
#include <set>

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

char ViablePorAlturaT(char casilla, int dif)
{
  if (abs(dif) <= 1)
    return casilla;
  else
    return 'P';
}



// =========================================================================
// 1. FUNCIONES AYUDANTES DEL TÉCNICO (SISTEMA DE INSTANTES)
// =========================================================================

/**
 * @brief Guarda el instante actual en la posición donde estamos.
 */
void ComportamientoTecnico::RegistrarVisitaT(ubicacion actual) {
    if (mapaUltimoPaso.empty()) {
        mapaUltimoPaso.assign(mapaResultado.size(), vector<int>(mapaResultado[0].size(), 0));
    }
    // Guardamos el "segundo" actual del reloj
    mapaUltimoPaso[actual.f][actual.c] = instanteActual;
}

bool ComportamientoTecnico::EsCaminoLimpioT(ubicacion destino, int idx_sensor, const Sensores& sensores) {
    char terreno = sensores.superficie[idx_sensor];
    bool es_pisable = (terreno == 'C' || terreno == 'D' || terreno == 'U');
    if (tiene_zapatillas && terreno == 'B') es_pisable = true; 
    
    if (!es_pisable) return false;
    if (sensores.agentes[idx_sensor] != '_') return false; //Con esto marcamos que la casilla esta desocupada _ por tanto miramos si no esta desocupada
    
    return true;
}

bool ComportamientoTecnico::SePuedeCaminarT(ubicacion origen, int idx_sensor, const Sensores& sensores) {
  ubicacion destino = Delante(origen);
    if (!EsCaminoLimpioT(destino, idx_sensor, sensores)) return false;
    
    int desnivel = abs(mapaCotas[destino.f][destino.c] - mapaCotas[origen.f][origen.c]);
    return (desnivel <= 1);
}

/**
 * @brief Evalúa la casilla basándose en cuánto tiempo hace que no pasamos.
 */
int ComportamientoTecnico::EvaluarLadoT(ubicacion origen, int idx_sensor, const Sensores& sensores) {
    if (!SePuedeCaminarT(origen, idx_sensor, sensores)) {
        return -999999; 
    }

    ubicacion destino = Delante(origen);
    char terreno = sensores.superficie[idx_sensor];
    
    int puntos = 100;

    // Prioridades de misión
    if (terreno == 'U') puntos = 10000;
    else if (terreno == 'D' && !tiene_zapatillas) puntos = 5000;

    // --- LÓGICA DE INSTANTES (FRESCURA) ---
    // Cuanto más pequeño sea el número en mapaUltimoPaso, más tiempo hace que no vamos.
    // Queremos que las casillas "viejas" tengan más puntos.
    // Restamos el instante guardado: si estuvimos en el paso 10, restamos 10. 
    // Si estuvimos en el 500, restamos 500. El de 10 ganará.
    int ultimo_instante = mapaUltimoPaso[destino.f][destino.c];
    puntos -= ultimo_instante; 

    return puntos;
}


// =========================================================================
// 2. EL CEREBRO PRINCIPAL
// =========================================================================

Action ComportamientoTecnico::ComportamientoTecnicoNivel_0(Sensores sensores)
{
    // Aumentamos nuestro reloj en cada decisión
    instanteActual++;

    ActualizarMapa(sensores); 
    ubicacion posicion_actual = {sensores.posF, sensores.posC, sensores.rumbo};
    RegistrarVisitaT(posicion_actual);

    if (sensores.superficie[0] == 'D') tiene_zapatillas = true;
    if (sensores.superficie[0] == 'U') return IDLE; 

    // Proyectar coordenadas
    ubicacion mirar_frente = posicion_actual;
    ubicacion mirar_izq = posicion_actual; 
    mirar_izq.brujula = (Orientacion)((posicion_actual.brujula + 7) % 8);
    ubicacion mirar_der = posicion_actual; 
    mirar_der.brujula = (Orientacion)((posicion_actual.brujula + 1) % 8);

    // Puntuar
    int nota_izq    = EvaluarLadoT(mirar_izq,    1, sensores);
    int nota_frente = EvaluarLadoT(mirar_frente, 2, sensores);
    int nota_der    = EvaluarLadoT(mirar_der,    3, sensores);

    // Encontrar al ganador
    int ganador = nota_frente;
    if (nota_izq > ganador) ganador = nota_izq;
    if (nota_der > ganador) ganador = nota_der;

    Action accion_elegida = IDLE;

    if (ganador <= -999999) {
        accion_elegida = TURN_SL;
    } 
    else if (ganador == nota_frente) {
        accion_elegida = WALK;
    } 
    else if (ganador == nota_izq) {
        accion_elegida = TURN_SL;
    } 
    else {
        accion_elegida = TURN_SR;
    }

    last_action = accion_elegida;
    return accion_elegida;
}


/*

// =========================================================================
// 1. FUNCIONES AYUDANTES DEL TÉCNICO
// =========================================================================

void ComportamientoTecnico::RegistrarVisitaT(ubicacion actual) {
    if (mapaVisitas.empty()) {
        mapaVisitas.assign(mapaResultado.size(), vector<int>(mapaResultado[0].size(), 0));
    }
    mapaVisitas[actual.f][actual.c]++;
}

bool ComportamientoTecnico::EsCaminoLimpioT(ubicacion destino, int idx_sensor, const Sensores& sensores) {
    char terreno = sensores.superficie[idx_sensor];
    
    // El Técnico puede pisar Caminos, Zapatillas, Metas y... ¡Bosques si tiene zapatillas!
    bool es_pisable = (terreno == 'C' || terreno == 'D' || terreno == 'U');
    if (tiene_zapatillas && terreno == 'B') es_pisable = true; 
    
    if (!es_pisable) return false;
    
    // Si hay un aldeano 't' o el propio ingeniero 'i' bloqueando, no es pisable
    if (sensores.agentes[idx_sensor] != '_') return false;
    
    return true;
}

bool ComportamientoTecnico::SePuedeCaminarT(ubicacion origen, int idx_sensor, const Sensores& sensores) {
    ubicacion destino = Delante(origen);

    if (!EsCaminoLimpioT(destino, idx_sensor, sensores)) return false;
    
    // El técnico NUNCA puede superar un desnivel mayor a 1
    int desnivel = abs(mapaCotas[destino.f][destino.c] - mapaCotas[origen.f][origen.c]);
    if (desnivel > 1) return false;

    return true;
}

int ComportamientoTecnico::EvaluarLadoT(ubicacion origen, int idx_sensor, const Sensores& sensores) {
    // Si físicamente no puedo ir (muro, precipicio o Ingeniero bloqueando), devuelvo la peor nota posible
    if (!SePuedeCaminarT(origen, idx_sensor, sensores)) {
        return -999999; // Usamos un número exageradamente bajo
    }

    ubicacion destino = Delante(origen);
    char terreno = sensores.superficie[idx_sensor];
    
    int puntos = 100; // Puntuación base

    // Prioridades
    if (terreno == 'U') puntos = 10000;
    else if (terreno == 'D' && !tiene_zapatillas) puntos = 5000;

    // Aplicamos la memoria (Aburrimiento): restamos puntos si ya la hemos pisado
    if (destino.f >= 0 && destino.f < mapaVisitas.size() && destino.c >= 0 && destino.c < mapaVisitas[0].size()) {
        puntos -= (mapaVisitas[destino.f][destino.c] * 15);
    }

    return puntos;
}

// =========================================================================
// 2. EL CEREBRO PRINCIPAL DEL TÉCNICO
// =========================================================================

Action ComportamientoTecnico::ComportamientoTecnicoNivel_0(Sensores sensores)
{
    ActualizarMapa(sensores); 
    
    ubicacion posicion_actual = {sensores.posF, sensores.posC, sensores.rumbo};
    RegistrarVisitaT(posicion_actual);

    // Actualizar Zapatillas y Victoria
    if (sensores.superficie[0] == 'D') tiene_zapatillas = true;
    if (sensores.superficie[0] == 'U') return IDLE; 

    // OJO: Ya no ponemos el `if (sensores.agentes[2] == 'i') return IDLE;`. 
    // Ahora, si el Ingeniero está delante, la función `EsCaminoLimpioT` lo detecta, 
    // le pone nota -999999, y el Técnico decide girar de forma natural para esquivarlo.

    // Proyectar coordenadas de visión
    ubicacion mirar_frente = posicion_actual;
    
    ubicacion mirar_izq = posicion_actual; 
    mirar_izq.brujula = (Orientacion)((posicion_actual.brujula + 7) % 8);
    
    ubicacion mirar_der = posicion_actual; 
    mirar_der.brujula = (Orientacion)((posicion_actual.brujula + 1) % 8);

    // Puntuar las 3 opciones
    int nota_izq    = EvaluarLadoT(mirar_izq,    1, sensores);
    int nota_frente = EvaluarLadoT(mirar_frente, 2, sensores);
    int nota_der    = EvaluarLadoT(mirar_der,    3, sensores);

    // Comparar y elegir al ganador
    int ganador = nota_frente;
    if (nota_izq > ganador) ganador = nota_izq;
    if (nota_der > ganador) ganador = nota_der;

    Action accion_elegida = IDLE;

    // Fíjate que ahora comparamos con -999999
    if (ganador <= -999999) {
        // Callejón sin salida total, giramos para buscar alternativa
        accion_elegida = TURN_SL;
    } 
    else if (ganador == nota_frente) {
        accion_elegida = WALK;
    } 
    else if (ganador == nota_izq) {
        accion_elegida = TURN_SL;
    } 
    else {
        accion_elegida = TURN_SL;
    }

    last_action = accion_elegida;
    return accion_elegida;
}
*/

































/*ANTIGUA

int VeoCasillaInteresanteT(char i, char c, char d)
{
  if (c == 'U')
    return 2;
  else if (i == 'U')
    return 1;
  else if (d == 'U')
    return 3;
  else if (c == 'C')
    return 2;
  else if (i == 'C')
    return 1;
  else if (d == 'C')
    return 3;
  else
    return 0;
}

*/














//17 NIVELES
/*
// Devuelve las visitas de una casilla de forma segura sin salirse de la matriz
int ObtenerVisitasSeguroT(int f, int c, const vector<vector<int>>& matrizVisitas)
{
  if (f >= 0 && f < matrizVisitas.size() && c >= 0 && c < matrizVisitas[0].size()) {
    return matrizVisitas[f][c];
  }
  return 9999; 
}

// Función auxiliar para darle una nota a cada casilla específica del TÉCNICO
int PuntuacionCasillaT(char terreno, int visitas, bool tiene_zap)
{
  if (terreno == 'P') return -9999; // Precipicios o compañeros prohibidos
  
  int puntos = 0;
  if (terreno == 'U') puntos = 10000; 
  else if (terreno == 'D' && !tiene_zap) puntos = 5000; 
  else if (terreno == 'C' || terreno == 'S' || (terreno == 'D' && tiene_zap)) puntos = 1000; 
  // LA GRAN DIFERENCIA DEL TÉCNICO: Si tiene zapatillas, el Bosque ('B') es un camino válido
  else if (terreno == 'B' && tiene_zap) puntos = 1000; 
  else return -9999; // Resto de terrenos (Agua, Muros, o Bosque sin zapatillas)

  // Restamos atractivo por cada visita
  puntos -= (visitas * 100); 
  
  return puntos;
}

// Evaluamos frente, izquierda y derecha
int VeoCasillaInteresanteT(char i, char c, char d, int vi, int vc, int vd, bool zap)
{
  int score_i = PuntuacionCasillaT(i, vi, zap);
  int score_c = PuntuacionCasillaT(c, vc, zap);
  int score_d = PuntuacionCasillaT(d, vd, zap);

  // Si todas son inviables, girar
  if (score_i == -9999 && score_c == -9999 && score_d == -9999) return 0;

  // Prioridad: Mayor puntuación. Desempate: Frente > Izquierda > Derecha
  if (score_c >= score_i && score_c >= score_d) return 2;
  if (score_i >= score_d) return 1;
  return 3;
}





Version 15 test
Action ComportamientoTecnico::ComportamientoTecnicoNivel_0(Sensores sensores)
{
  Action accion = IDLE;
  ActualizarMapa(sensores);

  // 1. Apuntamos en nuestra memoria que hemos pisado esta casilla
  mapaVisitas[sensores.posF][sensores.posC]++;

  if (sensores.superficie[0] == 'D')
    tiene_zapatillas = true;

  if (sensores.superficie[0] == 'U')
    return IDLE; // Objetivo conseguido

  // 2. Altura (Recordemos: ViablePorAlturaT del técnico solo permite desniveles <= 1 SIEMPRE)
  char i = ViablePorAlturaT(sensores.superficie[1], sensores.cota[1] - sensores.cota[0]);
  char c = ViablePorAlturaT(sensores.superficie[2], sensores.cota[2] - sensores.cota[0]);
  char d = ViablePorAlturaT(sensores.superficie[3], sensores.cota[3] - sensores.cota[0]);

  // 3. Evitar atropellar al Ingeniero ('i') tratándolo como precipicio
  if (sensores.agentes[1] == 'i') i = 'P';
  if (sensores.agentes[2] == 'i') c = 'P';
  if (sensores.agentes[3] == 'i') d = 'P';

  // 4. Calcular coordenadas exactas de las casillas Izq, Frente y Der
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

  // 5. Mirar mapa de visitas en esas coordenadas
  int vi = ObtenerVisitasSeguroT(c_izq.f, c_izq.c, mapaVisitas);
  int vc = ObtenerVisitasSeguroT(c_cen.f, c_cen.c, mapaVisitas);
  int vd = ObtenerVisitasSeguroT(c_der.f, c_der.c, mapaVisitas);

  // 6. Decidir la mejor dirección (Añadimos 'tiene_zapatillas' para que sepa si puede pisar Bosques)
  int pos = VeoCasillaInteresanteT(i, c, d, vi, vc, vd, tiene_zapatillas);

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
    // Si no hay salida, giramos a la izquierda para darnos la vuelta poco a poco
    accion = TURN_SL;
    break;
  }

  last_action = accion;
  return accion;
}
*/



/*ANTIGUO
// Niveles del técnico
Action ComportamientoTecnico::ComportamientoTecnicoNivel_0(Sensores sensores)
{
  
  Action accion = IDLE;

  ActualizarMapa(sensores);

  if (sensores.superficie[0] == 'D')
    tiene_zapatillas = true;

  if (sensores.superficie[0] == 'U')
    return IDLE;



//Solucionar choque 
  if (sensores.agentes[2] == 'i') {
      accion = IDLE;
      last_action = accion;
      return accion;
  }


  char i = ViablePorAlturaT(sensores.superficie[1], sensores.cota[1] - sensores.cota[0]);
  char c = ViablePorAlturaT(sensores.superficie[2], sensores.cota[2] - sensores.cota[0]);
  char d = ViablePorAlturaT(sensores.superficie[3], sensores.cota[3] - sensores.cota[0]);

  // Decisión de la siguiente acción
  int pos = VeoCasillaInteresanteT(i, c, d);

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
bool ComportamientoTecnico::es_camino(unsigned char c) const
{
  return (c == 'C' || c == 'D' || c == 'U');
}

/**
 * @brief Comportamiento reactivo del técnico para el Nivel 1.
 * @param sensores Datos actuales de los sensores.
 * @return Acción a realizar.
 */
















// Función de puntuación de exploración específica para el TÉCNICO
int PuntuacionExploracionT(char terreno, int visitas, bool tiene_zap) {
  // Peculiaridad: El Técnico NO entra en Agua 'A', ni Muros 'M'. 
  // Solo entra en Bosque 'B' si tiene zapatillas.
  if (terreno == 'P' || terreno == 'M' || terreno == 'A') return -9999;
  if (terreno == 'B' && !tiene_zap) return -9999;
  
  int puntos = 0;
  if (terreno == 'U') puntos = 10000; 
  else if (terreno == 'D' && !tiene_zap) puntos = 8000; 
  else if (terreno == 'C' || terreno == 'S' || (terreno == 'B' && tiene_zap) || (terreno == 'D' && tiene_zap)) puntos = 1000; 

  // PENALIZACIÓN AGRESIVA: 500 puntos menos por cada vez que hayamos pasado.
  // Esto le obliga a buscar casillas con 0 visitas.
  puntos -= (visitas * 500); 
  
  return puntos;
}





Action ComportamientoTecnico::ComportamientoTecnicoNivel_1(Sensores sensores) {
  Action accion = IDLE;
/*
  // 1. Actualizar memoria y marcar rastro
  ActualizarMapa(sensores);
  mapaVisitas[sensores.posF][sensores.posC]++; 

  if (sensores.superficie[0] == 'D') tiene_zapatillas = true;

  // 2. REGLA DE ORO: Si el Ingeniero está de frente, me paro para dejarle pasar
  if (sensores.agentes[2] == 'i') {
    last_action = IDLE;
    return IDLE;
  }

  // 3. Altura (Peculiaridad: el técnico siempre usa ViablePorAlturaT que limita a 1)
  char i_ter = ViablePorAlturaT(sensores.superficie[1], sensores.cota[1] - sensores.cota[0]);
  char c_ter = ViablePorAlturaT(sensores.superficie[2], sensores.cota[2] - sensores.cota[0]);
  char d_ter = ViablePorAlturaT(sensores.superficie[3], sensores.cota[3] - sensores.cota[0]);

  // 4. Detectar al Ingeniero en los laterales para no chocar
  if (sensores.agentes[1] == 'i') i_ter = 'P';
  if (sensores.agentes[3] == 'i') d_ter = 'P';

  // 5. Calcular coordenadas para consultar visitas (Usando solo 3 sensores frontales)
  ubicacion actual = {sensores.posF, sensores.posC, (Orientacion)sensores.rumbo};
  
  int v_izq = ObtenerVisitasSeguroT(Delante({actual.f, actual.c, (Orientacion)((actual.brujula+7)%8)}).f, 
                                    Delante({actual.f, actual.c, (Orientacion)((actual.brujula+7)%8)}).c, mapaVisitas);
  int v_cen = ObtenerVisitasSeguroT(Delante(actual).f, Delante(actual).c, mapaVisitas);
  int v_der = ObtenerVisitasSeguroT(Delante({actual.f, actual.c, (Orientacion)((actual.brujula+1)%8)}).f, 
                                    Delante({actual.f, actual.c, (Orientacion)((actual.brujula+1)%8)}).c, mapaVisitas);

  // 6. Evaluar con los pesos del Técnico
  int score_i = PuntuacionExploracionT(i_ter, v_izq, tiene_zapatillas);
  int score_c = PuntuacionExploracionT(c_ter, v_cen, tiene_zapatillas);
  int score_d = PuntuacionExploracionT(d_ter, v_der, tiene_zapatillas);

  // 7. Decidir acción (Prioridad: Frente > Izquierda > Derecha)
  if (score_c >= score_i && score_c >= score_d && score_c > -5000) {
    accion = WALK;
  } else if (score_i >= score_d && score_i > -5000) {
    accion = TURN_SL;
  } else if (score_d > -5000) {
    accion = TURN_SR;
  } else {
    // Si no hay salida o todo está muy pisado, giramos para buscar nuevas rutas
    accion = TURN_SL;
  }

  last_action = accion;
  */
  return accion;
}

























 /*ANTIGUO NIVEL 1
int VeoCasillaInteresanteNivel1T(char i, char c, char d, bool zap)
{
  // Prioridad 1: Frente (c)
  if (c == 'C' || c == 'S' || c == 'D' || c == 'U' || (zap && c == 'B'))
    return 2;
  // Prioridad 2: Izquierda (i)
  else if (i == 'C' || i == 'S' || i == 'D' || i == 'U' || (zap && i == 'B'))
    return 1;
  // Prioridad 3: Derecha (d)
  else if (d == 'C' || d == 'S' || d == 'D' || d == 'U' || (zap && d == 'B'))
    return 3;
  // Si no hay nada, 0
  else
    return 0;
}



Action ComportamientoTecnico::ComportamientoTecnicoNivel_1(Sensores sensores)
{
  Action accion = IDLE;

  // 1. Dibujar el mapa
  ActualizarMapa(sensores);

  // 2. Recoger zapatillas
  if (sensores.superficie[0] == 'D') {
    tiene_zapatillas = true;
  }

  // 3. Mecanismo de supervivencia al choque
  if (sensores.choque) {
    accion = TURN_SR;
    last_action = accion;
    return accion;
  }

  // 4. Comprobar alturas usando la función del Nivel 0 del técnico
  // Recordatorio: el técnico NUNCA usa zapatillas para saltar más alto, solo para los bosques
  char i = ViablePorAlturaT(sensores.superficie[1], sensores.cota[1] - sensores.cota[0]);
  char c = ViablePorAlturaT(sensores.superficie[2], sensores.cota[2] - sensores.cota[0]);
  char d = ViablePorAlturaT(sensores.superficie[3], sensores.cota[3] - sensores.cota[0]);

  // 5. Evitar colisiones con compañeros
  if (sensores.agentes[1] != '_') i = 'P';
  if (sensores.agentes[2] != '_') c = 'P';
  if (sensores.agentes[3] != '_') d = 'P';

  // 6. Decidir mejor dirección (¡pasando la variable de las zapatillas!)
  int pos = VeoCasillaInteresanteNivel1T(i, c, d, tiene_zapatillas);

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
    accion = TURN_SR;
    break;
  }

  last_action = accion;
  return accion;
}

*/












/**
 * @brief Comportamiento del técnico para el Nivel 2.
 * @param sensores Datos actuales de los sensores.
 * @return Acción a realizar.
 */
Action ComportamientoTecnico::ComportamientoTecnicoNivel_2(Sensores sensores)
{
  return IDLE;
}

/**
 * @brief Comportamiento del técnico para el Nivel 3.
 * @param sensores Datos actuales de los sensores.
 * @return Acción a realizar.
 */
Action ComportamientoTecnico::ComportamientoTecnicoNivel_3(Sensores sensores)
{
  return IDLE;
}

/**
 * @brief Comportamiento del técnico para el Nivel 4.
 * @param sensores Datos actuales de los sensores.
 * @return Acción a realizar.
 */
Action ComportamientoTecnico::ComportamientoTecnicoNivel_4(Sensores sensores)
{
  return IDLE;
}

/**
 * @brief Comportamiento del técnico para el Nivel 5.
 * @param sensores Datos actuales de los sensores.
 * @return Acción a realizar.
 */
Action ComportamientoTecnico::ComportamientoTecnicoNivel_5(Sensores sensores)
{
  return IDLE;
}

/**
 * @brief Comportamiento del técnico para el Nivel 6.
 * @param sensores Datos actuales de los sensores.
 * @return Acción a realizar.
 */
Action ComportamientoTecnico::ComportamientoTecnicoNivel_6(Sensores sensores)
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
