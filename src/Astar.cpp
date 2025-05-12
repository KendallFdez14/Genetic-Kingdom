#include "Astar.h"
#include<bits/stdc++.h>
#include<vector>


//Para ver si la celda es valida o no
bool isValid(int row, int col){

    //Devuelve true si la celda esta en el rango del mapa
    return (row >= 0) && (row < ROW) && (col>= 0) && (col < COL);

};

//Para ver si la celda esta bloqueada o no
bool isUnBlocked(std::vector<std::vector<int>> grid, int row, int col){

    //Devuelve true si la celda no es un obstaculo
    if (grid[row][col] == 1)
        return (true);
    else
        return (false);
};

//Para saber si la celda destino ha sido alcanzada o no
bool isDest(int row, int col, Pair dest){

    if (row == dest.first && col == dest.second)
        return (true);
    else
        return (false);

};

//Para calcular el valor de H
double calcHValue(int row, int col, Pair dest){

    return ((double)sqrt(
        (row - dest.first) * (row - dest.first) 
        + (col - dest.second) * (col - dest.second)));

};

//Para calcular el camino desde el origen hasta el destino
std::vector<Pair> tracePath(cell cellDetails[][COL], Pair dest){

    std::vector<Pair> path;

    
    int row = dest.first;
    int col = dest.second;

    std::stack<Pair> Path;

    while (!(cellDetails[row][col].parent_i == row
            && cellDetails[row][col].parent_j == col)){

                Path.push(std::make_pair(row, col));
                int temp_row = cellDetails[row][col].parent_i;
                int temp_col = cellDetails[row][col].parent_j;

                row = temp_row;
                col = temp_col;

    };
    Path.push(std::make_pair(row, col));

    while (!Path.empty()) {
        auto [r, c] = Path.top();
        Path.pop();
        // Invertir a {col, row}
        path.push_back(std::make_pair(c, r));
    };

    return path;

};

