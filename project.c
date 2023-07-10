#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <limits.h>
#include <float.h>
#include <stdbool.h>

#define MAX_LOCATIONS 100
#define MAX_NAME_LENGTH 500
#define MAX 100

struct Location {
    char name[MAX_NAME_LENGTH];
    double latitude;
    double longitude;
    char specialization[MAX_NAME_LENGTH];
};

struct Graph {
    int num_vertices;
    struct Location locations[MAX_LOCATIONS];
    double adjacency_matrix[MAX_LOCATIONS][MAX_LOCATIONS];
};

typedef struct info {
    char name[MAX_NAME_LENGTH];
    float rating;
    char timing[MAX];
    int fees;
    char address[MAX_NAME_LENGTH * 2];
} info;

struct HashNode {
    struct info info;
    struct HashNode* next;
};

struct HashTable {
    struct HashNode* buckets[MAX_LOCATIONS];
};

unsigned int hash(const char* str) {
    unsigned int hash = 5381;
    int c;

    while ((c = *str++)) {
        hash = ((hash << 5) + hash) + c; /* hash * 33 + c */
    }

    return hash;
}

void insert_hash_node(struct HashTable* hash_table, struct info info) {
    unsigned int index = hash(info.name) % MAX_LOCATIONS;

    struct HashNode* new_node = malloc(sizeof(struct HashNode));
    new_node->info = info;
    new_node->next = NULL;

    if (hash_table->buckets[index] == NULL) {
        hash_table->buckets[index] = new_node;
    } else {
        struct HashNode* current = hash_table->buckets[index];
        while (current->next != NULL) {
            current = current->next;
        }
        current->next = new_node;
    }
}

struct HashNode* search_hash_node(struct HashTable* hash_table, const char* name) {
    unsigned int index = hash(name) % MAX_LOCATIONS;

    struct HashNode* current = hash_table->buckets[index];
    while (current != NULL) {
        if (strcmp(current->info.name, name) == 0) {
            return current;
        }
        current = current->next;
    }

    return NULL;
}

void print_hospital_info(struct HashNode* hospital) {
    printf("\n");
    printf("Hospital Name  : %s\n", hospital->info.name);
    printf("Rating         : %.1f\n", hospital->info.rating);
    printf("Working hours  : %s\n", hospital->info.timing);
    printf("Average fees   : %d\n", hospital->info.fees);
    printf("Address        : %s\n", hospital->info.address);
}

double calculate_distance(struct Location loc1, struct Location loc2) {
    double lat1 = loc1.latitude;
    double lon1 = loc1.longitude;
    double lat2 = loc2.latitude;
    double lon2 = loc2.longitude;

    double dlat = (lat2 - lat1) * M_PI / 180.0;
    double dlon = (lon2 - lon1) * M_PI / 180.0;

    double a = sin(dlat / 2) * sin(dlat / 2) +
               cos(lat1 * M_PI / 180.0) * cos(lat2 * M_PI / 180.0) *
                   sin(dlon / 2) * sin(dlon / 2);
    double c = 2 * atan2(sqrt(a), sqrt(1 - a));

    double distance = 6371.0 * c; // Earth radius in kilometers
    return distance;
}

int find_nearest_location(double* dist, int* visited, int num_vertices) {
    double min_dist = DBL_MAX;
    int min_index = -1;

    for (int v = 0; v < num_vertices; v++) {
        if (visited[v] == 0 && dist[v] < min_dist) {
            min_dist = dist[v];
            min_index = v;
        }
    }

    return min_index;
}

void dijkstra(struct Graph* graph, int source, double* dist, int* prev) {
    int num_vertices = graph->num_vertices;
    int visited[MAX_LOCATIONS] = {0};

    for (int v = 0; v < num_vertices; v++) {
        dist[v] = DBL_MAX;
        prev[v] = -1;
    }

    dist[source] = 0;

    for (int count = 0; count < num_vertices - 1; count++) {
        int u = find_nearest_location(dist, visited, num_vertices);
        visited[u] = 1;

        for (int v = 0; v < num_vertices; v++) {
            if (!visited[v] && graph->adjacency_matrix[u][v] != 0 &&
                dist[u] != DBL_MAX &&
                dist[u] + graph->adjacency_matrix[u][v] < dist[v]) {
                dist[v] = dist[u] + graph->adjacency_matrix[u][v];
                prev[v] = u;
            }
        }
    }
}


