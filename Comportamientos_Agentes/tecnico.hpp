#ifndef COMPORTAMIENTOTECNICO_H
#define COMPORTAMIENTOTECNICO_H

#include <chrono>
#include <time.h>
#include <thread>
#include <list>
#include <map>

#include "comportamientos/comportamiento.hpp"

// =========================================================================
// DOCUMENTACIÓN PARA ESTUDIANTES
// =========================================================================
/*
 * CLASE: ComportamientoTecnico
 *
 * DESCRIPCIÓN:
 * Esta clase implementa el comportamiento del agente Técnico en el mundo Belkan.
 * El técnico colabora con el ingeniero para resolver el problema de instalación de tuberías
 */

struct EstadoT
{
  ubicacion site;
  bool zapatillas;

  bool operator==(const EstadoT &st) const
  {
    return site == st.site and zapatillas == st.zapatillas;
  }

  bool operator<(const EstadoT &st) const
  {
    if (site.f != st.site.f)
      return site.f < st.site.f;
    if (site.c != st.site.c)
      return site.c < st.site.c;
    if (site.brujula != st.site.brujula)
      return site.brujula < st.site.brujula;
    return zapatillas < st.zapatillas;
  }
};

struct NodoT
{
  EstadoT estado;
  list<Action> secuencia;
  bool operator==(const NodoT &node) const
  {
    return estado == node.estado;
  }
  bool operator<(const NodoT &node) const
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

struct NodoAStarT
{
  EstadoT estado;
  list<Action> secuencia;
  int g;
  int h;

  int f() const { return g + h; }

  bool operator<(const NodoAStarT &otro) const
  {
    if (f() != otro.f())
      return f() > otro.f(); 
    return g < otro.g;    
  }
};

class ComportamientoTecnico : public Comportamiento
{
public:
  // =========================================================================
  // CONSTRUCTORES
  // =========================================================================

  /**
   * @brief Constructor para niveles 0, 1 y 6 (sin mapa completo)
   * @param size Tamaño del mapa (si es 0, se inicializa más tarde)
   */
  ComportamientoTecnico(unsigned int size = 0) : Comportamiento(size)
  {
    // Inicializar Variables de Estado
    last_action = IDLE;
    tiene_zapatillas = false;
    giro45Izq = 0;

    instanteActual = 0;

    mapaUltimoPaso.assign(mapaResultado.size(), vector<int>(mapaResultado[0].size(), 0));
    nVisitas.assign(mapaResultado.size(), vector<int>(mapaResultado[0].size(), 0));

    n6_estado = 0;
    n6T = false;
  }

  /**
   * @brief Constructor para niveles 2, 3, 4 y 5 (con mapa completo conocido)
   * @param mapaR Mapa de terreno conocido
   * @param mapaC Mapa de cotas conocido
   */
  ComportamientoTecnico(std::vector<std::vector<unsigned char>> mapaR,
                        std::vector<std::vector<unsigned char>> mapaC) : Comportamiento(mapaR, mapaC)
  {
    // Inicializar Variables de Estado
    hayPlan = false;
    tiene_zapatillas = false;

    n5_estado = 0;
    n5_target_f = -1;
    n5_target_c = -1;

    n5_espera = 0;
    n5_obs_f = -1;
    n5_obs_c = -1;

    n5_espera_path = 0; // Tiempo de espera para evitar bloqueos del algoritmo
    n6T = false;
  }

  ComportamientoTecnico(const ComportamientoTecnico &comport) : Comportamiento(comport) {}
  ~ComportamientoTecnico() {}

  /**
   * @brief Bucle principal de decisión del técnico.
   * Estudia los sensores y decide la siguiente acción.
   *
   * EJEMPLO DE USO:
   * Action accion = think(sensores);
   * return accion; // El motor ejecutará esta acción
   */
  Action think(Sensores sensores);

  ComportamientoTecnico *clone()
  {
    return new ComportamientoTecnico(*this);
  }

  // =========================================================================
  // ÁREA DE IMPLEMENTACIÓN DEL ESTUDIANTE
  // =========================================================================

  /**
   * @brief Comportamiento del técnico para el Nivel 0.
   * @param sensores Datos actuales de los sensores.
   * @return Acción a realizar.
   */
  Action ComportamientoTecnicoNivel_0(Sensores sensores);

  /**
   * @brief Comportamiento del técnico para el Nivel 1.
   * @param sensores Datos actuales de los sensores.
   * @return Acción a realizar.
   */
  Action ComportamientoTecnicoNivel_1(Sensores sensores);

  Action ComportamientoTecnicoNivel_E(Sensores sensores);

  /**
   * @brief Comportamiento del técnico para el Nivel 2.
   * @param sensores Datos actuales de los sensores.
   * @return Acción a realizar.
   */
  Action ComportamientoTecnicoNivel_2(Sensores sensores);

  /**
   * @brief Comportamiento del técnico para el Nivel 3.
   * @param sensores Datos actuales de los sensores.
   * @return Acción a realizar.
   */
  Action ComportamientoTecnicoNivel_3(Sensores sensores);

