#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <limits.h> // For INT_MAX

// --- Constants and Type Definitions ---

#define WALL_CHAR '#'
#define PATH_CHAR '.'
#define START_CHAR 'S'
#define END_CHAR 'E'
#define SOLUTION_CHAR '*'

typedef struct {
    int x;
    int y;
} Point;

typedef struct {
    char** grid;
    int width;
    int height;
    Point start;
    Point end;
} Maze;

typedef struct Node {
    int vertex;
    struct Node* next;
} Node;

typedef struct {
    int num_vertices;
    Node** adj_lists;
} Graph;

// --- Function Prototypes ---

// Maze functions
Maze* create_maze(int width, int height);
void free_maze(Maze* maze);
void generate_maze_recursive(Maze* maze, Point p);
void print_maze(const Maze* maze);
int is_valid(int x, int y, int width, int height);

// Graph functions
Graph* create_graph(int num_vertices);
void free_graph(Graph* graph);
void add_edge(Graph* graph, int src, int dest);
Graph* maze_to_graph(const Maze* maze);

// Solver (Dijkstra) functions
int solve_with_dijkstra(Graph* graph, int start_node, int end_node, int** prev_path);
void reconstruct_and_mark_path(Maze* maze, int start_node, int* prev_path);


// --- Main Application Logic ---

int main() {
    srand(time(NULL));

    int width, height;
    printf("Enter the width of the maze (odd, >= 5): ");
    if (scanf("%d", &width) != 1) {
        printf("Invalid input.\n");
        return EXIT_FAILURE;
    }

    printf("Enter the height of the maze (odd, >= 5): ");
    if (scanf("%d", &height) != 1) {
        printf("Invalid input.\n");
        return EXIT_FAILURE;
    }

    if (width < 5 || height < 5 || width % 2 == 0 || height % 2 == 0) {
        printf("Error: Dimensions must be odd integers greater than or equal to 5.\n");
        return EXIT_FAILURE;
    }

    Maze* maze = create_maze(width, height);
    if (!maze) {
        perror("Failed to create maze");
        return EXIT_FAILURE;
    }
    
    Point generation_start = {1, 1};
    generate_maze_recursive(maze, generation_start);

    int start_x = rand() % (maze->width / 2) * 2 + 1;
    int end_x = rand() % (maze->width / 2) * 2 + 1;

    maze->start = (Point){start_x, 0};
    maze->end = (Point){end_x, maze->height - 1};

    maze->grid[maze->start.y][maze->start.x] = START_CHAR;
    maze->grid[maze->end.y][maze->end.x] = END_CHAR;
    
    printf("\nGenerated Maze:\n");
    print_maze(maze);

    Graph* graph = maze_to_graph(maze);
    int start_node = maze->start.y * maze->width + maze->start.x;
    int end_node = maze->end.y * maze->width + maze->end.x;

    int* prev_path = NULL;
    if (solve_with_dijkstra(graph, start_node, end_node, &prev_path)) {
        printf("\nPath found! Reconstructing path...\n");
        reconstruct_and_mark_path(maze, start_node, prev_path);
        printf("\nMaze with Solution Path:\n");
        print_maze(maze);
    } else {
        printf("\nNo path found from start to end.\n");
    }

    free(prev_path);
    free_graph(graph);
    free_maze(maze);

    return EXIT_SUCCESS;
}


// --- Function Implementations ---

Maze* create_maze(int width, int height) {
    Maze* maze = malloc(sizeof(Maze));
    if (!maze) return NULL;
    maze->width = width;
    maze->height = height;
    maze->grid = malloc(height * sizeof(char*));
    if (!maze->grid) {
        free(maze);
        return NULL;
    }
    for (int i = 0; i < height; ++i) {
        maze->grid[i] = malloc(width * sizeof(char));
        if (!maze->grid[i]) {
            for (int j = 0; j < i; ++j) free(maze->grid[j]);
            free(maze->grid);
            free(maze);
            return NULL;
        }
        memset(maze->grid[i], WALL_CHAR, width);
    }
    return maze;
}

void free_maze(Maze* maze) {
    if (!maze) return;
    for (int i = 0; i < maze->height; ++i) {
        free(maze->grid[i]);
    }
    free(maze->grid);
    free(maze);
}

void generate_maze_recursive(Maze* maze, Point p) {
    maze->grid[p.y][p.x] = PATH_CHAR;
    int directions[] = {0, 1, 2, 3};
    for (int i = 0; i < 4; ++i) {
        int r = rand() % 4;
        int temp = directions[i];
        directions[i] = directions[r];
        directions[r] = temp;
    }
    int dx[] = {0, 0, 2, -2};
    int dy[] = {-2, 2, 0, 0};
    for (int i = 0; i < 4; ++i) {
        int nx = p.x + dx[directions[i]];
        int ny = p.y + dy[directions[i]];
        if (is_valid(nx, ny, maze->width, maze->height) && maze->grid[ny][nx] == WALL_CHAR) {
            maze->grid[ny - dy[directions[i]] / 2][nx - dx[directions[i]] / 2] = PATH_CHAR;
            generate_maze_recursive(maze, (Point){nx, ny});
        }
    }
}