int find_location_index(struct Graph* graph, const char* location_name) {
    for (int i = 0; i < graph->num_vertices; i++) {
        if (strcmp(graph->locations[i].name, location_name) == 0) {
            return i;
        }
    }
    return -1;
}

int find_min_distance_vertex(double* dist, bool* visited, int num_vertices) {
    double min_distance = INFINITY;
    int min_vertex = -1;

    for (int v = 0; v < num_vertices; v++) {
        if (!visited[v] && dist[v] < min_distance) {
            min_distance = dist[v];
            min_vertex = v;
        }
    }

    return min_vertex;
}


void find_nearest_hospitals(struct Graph* graph, int source, double max_distance, const char* specialization, struct HashTable* hash_table) {
    int num_vertices = graph->num_vertices;
    double dist[MAX_LOCATIONS];
    int prev[MAX_LOCATIONS];

    dijkstra(graph, source, dist, prev);

    int hospital_count = 0;

    printf("_____________________________________________________________________________________________________\n\n");
    printf("\n\nHospitals within %.2lf km of %s with specialization '%s'\n", max_distance, graph->locations[source].name, specialization);
    printf("\n\n_____________________________________________________________________________________________________\n\n");
    printf("Finding nearest hospitals...\n");
    printf("_____________________________________________________________________________________________________\n\n");



    char *arr[MAX_NAME_LENGTH];

    for (int v = 0; v < num_vertices; v++) {
        if (v != source) {
            double distance = dist[v];
            if (distance <= max_distance && strcmp(graph->locations[v].specialization, specialization) == 0) {
                printf("%s  ==>  Distance: %.2lf km\n", graph->locations[v].name, distance);
                hospital_count++;
                arr[v] = graph->locations[v].name;
                // if (hospital_count == 5) {
                //     break;
                // }
            }
        }
    }


    if (hospital_count == 0) {
        printf("No hospitals found within %.2lf km with specialization '%s'.\n", max_distance, specialization);
    }
}

void details(struct Graph* graph, int source, double max_distance, const char* specialization, struct HashTable* hash_table) {
    int num_vertices = graph->num_vertices;
    double dist[MAX_LOCATIONS];
    int prev[MAX_LOCATIONS];

    dijkstra(graph, source, dist, prev);

    int hospital_count = 0;

    //printf("Hospitals within %.2lf km of %s with specialization '%s':\n", max_distance, graph->locations[source].name, specialization);

    char *arr[MAX_NAME_LENGTH];

    for (int v = 0; v < num_vertices; v++) {
        if (v != source) {
            double distance = dist[v];
            if (distance <= max_distance && strcmp(graph->locations[v].specialization, specialization) == 0) {
                //printf("%s (Distance: %.2lf km)\n", graph->locations[v].name, distance);
                hospital_count++;
                arr[v] = graph->locations[v].name;
                if (hospital_count == 5) {
                    break;
                }
            }
        }
    }

    for (int v = 0; v < graph->num_vertices; v++) {
        if (dist[v] <= max_distance && strcmp(graph->locations[v].specialization, specialization) == 0) {
            //printf("%s (Distance: %.2lf km)\n", graph->locations[v].name, dist[v]);
            hospital_count++;

            struct HashNode* hospital_info = search_hash_node(hash_table, graph->locations[v].name);
            if (hospital_info != NULL) {
                print_hospital_info(hospital_info);
                printf("\n\n======================================================================================================\n\n");
            }

            /*if (hospital_count == 5) {
                break;
            }*/
        }
    }


    if (hospital_count == 0) {
        printf("No hospitals found within %.2lf km with specialization '%s'.\n", max_distance, specialization);
    }
}