  /**
   * @brief Comportamiento del técnico para el Nivel 4.
   * @param sensores Datos actuales de los sensores.
   * @return Acción a realizar.
   */
  Action ComportamientoTecnicoNivel_4(Sensores sensores);

  /**
   * @brief Comportamiento del técnico para el Nivel 5.
   * @param sensores Datos actuales de los sensores.
   * @return Acción a realizar.
   */
  Action ComportamientoTecnicoNivel_5(Sensores sensores);

  /**
   * @brief Comportamiento del técnico para el Nivel 6.
   * @param sensores Datos actuales de los sensores.
   * @return Acción a realizar.
   */
  Action ComportamientoTecnicoNivel_6(Sensores sensores);

protected:
  // =========================================================================
  // FUNCIONES PROPORCIONADAS
  // =========================================================================

  /**
   * @brief Actualiza el mapaResultado y mapaCotas con la información de los sensores.
   * IMPORTANTE: Esta función ya está implementada. Actualiza mapaResultado y mapaCotas
   * con la información de los 16 sensores.
   */
  void ActualizarMapa(Sensores sensores);

  /**
   * @brief Determina si una casilla es transitable para el técnico.
   * NOTA: El técnico puede tener reglas de transitabilidad diferentes al ingeniero.
   * @param f Fila de la casilla.
   * @param c Columna de la casilla.
   * @param tieneZapatillas Indica si el agente posee las zapatillas.
   * @return true si la casilla es transitable.
   */
  bool EsCasillaTransitableLevel0(int f, int c, bool tieneZapatillas);

  /**
   * @brief Comprueba si la casilla de delante es accesible por diferencia de altura.
   * REGLA PARA TÉCNICO: Desnivel máximo siempre 1 (independiente de zapatillas).
   * @param actual Estado actual del agente (fila, columna, orientacion).
   * @return true si el desnivel con la casilla de delante es admisible.
   */
  bool EsAccesiblePorAltura(const ubicacion &actual);

  /**
   * @brief Devuelve la posición (fila, columna) de la casilla que hay delante del agente.
   * @param actual Estado actual del agente (fila, columna, orientacion).
   * @return Estado con la fila y columna de la casilla de enfrente.
   */
  ubicacion Delante(const ubicacion &actual) const;

  /**
   * @brief Comprueba si una celda es de tipo transitable por defecto.
   * @param c Carácter que representa el tipo de superficie.
   * @return true si es camino ('C'), zapatillas ('D') o meta ('U').
   */
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

private:
  // =========================================================================
  // VARIABLES DE ESTADO (PUEDEN SER EXTENDIDAS POR EL ALUMNO)
  // =========================================================================

  Action last_action;
  bool tiene_zapatillas;
  int giro45Izq;

  int instanteActual;
  vector<vector<int>> mapaUltimoPaso;
  vector<vector<int>> nVisitas;

  // Funciones para el Nivel 0
  void RegistrarVisitaT(ubicacion actual);
  bool EsCaminoLimpioT(ubicacion destino, int idx_sensor, const Sensores &sensores);
  bool SePuedeCaminarT(ubicacion origen, int idx_sensor, const Sensores &sensores);
  int EvaluarLadoT(ubicacion origen, int idx_sensor, const Sensores &sensores);

  // Funciones para el Nivel 1
  bool es_transitable_nivel_1_T(unsigned char c) const;
  bool EsCaminoLimpioN1_T(ubicacion destino, int idx_sensor, const Sensores &sensores);
  bool SePuedeCaminarN1_T(ubicacion origen, int idx_sensor, const Sensores &sensores);
  int EvaluarLadoN1_T(ubicacion origen, int idx_sensor, const Sensores &sensores);
  int ContarDesconocidosT(const ubicacion &centro);

  // Nivel E
  bool hayPlan;      // Indica si hay una plan que ejecutar
  list<Action> plan; // Almacena el plan a realizar.
  list<Action> B_Anchura(const EstadoT &inicio, const EstadoT &final,
                         const vector<vector<unsigned char>> &terreno,
                         const vector<vector<unsigned char>> &altura);

  list<Action> B_Anchura_V2(const EstadoT &inicio, const EstadoT &final,
                            const vector<vector<unsigned char>> &terreno,
                            const vector<vector<unsigned char>> &altura);

  list<Action> A_Star_Nivel3(const EstadoT &inicio, const EstadoT &final,
                             const vector<vector<unsigned char>> &terreno,
                             const vector<vector<unsigned char>> &altura);

  // VARIABLES Y MÉTODOS PARA EL NIVEL 5 (TÉCNICO)

  int n5_estado = 0;
  int n5_target_f = -1;
  int n5_target_c = -1;
  std::list<Action> n5_plan_movimiento;

  Action MoverA_Tecnico(int f, int c, const Sensores &sensores);

  int n5_espera = 0;
  int n5_obs_f = -1;
  int n5_obs_c = -1;

  int n5_espera_path = 0; // Tiempo de espera para evitar bloqueos del algoritmo

  // VARIABLES Y MÉTODOS PARA EL NIVEL 6 (TÉCNICO)

  int n6_estado = 0;
  bool n6T;
};

#endif
