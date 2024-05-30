#include<stdio.h>
#include<stdlib.h>
#include<time.h>
#define WALL '#' // znak oznaczający ścianę
#define PATH '.' // znak oznaczający ścieżkę
#define START 'S' // znak oznaczający start
#define END 'E' // znak oznaczający koniec
#define MARK '*' // znak oznaczający znacznik
typedef struct point{ // struktura przechowująca współrzędne punktu
    int x;
    int y;
    
}
point;
point start; // zmienna przechowująca punkt startowy
point end; // zmienna przechowująca punkt końcowy
int is_valid(int x,int y,int width,int height){ // funkcja sprawdzająca, czy punkt jest w granicach labiryntu
    return x>=0&&x<width&&y>=0&&y<height;
    
}
    int count_wall_neighbors(char**maze,point p,int width,int height){ // funkcja licząca liczbę sąsiadów będących ścianami dla danego punktu
        int count=0;
        int dx[]={-1,0,1,0}; // tablica przechowująca przesunięcia w osi x
        int dy[]={0,-1,0,1}; // tablica przechowująca przesunięcia w osi y
        for(int i=0;i<4;i++){ // pętla po czterech kierunkach
            int nx=p.x+dx[i]; // nowa współrzędna x
            int ny=p.y+dy[i]; // nowa współrzędna y
            if(is_valid(nx,ny,width,height)&&maze[ny][nx]==WALL){ // jeśli punkt jest w granicach i jest ścianą
                count++; // zwiększ licznik
            
        }
        }
        return count;} // zwróć liczbę sąsiadów będących ścianami
        void generate_maze(char**maze,point p,int width,int height){ // funkcja generująca labirynt za pomocą algorytmu DFS
            maze[p.y][p.x]=PATH; // oznacz punkt jako ścieżkę
            int order[]={0,1,2,3}; // tablica przechowująca kolejność odwiedzania sąsiadów
            for(int i=0;i<4;i++){ // pętla losująca kolejność
                int j=rand()%4; // losowy indeks
                int temp=order[i]; // zamiana miejscami
                order[i]=order[j];
                order[j]=temp;
                
            }
            int dx[]={-1,0,1,0}; // tablica przechowująca przesunięcia w osi x
            int dy[]={0,-1,0,1}; // tablica przechowująca przesunięcia w osi y
            for(int i=0;i<4;i++){ // pętla po czterech kierunkach
                int nx=p.x+dx[order[i]]; // nowa współrzędna x
                int ny=p.y+dy[order[i]]; // nowa współrzędna y
                point np={nx,ny}; // nowy punkt
                if(is_valid(nx,ny,width,height)&&count_wall_neighbors(maze,np,width,height)>=3){ // jeśli punkt jest w granicach i ma co najmniej trzech sąsiadów będących ścianami
                    generate_maze(maze,np,width,height); // rekurencyjnie odwiedź punkt
                    
                }
                
            }
            
        } // zakończ funkcję

       void print_maze(char**maze,int width,int height){ // funkcja wyświetlająca labirynt na ekranie
            for(int y=0;y<height;y++){ // pętla po wierszach
                for(int x=0;x<width;x++){ // pętla po kolumnach
                    printf("%c ",maze[y][x]); // wydrukuj znak
                    
                }
                printf("\n"); // przejdź do nowej linii
                
            }
            
        }
        void free_maze(char**maze,int height){ // funkcja zwalniająca pamięć labiryntu
            for(int y=0;y<height;y++){ // pętla po wierszach
                free(maze[y]); // zwolnij pamięć wiersza
                
            }
            free(maze); // zwolnij pamięć tablicy
            
        }