int main() {
    FILE* file = fopen("colleges.txt", "r");
    FILE* file1 = fopen("hospitals.txt", "r");
    FILE* file2 = fopen("info.txt", "r");
    if (file1 == NULL) {
        printf("Failed to open the hospitals file.\n");
        return 1;
    }
    if (file == NULL) {
        printf("Failed to open the colleges file.\n");
        return 1;
    }

    struct Graph graph;
    graph.num_vertices = 0;

    char line[100];
    while (fgets(line, sizeof(line), file)) {
        char name[MAX_NAME_LENGTH];
        double lat, lon;
        sscanf(line, "%[^,],%lf,%lf", name, &lat, &lon);

        struct Location location;
        strcpy(location.name, name);
        location.latitude = lat;
        location.longitude = lon;

        graph.locations[graph.num_vertices] = location;

        for (int i = 0; i < graph.num_vertices; i++) {
            double distance = calculate_distance(graph.locations[i], location);
            graph.adjacency_matrix[i][graph.num_vertices] = distance;
            graph.adjacency_matrix[graph.num_vertices][i] = distance;
        }

        graph.num_vertices++;

        if (graph.num_vertices >= MAX_LOCATIONS) {
            break; // Reached the maximum number of locations
        }
    }

    fclose(file);


    while (fgets(line, sizeof(line), file1)) {
        char name[MAX_NAME_LENGTH];
        double lat, lon;
        char specialization[MAX_NAME_LENGTH];
        sscanf(line, "%[^;];%lf;%lf;%s", name, &lat, &lon, specialization);

        struct Location location;
        strcpy(location.name, name);
        location.latitude = lat;
        location.longitude = lon;
        strcpy(location.specialization, specialization);

        graph.locations[graph.num_vertices] = location;

        for (int i = 0; i < graph.num_vertices; i++) {
            double distance = calculate_distance(graph.locations[i], location);
            graph.adjacency_matrix[i][graph.num_vertices] = distance;
            graph.adjacency_matrix[graph.num_vertices][i] = distance;
        }

        graph.num_vertices++;

        if (graph.num_vertices >= MAX_LOCATIONS) {
            break; // Reached the maximum number of locations
        }
    }

    fclose(file1);

    struct HashTable hash_table;
    for (int i = 0; i < MAX_LOCATIONS; i++) {
        hash_table.buckets[i] = NULL;
    }

    while (fgets(line, sizeof(line), file2)) {
        char name[MAX_NAME_LENGTH];
        float rating;
        char timing[MAX_NAME_LENGTH];
        char address[MAX_NAME_LENGTH * 2];
        int fees;
        sscanf(line, "%[^;];%f;%[^;];%d;%[^\n]", name, &rating, timing, &fees, address);

        struct info hospital_info;
        strcpy(hospital_info.name, name);
        hospital_info.rating = rating;
        strcpy(hospital_info.timing, timing);
        hospital_info.fees = fees;
        strcpy(hospital_info.address, address);

        insert_hash_node(&hash_table, hospital_info);
    }

    fclose(file2);


    char location_name[MAX_NAME_LENGTH];
    double max_distance;
    char specialization[MAX_NAME_LENGTH];

    printf("\n\n");
    printf("\n\n======================================================================================================\n\n");
    printf("\t\t\t\t   HOSPITAL FINDER FOR COLLEGES");
    printf("\n\n======================================================================================================\n\n");
    printf("\n\n");
    printf("Please choose college from the options given below: \n\n\n");

    FILE* file3 = fopen("outputFile.txt","r");
    while (fgets(line, sizeof(line), file3)) {
        char name[MAX_NAME_LENGTH];
        double lat, lon;
        printf("%s", line);
        
    }
    fclose(file3);
    
    printf("\n\n======================================================================================================\n\n");
    printf("\n\n");
    printf("Please choose specializations from the options given below: \n\n\n");
    printf("GENERAL\t\t\tMULTISPECIALITY\nORTHOPEDIC\t\tSKIN_AND_HAIR\nPEDIATRICS\t\tOPHTHALMOLOGY\nDENTAL\t\t\tAYURVED\nENDOSCOPY\t\tALLOPATHY");

    printf("\n\n\n======================================================================================================\n");


    printf("\n\nEnter the College Name: ");
    fgets(location_name, sizeof(location_name), stdin);
    location_name[strcspn(location_name, "\n")] = '\0';

    int source = find_location_index(&graph, location_name);
    if (source == -1) {
        printf("Invalid College Name.\n");
        return 1;
    }

    printf("\nEnter the maximum distance (in km): ");
    scanf("%lf", &max_distance);

    getchar(); // Consume the newline character

    printf("\nEnter the specialization: ");
    fgets(specialization, sizeof(specialization), stdin);
    specialization[strcspn(specialization, "\n")] = '\0';

    find_nearest_hospitals(&graph, source, max_distance, specialization, &hash_table);

    printf("\n");
    printf("_____________________________________________________________________________________________________\n\n");
    printf("Do you want more details of the hospitals? (YES/NO): ");
    char choice[3];
    scanf("%s",choice);
    printf("\n_____________________________________________________________________________________________________\n\n");
    if (strcmp(choice, "YES")==0){
        details(&graph, source, max_distance, specialization, &hash_table);
    }
    

    return 0;
}