void print_maze(const Maze* maze) {
    for (int y = 0; y < maze->height; ++y) {
        for (int x = 0; x < maze->width; ++x) {
            printf("%c ", maze->grid[y][x]);
        }
        printf("\n");
    }
}

int is_valid(int x, int y, int width, int height) {
    return x >= 0 && x < width && y >= 0 && y < height;
}

Graph* create_graph(int num_vertices) {
    Graph* graph = malloc(sizeof(Graph));
    graph->num_vertices = num_vertices;
    graph->adj_lists = malloc(num_vertices * sizeof(Node*));
    for (int i = 0; i < num_vertices; i++) {
        graph->adj_lists[i] = NULL;
    }
    return graph;
}

void free_graph(Graph* graph) {
    if (!graph) return;
    for (int i = 0; i < graph->num_vertices; i++) {
        Node* current = graph->adj_lists[i];
        while (current) {
            Node* temp = current;
            current = current->next;
            free(temp);
        }
    }
    free(graph->adj_lists);
    free(graph);
}

void add_edge(Graph* graph, int src, int dest) {
    Node* newNode = malloc(sizeof(Node));
    newNode->vertex = dest;
    newNode->next = graph->adj_lists[src];
    graph->adj_lists[src] = newNode;
    newNode = malloc(sizeof(Node));
    newNode->vertex = src;
    newNode->next = graph->adj_lists[dest];
    graph->adj_lists[dest] = newNode;
}

Graph* maze_to_graph(const Maze* maze) {
    int V = maze->width * maze->height;
    Graph* graph = create_graph(V);
    for (int y = 0; y < maze->height; ++y) {
        for (int x = 0; x < maze->width; ++x) {
            if (maze->grid[y][x] != WALL_CHAR) {
                int u = y * maze->width + x;
                int dx[] = {0, 0, 1, -1};
                int dy[] = {1, -1, 0, 0};
                for(int i = 0; i < 4; ++i) {
                    int nx = x + dx[i];
                    int ny = y + dy[i];
                    if (is_valid(nx, ny, maze->width, maze->height) && maze->grid[ny][nx] != WALL_CHAR) {
                        int v = ny * maze->width + nx;
                        if (u < v) {
                           add_edge(graph, u, v);
                        }
                    }
                }
            }
        }
    }
    return graph;
}

// Helper function to find the vertex with the minimum distance value,
// from the set of vertices not yet included in the shortest path tree.
int find_min_distance_vertex(int* distance, int* visited, int num_vertices) {
    int min = INT_MAX, min_index = -1;
    for (int v = 0; v < num_vertices; v++) {
        if (visited[v] == 0 && distance[v] <= min) {
            min = distance[v];
            min_index = v;
        }
    }
    return min_index;
}

// Solves the maze using Dijkstra's algorithm
int solve_with_dijkstra(Graph* graph, int start_node, int end_node, int** prev_path) {
    int V = graph->num_vertices;
    int* distance = malloc(V * sizeof(int));
    int* visited = malloc(V * sizeof(int));
    *prev_path = malloc(V * sizeof(int));

    for (int i = 0; i < V; ++i) {
        distance[i] = INT_MAX;
        visited[i] = 0; // 0 means not visited
        (*prev_path)[i] = -1;
    }

    distance[start_node] = 0;

    for (int count = 0; count < V - 1; count++) {
        int u = find_min_distance_vertex(distance, visited, V);
        if (u == -1 || u == end_node) {
            break;
        }
        
        visited[u] = 1;

        Node* temp = graph->adj_lists[u];
        while (temp) {
            int v = temp->vertex;
            // Edge weight is always 1 in our maze
            if (!visited[v] && distance[u] != INT_MAX && distance[u] + 1 < distance[v]) {
                distance[v] = distance[u] + 1;
                (*prev_path)[v] = u;
            }
            temp = temp->next;
        }
    }

    int path_found = (distance[end_node] != INT_MAX);
    
    free(distance);
    free(visited);
    
    return path_found;
}

void reconstruct_and_mark_path(Maze* maze, int start_node, int* prev_path) {
    int current = maze->end.y * maze->width + maze->end.x;
    while (current != start_node && current != -1) {
        int row = current / maze->width;
        int col = current % maze->width;
        if (maze->grid[row][col] != START_CHAR && maze->grid[row][col] != END_CHAR) {
            maze->grid[row][col] = SOLUTION_CHAR;
        }
        current = prev_path[current];
    }
}
