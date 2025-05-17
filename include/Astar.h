#pragma once
#include <utility>
#include <vector>

//Definimos el numero de filas y columnas
#define ROW 14
#define COL 14

//Creamos un shortcut para int, int 
typedef std::pair<int,int> Pair;

//Creamos un shortcut para pair<double, pair<int, int>>
typedef std::pair<double, std::pair<int, int>> pPair;

//Creamos una estructura que almacene los detalles de cada celda
struct cell{

    int parent_i, parent_j; // Nodo padre
    double f, g, h; // Costos
};

//Para ver si la celda es valida o no
bool isValid(int row, int col);

//Para ver si la celda esta bloqueada o no
bool isUnBlocked(std::vector<std::vector<int>> grid, int row, int col);

//Para saber si la celda destino ha sido alcanzada o no
bool isDest(int row, int col, Pair dest);

//Para calcular el valor de H
double calcHValue(int row, int col, Pair dest);

//Para calcular el camino desde el origen hasta el destino
std::vector<Pair> tracePath(cell cellDetails[][COL], Pair dest);

//Algoritmo A* para encontrar el camino mas corto
std::vector<Pair> aStarSearch(std::vector<std::vector<int>> grid, Pair src, Pair dest);