#ifndef COMPORTAMIENTOINGENIERO_H
#define COMPORTAMIENTOINGENIERO_H

#include <chrono>
#include <list>
#include <map>
#include <set>
#include <thread>
#include <time.h>

#include "comportamientos/comportamiento.hpp"

struct EstadoI
{
  ubicacion site;
  bool zapatillas;

  bool operator==(const EstadoI &st) const
  {
    return site == st.site and zapatillas == st.zapatillas;
  }

  bool operator!=(const EstadoI &st) const
  {
    return !(*this == st);
  }
};

struct NodoI
{
  EstadoI estado;
  list<Action> secuencia;
  bool operator==(const NodoI &node) const
  {
    return estado == node.estado;
  }
  bool operator<(const NodoI &node) const
  {
    if (estado.site.f < node.estado.site.f)
      return true;
    else if (estado.site.f == node.estado.site.f and estado.site.c < node.estado.site.c)
      return true;
    else if (estado.site.f == node.estado.site.f and estado.site.c == node.estado.site.c and estado.site.brujula < node.estado.site.brujula)
      return true;
    else if (estado.site.f == node.estado.site.f and estado.site.c == node.estado.site.c and estado.site.brujula == node.estado.site.brujula and estado.zapatillas < node.estado.zapatillas)
      return true;
    else
      return false;
  }
};

// Estructura del Nodo para el A* del Nivel 4
struct NodoAStarN4
{
  int f, c, op;
  int g;               
  int h;            
  int eco;              // Impacto ecológico acumulado
  std::list<Paso> plan; 

  int f_val() const { return g + h; }

  bool operator<(const NodoAStarN4 &otro) const
  {
    if (f_val() != otro.f_val())
      return f_val() > otro.f_val(); // Camino más corto en tramos
    if (h != otro.h)
      return h > otro.h;   // Si hay empate, coger el más cerca a la meta
    return eco > otro.eco; // Si hay empate otra vez, coger el que menos contamine
  }
};

class ComportamientoIngeniero : public Comportamiento
{
public:
  // =========================================================================
  // CONSTRUCTORES
  // =========================================================================

  /**
   * @brief Constructor para niveles 0, 1 y 6 (sin mapa completo)
   * @param size Tamaño del mapa (si es 0, se inicializa más tarde)
   */
  ComportamientoIngeniero(unsigned int size = 0) : Comportamiento(size)
  {
    // Inicializar Variables de Estado

    last_action = IDLE;
    tiene_zapatillas = false;
    giro45Izq = 0;

    instanteActual = 0;
    mapaUltimoPaso.assign(mapaResultado.size(), vector<int>(mapaResultado[0].size(), 0));
    nVisitas.assign(mapaResultado.size(), vector<int>(mapaResultado[0].size(), 0));

    n6_estado = 0;
    n6_casillas_conocidas_last_plan = 0;

    ha_llegado_fuga = false;
    hayPlan = false;

    n5_estado = 0;
    n5_tramo = 0;
    n5_espera = 0;
    n5_obs_f = -1;
    n5_obs_c = -1;
    n5_espera_path = 0;        // Cooldown para no quemar CPU
    n5_plan_intentado = false; // Solo planificar 1 vez

    n6 = false;
  }

  /**
   * @brief Constructor para niveles 2, 3, 4 y 5 (con mapa completo conocido)
   * @param mapaR Mapa de terreno conocido
   * @param mapaC Mapa de cotas conocido
   */
  ComportamientoIngeniero(std::vector<std::vector<unsigned char>> mapaR,
                          std::vector<std::vector<unsigned char>> mapaC) : Comportamiento(mapaR, mapaC)
  {
    // Inicializar Variables de Estado
    hayPlan = false;
    tiene_zapatillas = false;

    n5_estado = 0;
    n5_tramo = 0;
    n5_espera = 0;
    n5_obs_f = -1;
    n5_obs_c = -1;
    n5_espera_path = 0;        // Cooldown para no quemar CPU
    n5_plan_intentado = false; // Solo planificar 1 vez

    n6_estado = 0;
    n6_casillas_conocidas_last_plan = 0;
    ha_llegado_fuga = false;
    n6 = false;
  }

