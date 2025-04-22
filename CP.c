#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <limits.h>
#include <string.h>
#include <windows.h>
#include <pthread.h>

#define MAX_NODES 100
#define PACKET_SIZE 10 // Simulated size per packet in characters

typedef struct Edge {
    int destination;
    int weight;
    struct Edge* next;
} Edge;

Edge* graph[MAX_NODES];
int nodes = 0;
int sent_packets[MAX_NODES] = {0};
int received_packets[MAX_NODES] = {0};

void add_route(int u, int v, int weight);
void save_network_to_file();
void load_network_from_file();
void export_network_dot(const char* filename);

void initialize_graph() {
    for (int i = 0; i < MAX_NODES; i++) {
        graph[i] = NULL;
    }
}

void add_computer() {
    if (nodes < MAX_NODES) {
        char filename[25];
        sprintf(filename, "computer_%d.txt", nodes);
        FILE* file = fopen(filename, "w");
        if (file) {
            fprintf(file, "Computer %d data file\n", nodes);
            fclose(file);
        }
        nodes++;
        printf("Computer %d added.\n", nodes - 1);
        save_network_to_file();
    } else {
        printf("Network is full!\n");
    }
}

void remove_computer(int comp) {
    if (comp < 0 || comp >= nodes) {
        printf("Invalid computer index!\n");
        return;
    }
    for (int i = 0; i < nodes; i++) {
        Edge** ptr = &graph[i];
        while (*ptr) {
            if ((*ptr)->destination == comp) {
                Edge* temp = *ptr;
                *ptr = (*ptr)->next;
                free(temp);
            } else {
                if ((*ptr)->destination > comp)
                    (*ptr)->destination--;
                ptr = &(*ptr)->next;
            }
        }
    }
    for (int i = comp; i < nodes - 1; i++) {
        graph[i] = graph[i + 1];
        sent_packets[i] = sent_packets[i + 1];
        received_packets[i] = received_packets[i + 1];
    }
    nodes--;
    char filename[25];
    sprintf(filename, "computer_%d.txt", comp);
    remove(filename);
    printf("Computer %d removed.\n", comp);
    save_network_to_file();
}

void add_route(int u, int v, int weight) {
    if (u < nodes && v < nodes && u != v) {
        Edge* edge1 = (Edge*)malloc(sizeof(Edge));
        edge1->destination = v;
        edge1->weight = weight;
        edge1->next = graph[u];
        graph[u] = edge1;

        Edge* edge2 = (Edge*)malloc(sizeof(Edge));
        edge2->destination = u;
        edge2->weight = weight;
        edge2->next = graph[v];
        graph[v] = edge2;

        printf("Route added: %d <--> %d (%dms)\n", u, v, weight);
        save_network_to_file();
    } else {
        printf("Invalid computers!\n");
    }
}

void dijkstra(int start, int end, int* previous) {
    int distance[MAX_NODES];
    bool visited[MAX_NODES];

    for (int i = 0; i < nodes; i++) {
        distance[i] = INT_MAX;
        previous[i] = -1;
        visited[i] = false;
    }
    distance[start] = 0;

    for (int count = 0; count < nodes - 1; count++) {
        int min = INT_MAX, minIndex = -1;
        for (int v = 0; v < nodes; v++) {
            if (!visited[v] && distance[v] <= min) {
                min = distance[v];
                minIndex = v;
            }
        }

        if (minIndex == -1) break;
        visited[minIndex] = true;

        for (Edge* temp = graph[minIndex]; temp; temp = temp->next) {
            int v = temp->destination;
            if (!visited[v] && distance[minIndex] + temp->weight < distance[v]) {
                distance[v] = distance[minIndex] + temp->weight;
                previous[v] = minIndex;
            }
        }
    }
}

void print_shortest_path(int start, int end, int* previous) {
    if (start == end) {
        printf("%d ", start);
        return;
    }
    if (previous[end] == -1) {
        printf("No path found.\n");
        return;
    }
    print_shortest_path(start, previous[end], previous);
    printf("-> %d ", end);
}