typedef struct node { // struktura przechowująca węzeł listy sąsiedztwa
    int x; // współrzędna x węzła
    int y; // współrzędna y węzła
    struct node* next; // wskaźnik na następny węzeł w liście
    
} node;
typedef struct graph { // struktura przechowująca graf
    int V; // liczba wierzchołków
    node** adj; // tablica list sąsiedztwa
    
} graph;
graph* create_graph(int V) { // funkcja tworząca graf
    graph* g = (graph*)malloc(sizeof(graph)); // alokacja pamięci dla grafu
    g->V = V; // ustawienie liczby wierzchołków
    g->adj = (node**)malloc(V * sizeof(node*)); // alokacja pamięci dla tablicy list sąsiedztwa
    for (int i = 0; i < V; i++) { // pętla po wierzchołkach
        g->adj[i] = NULL; // ustawienie listy sąsiedztwa na pustą
        
    }
    return g; // zwrócenie grafu
    
}
void add_edge(graph* g, int u, int v) { // funkcja dodająca krawędź skierowaną od wierzchołka u do wierzchołka v
    node* new_node = (node*)malloc(sizeof(node)); // alokacja pamięci dla nowego węzła
    new_node->x = v / g->V; // obliczenie współrzędnej x węzła
    new_node->y = v % g->V; // obliczenie współrzędnej y węzła
    new_node->next = g->adj[u]; // ustawienie wskaźnika na następny węzeł na głowę listy sąsiedztwa u
    g->adj[u] = new_node; // ustawienie nowego węzła na głowę listy sąsiedztwa u
    
} // zakończ funkcję

graph* generate_graph(char** maze, int width, int height) { // funkcja tworząca graf na podstawie labiryntu
    graph* g = create_graph(width * height); // utworzenie grafu o liczbie wierzchołków równej iloczynowi szerokości i wysokości labiryntu
    for (int i = 0; i < height; i++) { // pętla po wierszach labiryntu
        for (int j = 0; j < width; j++) { // pętla po kolumnach labiryntu
            if (maze[i][j] == PATH || maze[i][j] == START || maze[i][j] == END) { // jeśli komórka labiryntu jest ścieżką, startem lub końcem
                int u = i * width + j; // obliczenie indeksu wierzchołka odpowiadającego komórce
                if (i > 0 && (maze[i-1][j] == PATH || maze[i-1][j] == START || maze[i-1][j] == END)) { // jeśli komórka nad jest ścieżką, startem lub końcem
                    int v = (i-1) * width + j; // obliczenie indeksu wierzchołka odpowiadającego komórce nad
                    add_edge(g, u, v); // dodanie krawędzi od u do v
                    
                }
                if (i < height - 1 && (maze[i+1][j] == PATH || maze[i+1][j] == START || maze[i+1][j] == END)) { // jeśli komórka pod jest ścieżką, startem lub końcem
                    int v = (i+1) * width + j; // obliczenie indeksu wierzchołka odpowiadającego komórce pod
                    add_edge(g, u, v); // dodanie krawędzi od u do v
                    
                }
                if (j > 0 && (maze[i][j-1] == PATH || maze[i][j-1] == START || maze[i][j-1] == END)) { // jeśli komórka po lewej jest ścieżką, startem lub końcem
                    int v = i * width + (j-1); // obliczenie indeksu wierzchołka odpowiadającego komórce po lewej
                    add_edge(g, u, v); // dodanie krawędzi od u do v
                    
                }
                if (j < width - 1 && (maze[i][j+1] == PATH || maze[i][j+1] == START || maze[i][j+1] == END)) { // jeśli komórka po prawej jest ścieżką, startem lub końcem
                    int v = i * width + (j+1); // obliczenie indeksu wierzchołka odpowiadającego komórce po prawej
                    add_edge(g, u, v); // dodanie krawędzi od u do v
                    
                }
                
            }
            
        }
        
    }
    return g; // zwrócenie grafu
    
}
int* visited; // tablica przechowująca informację, czy wierzchołek został odwiedzony
int* dist; // tablica przechowująca odległość od wierzchołka startowego
int* prev; // tablica przechowująca poprzednika wierzchołka na najkrótszej ścieżce
void init_arrays(int V) { // funkcja inicjalizująca tablice
    visited = (int*)malloc(V * sizeof(int)); // alokacja pamięci dla tablicy visited
    dist = (int*)malloc(V * sizeof(int)); // alokacja pamięci dla tablicy dist
    prev = (int*)malloc(V * sizeof(int)); // alokacja pamięci dla tablicy prev
    for (int i = 0; i < V; i++) { // pętla po wierzchołkach
        visited[i] = 0; // ustawienie wartości na 0 (nieodwiedzony)
        dist[i] = 999; // ustawienie wartości na 999 (nieskończoność)
        prev[i] = -1; // ustawienie wartości na -1 (brak poprzednika)
        
    }
    
}
int find_min(int V) { // funkcja znajdująca nieodwiedzony wierzchołek o najmniejszej odległości
    int min = 999; // zmienna przechowująca minimalną odległość
    int min_index = -1; // zmienna przechowująca indeks minimalnego wierzchołka
    for (int i = 0; i < V; i++) { // pętla po wierzchołkach
        if (!visited[i] && dist[i] < min) { // jeśli wierzchołek jest nieodwiedzony i ma mniejszą odległość niż dotychczasowa minimalna
            min = dist[i]; // uaktualnienie minimalnej odległości
            min_index = i; // uaktualnienie indeksu minimalnego wierzchołka
            
        }
        
    }
    return min_index; // zwrócenie indeksu minimalnego wierzchołka lub -1, jeśli nie ma takiego
    
}
void dijkstra(graph* g, int s, int e) { // funkcja znajdująca najkrótszą ścieżkę w grafie za pomocą algorytmu Dijkstry
    dist[s] = 0; // ustawienie odległości wierzchołka startowego na 0
    int u = find_min(g->V); // znalezienie nieodwiedzonego wierzchołka o najmniejszej odległości
    while (u != -1) { // pętla dopóki taki wierzchołek istnieje
        visited[u] = 1; // oznaczenie wierzchołka jako odwiedzonego
        if (u == e) { // jeśli wierzchołek jest końcowy
            break; // przerwanie pętli
            
        }
        node* curr = g->adj[u]; // ustawienie wskaźnika na głowę listy sąsiedztwa u
        while (curr != NULL) { // pętla po liście sąsiedztwa u
            int v = curr->x * g->V + curr->y; // obliczenie indeksu wierzchołka sąsiadującego z u
            if (!visited[v] && dist[u] + 1 < dist[v]) { // jeśli wierzchołek jest nieodwiedzony i ma większą odległość niż u plus 1
                dist[v] = dist[u] + 1; // uaktualnienie odległości wierzchołka v
                prev[v] = u; // ustawienie poprzednika wierzchołka v na u
                
            }
            curr = curr->next; // przejście do następnego węzła w liście
            
        }
        u = find_min(g->V); // znalezienie kolejnego nieodwiedzonego wierzchołka o najmniejszej odległości
        
    }
    
} // zakończ funkcję

