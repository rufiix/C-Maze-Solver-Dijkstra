#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <limits.h>

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

// --- Min-Heap Structures for Dijkstra ---

typedef struct {
    int vertex;
    int distance;
} HeapNode;

typedef struct {
    int size;
    int capacity;
    int* pos;
    HeapNode* array;
} MinHeap;

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

// Min-Heap functions
MinHeap* create_min_heap(int capacity);
void free_min_heap(MinHeap* heap);
HeapNode extract_min(MinHeap* heap);
void decrease_key(MinHeap* heap, int v, int dist);
int is_in_heap(MinHeap* heap, int v);
void min_heapify(MinHeap* heap, int idx);

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
        printf("Error: Dimensions must be odd integers >= 5.\n");
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

// --- Maze Implementation ---

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
        int tmp = directions[i];
        directions[i] = directions[r];
        directions[r] = tmp;
    }
    int dx[] = {0, 0, 2, -2};
    int dy[] = {-2, 2, 0, 0};
    for (int i = 0; i < 4; ++i) {
        int nx = p.x + dx[directions[i]];
        int ny = p.y + dy[directions[i]];
        if (is_valid(nx, ny, maze->width, maze->height) && maze->grid[ny][nx] == WALL_CHAR) {
            maze->grid[p.y + dy[directions[i]] / 2][p.x + dx[directions[i]] / 2] = PATH_CHAR;
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

// --- Graph Implementation ---

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
    for (int i = 0; i < graph->num_vertices; i++) {
        Node* curr = graph->adj_lists[i];
        while (curr) {
            Node* temp = curr;
            curr = curr->next;
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
                for (int i = 0; i < 4; ++i) {
                    int nx = x + dx[i];
                    int ny = y + dy[i];
                    if (is_valid(nx, ny, maze->width, maze->height) && maze->grid[ny][nx] != WALL_CHAR) {
                        int v = ny * maze->width + nx;
                        if (u < v) add_edge(graph, u, v);
                    }
                }
            }
        }
    }
    return graph;
}

// --- Min-Heap Implementation ---

MinHeap* create_min_heap(int capacity) {
    MinHeap* heap = malloc(sizeof(MinHeap));
    heap->pos = malloc(capacity * sizeof(int));
    heap->size = 0;
    heap->capacity = capacity;
    heap->array = malloc(capacity * sizeof(HeapNode));
    return heap;
}

void free_min_heap(MinHeap* heap) {
    free(heap->pos);
    free(heap->array);
    free(heap);
}

void swap_heap_node(HeapNode* a, HeapNode* b) {
    HeapNode temp = *a;
    *a = *b;
    *b = temp;
}

void min_heapify(MinHeap* heap, int idx) {
    int smallest = idx;
    int left = 2 * idx + 1;
    int right = 2 * idx + 2;

    if (left < heap->size &&
        heap->array[left].distance < heap->array[smallest].distance)
        smallest = left;

    if (right < heap->size &&
        heap->array[right].distance < heap->array[smallest].distance)
        smallest = right;

    if (smallest != idx) {
        HeapNode smallestNode = heap->array[smallest];
        HeapNode idxNode = heap->array[idx];

        heap->pos[smallestNode.vertex] = idx;
        heap->pos[idxNode.vertex] = smallest;

        swap_heap_node(&heap->array[smallest], &heap->array[idx]);
        min_heapify(heap, smallest);
    }
}

HeapNode extract_min(MinHeap* heap) {
    if (heap->size == 0) {
        HeapNode dummy = {-1, INT_MAX};
        return dummy;
    }

    HeapNode root = heap->array[0];
    HeapNode lastNode = heap->array[heap->size - 1];
    heap->array[0] = lastNode;

    heap->pos[root.vertex] = heap->size - 1;
    heap->pos[lastNode.vertex] = 0;

    heap->size--;
    min_heapify(heap, 0);

    return root;
}

void decrease_key(MinHeap* heap, int v, int dist) {
    int i = heap->pos[v];
    heap->array[i].distance = dist;

    while (i > 0 && heap->array[i].distance < heap->array[(i - 1) / 2].distance) {
        heap->pos[heap->array[i].vertex] = (i - 1) / 2;
        heap->pos[heap->array[(i - 1) / 2].vertex] = i;
        swap_heap_node(&heap->array[i], &heap->array[(i - 1) / 2]);
        i = (i - 1) / 2;
    }
}

int is_in_heap(MinHeap* heap, int v) {
    return heap->pos[v] < heap->size;
}

// --- Dijkstra with Min-Heap ---

int solve_with_dijkstra(Graph* graph, int start_node, int end_node, int** prev_path) {
    int V = graph->num_vertices;
    int* distance = malloc(V * sizeof(int));
    *prev_path = malloc(V * sizeof(int));

    MinHeap* heap = create_min_heap(V);

    for (int v = 0; v < V; v++) {
        distance[v] = INT_MAX;
        (*prev_path)[v] = -1;
        heap->array[v].vertex = v;
        heap->array[v].distance = INT_MAX;
        heap->pos[v] = v;
    }

    distance[start_node] = 0;
    heap->array[start_node].distance = 0;
    heap->size = V;

    while (heap->size > 0) {
        HeapNode minNode = extract_min(heap);
        int u = minNode.vertex;

        if (u == end_node) break;

        Node* temp = graph->adj_lists[u];
        while (temp) {
            int v = temp->vertex;
            if (is_in_heap(heap, v) && distance[u] != INT_MAX &&
                distance[u] + 1 < distance[v]) {
                distance[v] = distance[u] + 1;
                (*prev_path)[v] = u;
                decrease_key(heap, v, distance[v]);
            }
            temp = temp->next;
        }
    }

    int path_found = (distance[end_node] != INT_MAX);

    free(distance);
    free_min_heap(heap);

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