void* data_transfer(void* arg) {
    int* params = (int*)arg;
    int from = params[0];
    int to = params[1];
    int previous[MAX_NODES];
    char buffer[1024];

    dijkstra(from, to, previous);

    printf("Shortest path: ");
    print_shortest_path(from, to, previous);
    printf("\n");

    if (from != to && previous[to] == -1) {
        printf("No path exists between %d and %d. Transfer aborted.\n", from, to);
        return NULL;
    }

    char from_file[25], to_file[25];
    sprintf(from_file, "computer_%d.txt", from);
    sprintf(to_file, "computer_%d.txt", to);

    FILE* f1 = fopen(from_file, "r");
    if (!f1) {
        printf("Source file missing.\n");
        return NULL;
    }
    FILE* f2 = fopen(to_file, "a");
    if (!f2) {
        fclose(f1);
        printf("Destination file not found.\n");
        return NULL;
    }

    printf("Transferring data in packets...\n");
    while (fgets(buffer, PACKET_SIZE, f1)) {
        fprintf(f2, "%s", buffer);
        sent_packets[from]++;
        received_packets[to]++;
        printf("Packet sent: %s\n", buffer);
        Sleep(100);
    }
    fclose(f1);
    fclose(f2);
    printf("Data transfer complete.\n");
    return NULL;
}

void save_network_to_file() {
    FILE* f = fopen("network.txt", "w");
    fprintf(f, "%d\n", nodes);
    for (int i = 0; i < nodes; i++) {
        for (Edge* e = graph[i]; e; e = e->next) {
            if (i < e->destination)
                fprintf(f, "%d %d %d\n", i, e->destination, e->weight);
        }
    }
    fclose(f);
}

void load_network_from_file() {
    FILE* f = fopen("network.txt", "r");
    if (!f) return;

    fscanf(f, "%d", &nodes);
    int u, v, w;
    while (fscanf(f, "%d %d %d", &u, &v, &w) == 3) {
        add_route(u, v, w);
    }
    fclose(f);
}

void export_network_dot(const char* filename) {
    FILE* f = fopen(filename, "w");
    if (!f) return;

    fprintf(f, "graph Network {\n");
    for (int i = 0; i < nodes; i++) {
        for (Edge* e = graph[i]; e; e = e->next) {
            if (i < e->destination)
                fprintf(f, "    %d -- %d [label=\"%dms\"];\n", i, e->destination, e->weight);
        }
    }
    fprintf(f, "}\n");
    fclose(f);
    printf("Network exported to %s\n", filename);
}

int main() {
    initialize_graph();
    load_network_from_file();

    while (1) {
        printf("\n--- MENU ---\n");
        printf("1. Add Computer\n");
        printf("2. Remove Computer\n");
        printf("3. Add Route\n");
        printf("4. Transfer File\n");
        printf("5. Export Network\n");
        printf("6. Exit\n");
        printf("Choice: ");
        int choice;
        scanf("%d", &choice);

        if (choice == 1) {
            add_computer();
        } else if (choice == 2) {
            int id;
            printf("Enter computer ID to remove: ");
            scanf("%d", &id);
            remove_computer(id);
        } else if (choice == 3) {
            int u, v, w;
            printf("Enter u, v and weight: ");
            scanf("%d %d %d", &u, &v, &w);
            add_route(u, v, w);
        } else if (choice == 4) {
            int from, to;
            printf("Enter source and destination: ");
            scanf("%d %d", &from, &to);
            int* params = (int*)malloc(3 * sizeof(int));
            params[0] = from;
            params[1] = to;
            pthread_t t;
            pthread_create(&t, NULL, data_transfer, params);
            pthread_join(t, NULL);
            free(params);
        } else if (choice == 5) {
            export_network_dot("network.dot");
        } else if (choice == 6) {
            printf("Goodbye!\n");
            break;
        } else {
            printf("Invalid choice!\n");
        }
    }
    return 0;
}