  ComportamientoIngeniero(const ComportamientoIngeniero &comport)
      : Comportamiento(comport) {}
  ~ComportamientoIngeniero() {}

  /**
   * @brief Bucle principal de decisión del agente.
   * Estudia los sensores y decide la siguiente acción.
   *
   * EJEMPLO DE USO:
   * Action accion = think(sensores);
   * return accion; // El motor ejecutará esta acción
   */
  Action think(Sensores sensores);

  ComportamientoIngeniero *clone()
  {
    return new ComportamientoIngeniero(*this);
  }

  // =========================================================================
  // ÁREA DE IMPLEMENTACIÓN DEL ESTUDIANTE
  // =========================================================================

  // Funciones específicas para cada nivel (para ser implementadas por el alumno)

  /**
   * @brief Implementación del Nivel 0.
   * @param sensores Datos actuales de los sensores del agente.
   * @return Acción a realizar.
   */
  Action ComportamientoIngenieroNivel_0(Sensores sensores);

  /**
   * @brief Implementación del Nivel 1.
   * @param sensores Datos actuales de los sensores del agente.
   * @return Acción a realizar.
   */
  Action ComportamientoIngenieroNivel_1(Sensores sensores);

  /**
   * @brief Implementación del Nivel 2.
   * @param sensores Datos actuales de los sensores del agente.
   * @return Acción a realizar.
   */
  Action ComportamientoIngenieroNivel_2(Sensores sensores);

  /**
   * @brief Implementación del Nivel 3.
   * @param sensores Datos actuales de los sensores del agente.
   * @return Acción a realizar.
   */
  Action ComportamientoIngenieroNivel_3(Sensores sensores);

  /**
   * @brief Implementación del Nivel 4.
   * @param sensores Datos actuales de los sensores del agente.
   * @return Acción a realizar.
   */
  Action ComportamientoIngenieroNivel_4(Sensores sensores);

  /**
   * @brief Implementación del Nivel 5.
   * @param sensores Datos actuales de los sensores del agente.
   * @return Acción a realizar.
   */
  Action ComportamientoIngenieroNivel_5(Sensores sensores);

  /**
   * @brief Implementación del Nivel 6.
   * @param sensores Datos actuales de los sensores del agente.
   * @return Acción a realizar.
   */
  Action ComportamientoIngenieroNivel_6(Sensores sensores);

protected:
  // =========================================================================
  // FUNCIONES PROPORCIONADAS
  // =========================================================================

  /**
   * @brief Actualiza la información del mapa interno basándose en los sensores.
   * IMPORTANTE: Esta función ya está implementada. Actualiza mapaResultado y mapaCotas
   * con la información de los 16 sensores (casilla actual + 15 casillas alrededor).
   */
  void ActualizarMapa(Sensores sensores);

  /**
   * @brief Comprueba si una casilla es transitable.
   * @param f Fila de la casilla.
   * @param c Columna de la casilla.
   * @param tieneZapatillas Indica si el agente posee zapatillas.
   * @return true si la casilla es transitable (no es muro ni precipicio).
   */
  bool EsCasillaTransitableLevel0(int f, int c, bool tieneZapatillas);

  /**
   * @brief Comprueba si la casilla de delante es accesible por diferencia de altura.
   * REGLAS: Desnivel máximo 1 sin zapatillas, 2 con zapatillas.
   * @param actual Estado actual del agente (fila, columna, orientacion).
   * @return true si el desnivel con la casilla de delante es admisible.
   */
  bool EsAccesiblePorAltura(const ubicacion &actual, bool zap);

  /**
   * @brief Devuelve la posición (fila, columna) de la casilla que hay delante del agente.
   * @param actual Estado actual del agente (fila, columna, orientacion).
   * @return Estado con la fila y columna de la casilla de enfrente.
   */
  ubicacion Delante(const ubicacion &actual) const;

  bool es_camino(unsigned char c) const;