//Algoritmo A* para encontrar el camino mas corto
std::vector<Pair> aStarSearch(std::vector<std::vector<int>> grid, Pair src, Pair dest){

    // Si el nodo de origen no es valido
    if (isValid(src.first, src.second) == false){
        printf("Source is invalid\n");
        return {};
    }

    //Si el nodo destino no es valido
    if (isValid(dest.first, dest.second) == false){
        printf("Destination is invalid\n");
        return {};
    }

    // Si el nodo de origen esta bloqueado
    if (isUnBlocked(grid, src.first, src.second) == false){
        printf("Source is blocked\n");
        return {};
    }

    //Si el nodo destino esta bloqueado
    if (isUnBlocked(grid, dest.first, dest.second) == false){
        printf("Destination is blocked\n");
        return {};
    }

    // Si el nodo de origen y destino son iguales
    if (isDest(src.first, src.second, dest) == true){
        printf("We are already at the destination\n");
        return {};
    }

    //Definimos el peso de cada movimiento
    //Vertical, Horizontal y Diagonal
    double peso_vert = 1.0;
    double peso_hor = 1.0;
    double peso_diag = 3.0;

    //Creamos la "closed list" como una matriz que guarda los nodos ya visitados
    bool closedList[ROW][COL];
    memset(closedList, false, sizeof(closedList));
    
    //Creamos una matriz para guardar los detalles de cada celda
    cell cellDetails[ROW][COL];

    int i, j;

    for (int i = 0; i < ROW; i++){
        for (int j = 0; j < COL; j++){
            cellDetails[i][j].f = FLT_MAX;
            cellDetails[i][j].g = FLT_MAX;
            cellDetails[i][j].h = FLT_MAX;
            cellDetails[i][j].parent_i = -1;
            cellDetails[i][j].parent_j = -1;
            
        };
    };

    //Inicializamos el nodo de origen
    i = src.first, j = src.second;
    cellDetails[i][j].f = 0.0;
    cellDetails[i][j].g = 0.0;
    cellDetails[i][j].h = 0.0;
    cellDetails[i][j].parent_i = i;
    cellDetails[i][j].parent_j = j;

    //Creamos la "open list" como <f, <i, j>>
    //donde f = g + h
    std::set<pPair> openList;
    //f=0 para el nodo de origen
    openList.insert(std::make_pair(0.0, std::make_pair(i, j)));

    //Mientras no se haya alcanzado el destino
    bool foundDest = false;

    while(!openList.empty()){

        //Obtenemos el primer elemento de la open list
        pPair p = *openList.begin();

        //Eliminamos el primer elemento de la open list
        openList.erase(openList.begin());

        //Obtenemos las coordenadas del nodo
        i = p.second.first;
        j = p.second.second;
        closedList[i][j] = true; //Marcamos el nodo como visitado

        /*
        Generamos los 8 sucerores del nodo actual, donde:cellDetails

        N = North = (i-1, j)
        S = South = (i+1, j)
        E = East = (i, j+1)
        W = West = (i, j-1)
        NE = North-East = (i-1, j+1)
        NW = North-West = (i-1, j-1)
        SE = South-East = (i+1, j+1)
        SW = South-West = (i+1, j-1)
        
        */

        //Para almacenar g, h, y f de los sucesores
        double gNew, hNew, fNew;

        //Primer sucesor (N)

        //Si es valida
        if (isValid(i - 1, j) == true){

            //Si el nodo destino es este sucesor
            if (isDest(i - 1, j, dest) == true){
                //Seteamos el padre del nodo destino
                cellDetails[i - 1][j].parent_i = i;
                cellDetails[i - 1][j].parent_j = j;
                printf("The destination node is found\n");
                
                foundDest = true;
                return tracePath(cellDetails, dest);
            }

            //Si el sucesor esta en la closed list o esta bloqueado, lo ignoramos
            else if (closedList[i - 1][j] == false && isUnBlocked(grid, i - 1, j) == true) {
                gNew = cellDetails[i][j].g + peso_vert; //"Peso" del movimiento vertical
                hNew = calcHValue(i - 1, j, dest);
                fNew = gNew + hNew;

                //Si el nodo no esta en la open list, se agrega
                //o si el nodo esta en la open list, se verifica su valor
                //de f para ver si este camino es el mejor

                if (cellDetails[i - 1 ][j].f == FLT_MAX || cellDetails[i - 1][j].f > fNew) {

                    openList.insert(std::make_pair(fNew, std::make_pair(i - 1, j)));

                    //Actualizamos los detalles de la celda
                    cellDetails[i - 1][j].f = fNew;
                    cellDetails[i - 1][j].g = gNew;
                    cellDetails[i - 1][j].h = hNew;
                    cellDetails[i - 1][j].parent_i = i;
                    cellDetails[i - 1][j].parent_j = j;

                }

            }

        }

        //Segundo sucesor (S)

        //Si es valida
        if(isValid(i + 1, j) == true){

            //Si el nodo destino es este sucesor
            if (isDest(i + 1, j, dest) == true){
                //Seteamos el padre del nodo destino
                cellDetails[i + 1][j].parent_i = i;
                cellDetails[i + 1][j].parent_j = j;
                printf("The destination node is found\n");
                
                foundDest = true;
                return tracePath(cellDetails, dest);
            }

            //Si el sucesor esta en la closed list o esta bloqueado, lo ignoramos
            else if (closedList[i + 1][j] == false && isUnBlocked(grid, i + 1, j) == true) {
                gNew = cellDetails[i][j].g + peso_vert; //"Peso" del movimiento vertical
                hNew = calcHValue(i + 1, j, dest);
                fNew = gNew + hNew;

                //Si el nodo no esta en la open list, se agrega
                //o si el nodo esta en la open list, se verifica su valor
                //de f para ver si este camino es el mejor

                if (cellDetails[i + 1 ][j].f == FLT_MAX || cellDetails[i + 1][j].f > fNew) {

                    openList.insert(std::make_pair(fNew, std::make_pair(i + 1, j)));

                    //Actualizamos los detalles de la celda
                    cellDetails[i + 1][j].f = fNew;
                    cellDetails[i + 1][j].g = gNew;
                    cellDetails[i + 1][j].h = hNew;
                    cellDetails[i + 1][j].parent_i = i;
                    cellDetails[i + 1][j].parent_j = j;

                }

            }

        }

        //Tercer sucesor (E)

        //Si es valida
        if (isValid(i, j + 1) == true){
            //Si el nodo destino es este sucesor
            if (isDest(i, j + 1, dest) == true){
                //Seteamos el padre del nodo destino
                cellDetails[i][j + 1].parent_i = i;
                cellDetails[i][j + 1].parent_j = j;
                printf("The destination node is found\n");
                
                foundDest = true;
                return tracePath(cellDetails, dest);
            }

            //Si el sucesor esta en la closed list o esta bloqueado, lo ignoramos
            else if (closedList[i][j + 1] == false && isUnBlocked(grid, i, j + 1) == true) {
                gNew = cellDetails[i][j].g + peso_hor; //"Peso" del movimiento horizontal
                hNew = calcHValue(i, j + 1, dest);
                fNew = gNew + hNew;

                //Si el nodo no esta en la open list, se agrega
                //o si el nodo esta en la open list, se verifica su valor
                //de f para ver si este camino es el mejor

                if (cellDetails[i][j + 1].f == FLT_MAX || cellDetails[i][j + 1].f > fNew) {

                    openList.insert(std::make_pair(fNew, std::make_pair(i, j + 1)));

                    //Actualizamos los detalles de la celda
                    cellDetails[i][j + 1].f = fNew;
                    cellDetails[i][j + 1].g = gNew;
                    cellDetails[i][j + 1].h = hNew;
                    cellDetails[i][j + 1].parent_i = i;
                    cellDetails[i][j + 1].parent_j = j;

                }

            }

        }

        //Cuarto sucesor (W)

        //Si es valida
        if (isValid(i, j - 1) == true){
            //Si el nodo destino es este sucesor
            if (isDest(i, j - 1, dest) == true){
                //Seteamos el padre del nodo destino
                cellDetails[i][j - 1].parent_i = i;
                cellDetails[i][j - 1].parent_j = j;
                printf("The destination node is found\n");
                
                foundDest = true;
                return tracePath(cellDetails, dest);
            }

            //Si el sucesor esta en la closed list o esta bloqueado, lo ignoramos
            else if (closedList[i][j - 1] == false && isUnBlocked(grid, i, j - 1) == true) {
                gNew = cellDetails[i][j].g + peso_hor; //"Peso" del movimiento horizontal
                hNew = calcHValue(i, j - 1, dest);
                fNew = gNew + hNew;

                //Si el nodo no esta en la open list, se agrega
                //o si el nodo esta en la open list, se verifica su valor
                //de f para ver si este camino es el mejor

                if (cellDetails[i][j - 1].f == FLT_MAX || cellDetails[i][j - 1].f > fNew) {

                    openList.insert(std::make_pair(fNew, std::make_pair(i, j - 1)));

                    //Actualizamos los detalles de la celda
                    cellDetails[i][j - 1].f = fNew;
                    cellDetails[i][j - 1].g = gNew;
                    cellDetails[i][j - 1].h = hNew;
                    cellDetails[i][j - 1].parent_i = i;
                    cellDetails[i][j - 1].parent_j = j;

                }

            }

        }

        //Quinto sucesor (NE)

        //Si es valida
        if (isValid(i - 1, j + 1) == true){
            //Si el nodo destino es este sucesor
            if (isDest(i - 1, j + 1, dest) == true){
                //Seteamos el padre del nodo destino
                cellDetails[i - 1][j + 1].parent_i = i;
                cellDetails[i - 1][j + 1].parent_j = j;
                printf("The destination node is found\n");
                
                foundDest = true;
                return tracePath(cellDetails, dest);
            }

            //Si el sucesor esta en la closed list o esta bloqueado, lo ignoramos
            else if (closedList[i - 1][j + 1] == false && isUnBlocked(grid, i - 1, j + 1) == true) {
                gNew = cellDetails[i][j].g + peso_diag; //Diagonal, mas "peso"
                hNew = calcHValue(i - 1, j + 1, dest);
                fNew = gNew + hNew;

                //Si el nodo no esta en la open list, se agrega
                //o si el nodo esta en la open list, se verifica su valor
                //de f para ver si este camino es el mejor

                if (cellDetails[i - 1 ][j + 1].f == FLT_MAX || cellDetails[i - 1][j + 1].f > fNew) {

                    openList.insert(std::make_pair(fNew, std::make_pair(i - 1, j + 1)));

                    //Actualizamos los detalles de la celda
                    cellDetails[i - 1][j + 1].f = fNew;
                    cellDetails[i - 1][j + 1].g = gNew;
                    cellDetails[i - 1][j + 1].h = hNew;
                    cellDetails[i - 1][j + 1].parent_i = i;
                    cellDetails[i - 1][j + 1].parent_j = j;

                }

            }

        }

        //Sexto sucesor (NW)

        //Si es valida
        if (isValid(i - 1, j - 1) == true){
            //Si el nodo destino es este sucesor
            if (isDest(i - 1, j - 1, dest) == true){
                //Seteamos el padre del nodo destino
                cellDetails[i - 1][j - 1].parent_i = i;
                cellDetails[i - 1][j - 1].parent_j = j;
                printf("The destination node is found\n");
                
                foundDest = true;
                return tracePath(cellDetails, dest);
            }

            //Si el sucesor esta en la closed list o esta bloqueado, lo ignoramos
            else if (closedList[i - 1][j - 1] == false && isUnBlocked(grid, i - 1, j - 1) == true) {
                gNew = cellDetails[i][j].g + peso_diag; //Diagonal, mas "peso"
                hNew = calcHValue(i - 1, j - 1, dest);
                fNew = gNew + hNew;

                //Si el nodo no esta en la open list, se agrega
                //o si el nodo esta en la open list, se verifica su valor
                //de f para ver si este camino es el mejor

                if (cellDetails[i - 1][j - 1].f == FLT_MAX || cellDetails[i - 1][j - 1].f > fNew) {

                    openList.insert(std::make_pair(fNew, std::make_pair(i - 1, j - 1)));

                    //Actualizamos los detalles de la celda
                    cellDetails[i - 1][j - 1].f = fNew;
                    cellDetails[i - 1][j - 1].g = gNew;
                    cellDetails[i - 1][j - 1].h = hNew;
                    cellDetails[i - 1][j - 1].parent_i = i;
                    cellDetails[i - 1][j - 1].parent_j = j;

                }

            }

        }

        //Septimo sucesor (SE)

        //Si es valida
        if (isValid(i + 1, j + 1) == true){
            //Si el nodo destino es este sucesor
            if (isDest(i + 1, j + 1, dest) == true){
                //Seteamos el padre del nodo destino
                cellDetails[i + 1][j + 1].parent_i = i;
                cellDetails[i + 1][j + 1].parent_j = j;
                printf("The destination node is found\n");
                
                foundDest = true;
                return tracePath(cellDetails, dest);
            }

            //Si el sucesor esta en la closed list o esta bloqueado, lo ignoramos
            else if (closedList[i + 1][j + 1] == false && isUnBlocked(grid, i + 1, j + 1) == true) {
                gNew = cellDetails[i][j].g + peso_diag; //Diagonal, mas "peso"
                hNew = calcHValue(i + 1, j + 1, dest);
                fNew = gNew + hNew;

                //Si el nodo no esta en la open list, se agrega
                //o si el nodo esta en la open list, se verifica su valor
                //de f para ver si este camino es el mejor

                if (cellDetails[i + 1][j + 1].f == FLT_MAX || cellDetails[i + 1][j + 1].f > fNew) {

                    openList.insert(std::make_pair(fNew, std::make_pair(i + 1, j + 1)));

                    //Actualizamos los detalles de la celda
                    cellDetails[i + 1][j + 1].f = fNew;
                    cellDetails[i + 1][j + 1].g = gNew;
                    cellDetails[i + 1][j + 1].h = hNew;
                    cellDetails[i + 1][j + 1].parent_i = i;
                    cellDetails[i + 1][j + 1].parent_j = j;

                }

            }

        }

        //Octavo sucesor (SW)

        //Si es valida
        if (isValid(i + 1, j - 1) == true){
            //Si el nodo destino es este sucesor
            if (isDest(i + 1, j - 1, dest) == true){
                //Seteamos el padre del nodo destino
                cellDetails[i + 1][j - 1].parent_i = i;
                cellDetails[i + 1][j - 1].parent_j = j;
                printf("The destination node is found\n");
                
                foundDest = true;
                return tracePath(cellDetails, dest);
            }

            //Si el sucesor esta en la closed list o esta bloqueado, lo ignoramos
            else if (closedList[i + 1][j - 1] == false && isUnBlocked(grid, i + 1, j - 1) == true) {
                gNew = cellDetails[i][j].g + peso_diag; //Diagonal, mas "peso"
                hNew = calcHValue(i + 1, j - 1, dest);
                fNew = gNew + hNew;

                //Si el nodo no esta en la open list, se agrega
                //o si el nodo esta en la open list, se verifica su valor
                //de f para ver si este camino es el mejor

                if (cellDetails[i + 1 ][j - 1].f == FLT_MAX || cellDetails[i + 1][j - 1].f > fNew) {

                    openList.insert(std::make_pair(fNew, std::make_pair(i + 1, j - 1)));

                    //Actualizamos los detalles de la celda
                    cellDetails[i + 1][j - 1].f = fNew;
                    cellDetails[i + 1][j - 1].g = gNew;
                    cellDetails[i + 1][j - 1].h = hNew;
                    cellDetails[i + 1][j - 1].parent_i = i;
                    cellDetails[i + 1][j - 1].parent_j = j;

                }

            }

        }
    };

    //Si no se ha encontrado el destino y la open list esta vacia
    if (foundDest == false)
        printf("Failed to find the Destination Cell\n");

    return {};

};