void mark_path(char** maze, int width, int height, int s, int e) { // funkcja zaznaczająca najkrótszą ścieżkę w labiryncie za pomocą znaku MARK
    int u = e; // ustawienie aktualnego wierzchołka na końcowy
    while (u != s) { // pętla dopóki aktualny wierzchołek nie jest startowy
        int x = u / width; // obliczenie współrzędnej x komórki odpowiadającej wierzchołkowi
        int y = u % width; // obliczenie współrzędnej y komórki odpowiadającej wierzchołkowi
        if (maze[x][y] != START && maze[x][y] != END) { // jeśli komórka nie jest startem ani końcem
            maze[x][y] = MARK; // zastąp znak komórki znakiem MARK
            
        }
        u = prev[u]; // przejdź do poprzednika wierzchołka na najkrótszej ścieżce
        
    }
    
}
int main(){ // główna funkcja programu
    srand(time(NULL)); // ustawienie ziarna generatora liczb losowych
    int width,height; // zmienne przechowujące szerokość i wysokość labiryntu
    printf("Podaj liczbę kolumn labiryntu: "); // wyświetlenie komunikatu
    scanf("%d",&width); // wczytanie szerokości od użytkownika
    printf("Podaj liczbę wierszy labiryntu: "); // wyświetlenie komunikatu
    scanf("%d",&height); // wczytanie wysokości od użytkownika
   
  // Sprawdź, czy szerokość i wysokość są liczbami całkowitymi dodatnimi
  char width_str[10], height_str[10];
  sprintf(width_str, "%d", width); // Konwertuj szerokość na ciąg znaków
  sprintf(height_str, "%d", height); // Konwertuj wysokość na ciąg znaków
  for (int i = 0; i < strlen(width_str); i++) { // Sprawdź każdy znak w ciągu szerokości
    if (width_str[i] < '0' || width_str[i] > '9') { // Jeśli znak nie jest cyfrą
      printf("Wymiary labiryntu muszą być liczbami całkowitymi dodatnimi.\n");
      return 0;
    }
  }
  for (int i = 0; i < strlen(height_str); i++) { // Sprawdź każdy znak w ciągu wysokości
    if (height_str[i] < '0' || height_str[i] > '9') { // Jeśli znak nie jest cyfrą
      printf("Wymiary labiryntu muszą być liczbami całkowitymi dodatnimi.\n");
      return 0;
    }
  }
  // Jeśli szerokość lub wysokość nie są liczbami całkowitymi dodatnimi, zwróć 0
  if (width <= 0 || height <= 0) {
    printf("Wymiary labiryntu muszą być liczbami całkowitymi dodatnimi.\n");
    return 0;
  }
  // Jeśli szerokość lub wysokość są liczbami parzystymi, zwróć 0
  if (width % 2 == 0 || height % 2 == 0) {
    printf("Wymiary labiryntu muszą być liczbami nieparzystymi.\n");
    return 0;
  }
  // Jeśli szerokość lub wysokość są mniejsze niż 3, zwróć 0
  if (width < 3 || height < 3) {
    printf("Wymiary labiryntu muszą być większe lub równe 3.\n");
    return 0;
  }
  // Jeśli szerokość lub wysokość są większe niż 99, zwróć 0
  if (width > 99 || height > 99) {
    printf("Wymiary labiryntu muszą być mniejsze lub równe 99.\n");
    return 0;
  }
  

    char**maze=malloc(height*sizeof(char*)); // alokacja pamięci dla tablicy labiryntu
    for(int y=0;y<height;y++){ // pętla po wierszach labiryntu
        maze[y]=malloc(width*sizeof(char)); // alokacja pamięci dla wiersza labiryntu
        for(int x=0;x<width;x++){ // pętla po kolumnach labiryntu
            maze[y][x]=WALL; // ustawienie znaku komórki na WALL
            
        }
        
    }
    start.x=rand()%(width/2)*2+1; // losowanie współrzędnej x punktu startowego
    start.y=0; // ustawienie współrzędnej y punktu startowego na 0
    end.x=rand()%(width/2)*2+1; // losowanie współrzędnej x punktu końcowego
    end.y=height-1; // ustawienie współrzędnej y punktu końcowego na ostatni wiersz
    generate_maze(maze,start,width,height); // generowanie labiryntu za pomocą algorytmu DFS
    maze[start.y][start.x]=START; // ustawienie znaku komórki startowej na START
    maze[end.y][end.x]=END; // ustawienie znaku komórki końcowej na END
    printf("Oto wygenerowany labirynt:\n"); // wyświetlenie komunikatu
    print_maze(maze,width,height); // wyświetlenie labiryntu na ekranie
    graph* g = generate_graph(maze, width, height); // tworzenie grafu na podstawie labiryntu
    init_arrays(width * height); // inicjalizacja tablic pomocniczych
    int s = start.y * width + start.x; // obliczenie indeksu wierzchołka startowego
    int e = end.y * width + end.x; // obliczenie indeksu wierzchołka końcowego
    dijkstra(g, s, e); // znajdowanie najkrótszej ścieżki w grafie za pomocą algorytmu Dijkstry
    if (dist[e] != 999) { // jeśli istnieje rozwiązanie
        printf("\nLabirynt ma rozwiązanie.\n\n"); // wyświetlenie komunikatu
        printf("Najkrótsza ścieżka ma długość %d.\n\n", dist[e]+1); // wyświetlenie długości najkrótszej ścieżki
        mark_path(maze, width, height, s,e); // zaznaczenie najkrótszej ścieżki w labiryncie
        printf("Oto labirynt z zaznaczoną najkrótszą ścieżką:\n"); // wyświetlenie komunikatu
        print_maze(maze, width, height); // wyświetlenie labiryntu z zaznaczoną ścieżką
        
    }
    else { // jeśli nie istnieje rozwiązanie
        printf("\nLabirynt nie ma rozwiązania.\n"); // wyświetlenie komunikatu
        
    }
    free_maze(maze,height); // zwolnienie pamięci labiryntu
    free(visited); // zwolnienie pamięci tablicy visited
    free(dist); // zwolnienie pamięci tablicy dist
    free(prev); // zwolnienie pamięci tablicy prev
    free(g->adj); // zwolnienie pamięci tablicy list sąsiedztwa
    free(g); // zwolnienie pamięci grafu
    return 0; // zakończenie programu
    
} // zakończenie funkcji