  /**
   * @brief Imprime por consola la secuencia de acciones de un plan para un agente.
   * @param plan  Lista de acciones del plan.
   */
  void PintaPlan(const list<Action> &plan);

  /**
   * @brief Imprime las coordenadas y operaciones de un plan de tubería.
   * @param plan  Lista de pasos (fila, columna, operación).
   */
  void PintaPlan(const list<Paso> &plan);

  /**
   * @brief Convierte un plan de acciones en una lista de casillas para
   *        su visualización en el mapa gráfico.
   * @param st    Estado de partida.
   * @param plan  Lista de acciones del plan.
   */
  void VisualizaPlan(const ubicacion &st, const list<Action> &plan);

  /**
   * @brief Convierte un plan de tubería en la lista de casillas usada
   *        por el sistema de visualización.
   * @param st    Estado de partida (no utilizado directamente).
   * @param plan  Lista de pasos del plan de tubería.
   */
  void VisualizaRedTuberias(const list<Paso> &plan);

private:
  // =========================================================================
  // VARIABLES DE ESTADO (PUEDEN SER EXTENDIDAS POR EL ALUMNO)
  // =========================================================================

  Action last_action;
  bool tiene_zapatillas;
  int giro45Izq;

  vector<vector<int>> nVisitas;
  int instanteActual;
  vector<vector<int>> mapaUltimoPaso;

  // Funciones para el nivel 0
  void RegistrarPasoEnElReloj(ubicacion actual);
  bool EsCasillaDespejada(ubicacion destino, int idx_sensor, const Sensores &sensores);
  bool SePuedeCaminar(ubicacion origen, int idx_sensor, const Sensores &sensores);
  bool SePuedeSaltar(ubicacion origen, int idx_int, int idx_dest, const Sensores &sensores);
  int CalcularInteres(ubicacion destino, char terreno);
  void EvaluarRuta(ubicacion origen, int idx_walk, int idx_jump, const Sensores &sensores, int &mejor_nota, bool &prefiere_saltar);

  // Funciones para el Nivel 1
  bool es_transitable_nivel_1(unsigned char c) const;
  bool EsCasillaDespejadaN1(ubicacion destino, int idx_sensor, const Sensores &sensores);
  bool SePuedeCaminarN1(ubicacion origen, int idx_sensor, const Sensores &sensores);
  bool SePuedeSaltarN1(ubicacion origen, int idx_int, int idx_dest, const Sensores &sensores);
  void EvaluarRutaN1(ubicacion origen, int idx_walk, int idx_jump, const Sensores &sensores, int &mejor_nota, bool &prefiere_saltar);
  int ContarDesconocidos(const ubicacion &centro);
  int CalcularInteresN1(ubicacion destino, char terreno);

  bool hayPlan;
  std::list<Action> plan;

  // Funciones del Nivel 2
  std::list<Action> B_Anchura_Ingeniero(const EstadoI &inicio, const EstadoI &final,
                                        const std::vector<std::vector<unsigned char>> &mapa,
                                        const std::vector<std::vector<unsigned char>> &cotas);

  // VARIABLES Y MÉTODOS PARA EL NIVEL 5

  int n5_estado;
  int n5_tramo;
  vector<Paso> n5_plan_tuberia;
  std::list<Action> n5_plan_movimiento;

  Action MoverA_Ingeniero(int f, int c, const Sensores &sensores);
  Orientacion GetDireccion(int f_origen, int c_origen, int f_destino, int c_destino);
  Action GetTurnAction(Orientacion actual, Orientacion deseada);

  int n5_espera = 0;
  int n5_obs_f = -1;
  int n5_obs_c = -1;

  int n5_espera_path = 0;         // Cooldown para no quemar CPU
  bool n5_plan_intentado = false; // Solo planificar 1 vez

  // VARIABLES Y MÉTODOS PARA EL NIVEL 6

  int n6_estado = 0;
  int n6_casillas_conocidas_last_plan = 0;
  bool ha_llegado_fuga;
  bool n6;
};

#endif
