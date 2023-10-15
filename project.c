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
    #define MAX_COMMENTS 100


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
        // New fields for reviews and ratings
        int num_reviews;
        float total_rating;
    } info;

    struct HashNode {
        struct info info;
        struct HashNode* next;
    };

    struct HashTable {
        struct HashNode* buckets[MAX_LOCATIONS];
    };

    struct Comment {
        char hospital_name[MAX_NAME_LENGTH];
        char user_name[MAX_NAME_LENGTH];
        char text[MAX_NAME_LENGTH * 2];
    };
    struct Comment comments[MAX_COMMENTS];
    int num_comments = 0;

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
        new_node->info.num_reviews = 0;
        new_node->info.total_rating = 0.0;
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

        for (int v = 0; v < num_vertices; v++) {
            if (v != source) {
                double distance = dist[v];
                if (distance <= max_distance && strcmp(graph->locations[v].specialization, specialization) == 0) {
                    printf("%s  ==>  Distance: %.2lf km\n", graph->locations[v].name, distance);
                    hospital_count++;
                }
            }
        }

        if (hospital_count == 0) {
            printf("No hospitals found within %.2lf km with specialization '%s'.\n", max_distance, specialization);
        }
    }

    void get_hospital_names(struct Graph* graph, struct HashTable* hash_table) {
        printf("\n\nHospitals Available:\n\n");
        for (int i = 0; i < graph->num_vertices; i++) {
            printf("%d. %s\n", i + 1, graph->locations[i].name);
        }

        // Consume the newline character left in the buffer
        int c;
        while ((c = getchar()) != '\n' && c != EOF);
    }


    void details(struct Graph* graph, int source, double max_distance, const char* specialization, struct HashTable* hash_table) {
        int num_vertices = graph->num_vertices;
        double dist[MAX_LOCATIONS];
        int prev[MAX_LOCATIONS];

        dijkstra(graph, source, dist, prev);

        int hospital_count = 0;

        char* arr[MAX_NAME_LENGTH];

        for (int v = 0; v < num_vertices; v++) {
            if (v != source) {
                double distance = dist[v];
                if (distance <= max_distance && strcmp(graph->locations[v].specialization, specialization) == 0) {
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
                hospital_count++;

                struct HashNode* hospital_info = search_hash_node(hash_table, graph->locations[v].name);
                if (hospital_info != NULL) {
                    print_hospital_info(hospital_info);
                    // printf("Rating: %.1f\n", hospital_info->info.rating);
                    // printf("Total Reviews: %d\n", hospital_info->info.num_reviews);
                    printf("\n======================================================================================================\n\n");
                }
            }
        }

        if (hospital_count == 0) {
            printf("No hospitals found within %.2lf km with specialization '%s'.\n", max_distance, specialization);
        }
    }


    void save_reviews_to_file(struct HashTable* hash_table, const char* filename) {
        FILE* file = fopen(filename, "w");
        if (file == NULL) {
            printf("Error opening file for writing.\n");
            return;
        }

        for (int i = 0; i < MAX_LOCATIONS; i++) {
            struct HashNode* current = hash_table->buckets[i];
            while (current != NULL) {
                if (current->info.num_reviews > 0) {
                    fprintf(file, "%s;%d;%f;%f\n", current->info.name, current->info.num_reviews, current->info.total_rating, current->info.rating);
                }
                current = current->next;
            }
        }

        fclose(file);
    }

    void load_reviews_from_file(struct HashTable* hash_table, const char* filename) {
        FILE* file = fopen(filename, "r");
        if (file == NULL) {
            printf("Error opening file for reading.\n");
            return;
        }

        char line[MAX_NAME_LENGTH * 2];
        while (fgets(line, sizeof(line), file)) {
            char name[MAX_NAME_LENGTH];
            int num_reviews;
            float total_rating, rating;
            sscanf(line, "%[^;];%d;%f;%f", name, &num_reviews, &total_rating, &rating);

            struct HashNode* hospital_info = search_hash_node(hash_table, name);
            if (hospital_info != NULL) {
                hospital_info->info.num_reviews = num_reviews;
                hospital_info->info.total_rating = total_rating;
                hospital_info->info.rating = rating;
            }
        }

        fclose(file);
    }

    void review_hospitals(struct Graph* graph, struct HashTable* hash_table) {
        char *L[] = {"GENERAL", "MULTISPECIALITY", "ORTHOPEDIC", "SKIN_AND_HAIR", "PEDIATRICS", "OPHTHALMOLOGY", "DENTAL", "AYURVED", "ENDOSCOPY", "ALLOPATHY","OPTHALMOLOGY"};
        printf("\n\nHospitals Available for Review:\n\n");
        int cnt=0;
        for (int i = 0; i < graph->num_vertices; i++) {
            int specializationExists = 0;
            for (int j = 0; j < sizeof(L) / sizeof(L[0]); j++) {
                if (strcmp(graph->locations[i].specialization, L[j]) == 0) {
                    specializationExists = 1;
                    break;
                }
            }
            if (specializationExists) {
                printf("%d. %s\n", cnt + 1, graph->locations[i].name);
                cnt+=1;
            }
        }

        int choice;
        printf("\nEnter the number of the hospital you want to review: ");
        scanf("%d", &choice);

        if (choice >= 1 && choice <= graph->num_vertices) {
            char hospital_name[MAX_NAME_LENGTH];
            strcpy(hospital_name, graph->locations[choice - 1 + 20].name);

            struct HashNode* hospital_info_node = search_hash_node(hash_table, hospital_name);

            if (hospital_info_node != NULL) {
                printf("\nHospital Name  : %s\n", hospital_info_node->info.name);
                printf("Rating         : %.1f\n", hospital_info_node->info.rating);
                printf("Total Reviews  : %d\n", hospital_info_node->info.num_reviews);
                printf("Working hours  : %s\n", hospital_info_node->info.timing);
                printf("Average fees   : %d\n", hospital_info_node->info.fees);
                printf("Address        : %s\n", hospital_info_node->info.address);

                printf("\nLeave a Review (1-5): ");
                int user_rating;
                scanf("%d", &user_rating);

                hospital_info_node->info.num_reviews++;
                hospital_info_node->info.total_rating += user_rating;
                hospital_info_node->info.rating = hospital_info_node->info.total_rating / hospital_info_node->info.num_reviews;

                printf("\nThank you for your review!\n");
                save_reviews_to_file(hash_table, "reviews.txt");
            } else {
                printf("Hospital not found.\n");
            }
        } else {
            printf("Invalid choice.\n");
        }
    }

    void load_comments_from_file(struct Comment* comments, int* num_comments, const char* filename) {
        FILE* file = fopen(filename, "r");
        if (file == NULL) {
            printf("Error opening file for reading comments.\n");
            return;
        }

        char line[MAX_NAME_LENGTH * 4];
        while (fgets(line, sizeof(line), file)) {
            char hospital_name[MAX_NAME_LENGTH];
            char user_name[MAX_NAME_LENGTH];
            char text[MAX_NAME_LENGTH * 2];
            if (sscanf(line, "%[^;];%[^;];%[^\n]", hospital_name, user_name, text) == 3) {
                struct Comment new_comment;
                strcpy(new_comment.hospital_name, hospital_name);
                strcpy(new_comment.user_name, user_name);
                strcpy(new_comment.text, text);

                comments[(*num_comments)++] = new_comment;

                if (*num_comments >= MAX_COMMENTS) {
                    break;  // Reached the maximum number of comments
                }
            }
        }

        fclose(file);
    }

void add_comment(struct Comment* comments, int* num_comments, const char* hospital_name, const char* user_name, const char* text) {
    if (*num_comments >= MAX_COMMENTS) {
        printf("Comments limit reached.\n");
        return;
    }

    struct Comment new_comment;
    strcpy(new_comment.hospital_name, hospital_name);
    strcpy(new_comment.user_name, user_name);
    strcpy(new_comment.text, text);

    comments[(*num_comments)++] = new_comment;

    // Open the file in append mode with error checking
    FILE* file = fopen("comments.txt", "a");
    if (file == NULL) {
        printf("Error opening file for appending comments.\n");
        return;
    }

    // Write the new comment to the file with error checking
    if (fprintf(file, "%s;%s;%s\n", hospital_name, user_name, text) < 0) {
        printf("Error writing to comments file.\n");
    }

    fclose(file);

    printf("Comment added successfully!\n");
}

void display_comments(struct Comment* comments, int num_comments, const char* hospital_name) {
    printf("Comments for Hospital: %s\n", hospital_name);

    int found_comments = 0;

    for (int i = 0; i < num_comments; i++) {
        if (strcmp(comments[i].hospital_name, hospital_name) == 0) {
            printf("User: %s\n", comments[i].user_name);
            printf("Comment: %s\n\n", comments[i].text);
            found_comments = 1;
        }
    }

    if (!found_comments) {
        printf("No comments available for this hospital.\n");
    }

    // Load comments from the file and store them in the file_comments array
    FILE* file = fopen("comments.txt", "r");
    if (file == NULL) {
        printf("Error opening file for reading comments.\n");
        return;
    }

    char line[MAX_NAME_LENGTH * 4];
    while (fgets(line, sizeof(line), file)) {
        char stored_hospital_name[MAX_NAME_LENGTH];
        char user_name[MAX_NAME_LENGTH];
        char text[MAX_NAME_LENGTH * 2];
        sscanf(line, "%[^;];%[^;];%[^\n]", stored_hospital_name, user_name, text);

        if (strcmp(stored_hospital_name, hospital_name) == 0) {
            int found_in_comments = 0;
            for (int i = 0; i < num_comments; i++) {
                if (strcmp(comments[i].user_name, user_name) == 0 && strcmp(comments[i].text, text) == 0) {
                    found_in_comments = 1;
                    break;
                }
            }

            if (!found_in_comments) {
                // This comment from the file is not in the comments array, so add it to the file_comments array
                struct Comment new_comment;
                strcpy(new_comment.hospital_name, hospital_name);
                strcpy(new_comment.user_name, user_name);
                strcpy(new_comment.text, text);

                // Display the comment from the file (without "(from file)")
                printf("User: %s\n", user_name);
                printf("Comment: %s\n\n", text);
                found_comments = 1;
            }
        }
    }

    fclose(file);

    if (!found_comments) {
        // printf("No additional comments available from the file.\n");
    }
}


    void save_comments_to_file(struct Comment* comments, int num_comments, const char* filename) {
        FILE* file = fopen(filename, "w");
        if (file == NULL) {
            printf("Error opening file for writing comments.\n");
            return;
        }

        for (int i = 0; i < num_comments; i++) {
            fprintf(file, "%s;%s;%s\n", comments[i].hospital_name, comments[i].user_name, comments[i].text);
        }

        fclose(file);
    }

void display_hospitals(struct Graph* graph) {
    printf("\nHospitals Available:\n");
    int hospital_count = 0;
    for (int i = 0; i < graph->num_vertices; i++) {
        int specializationExists = 0;
        char* L[] = {"GENERAL", "MULTISPECIALITY", "ORTHOPEDIC", "SKIN_AND_HAIR", "PEDIATRICS", "OPHTHALMOLOGY", "DENTAL", "AYURVED", "ENDOSCOPY", "ALLOPATHY","OPTHALMOLOGY"};
        for (int j = 0; j < sizeof(L) / sizeof(L[0]); j++) {
            if (strcmp(graph->locations[i].specialization, L[j]) == 0) {
                specializationExists = 1;
                break;
            }
        }
        if (specializationExists) {
            printf("%d. %s\n", hospital_count + 1, graph->locations[i].name);
            hospital_count += 1;
        }
    }
}

int get_hospital_index(struct Graph* graph, const char* hospital_name) {
    // Define the list of valid specializations
    const char* valid_specializations[] = {
        "GENERAL",
        "MULTISPECIALITY",
        "ORTHOPEDIC",
        "SKIN_AND_HAIR",
        "PEDIATRICS",
        "OPHTHALMOLOGY",
        "DENTAL",
        "AYURVED",
        "ENDOSCOPY",
        "ALLOPATHY",
        "OPTHALMOLOGY"
    };

    int num_specializations = sizeof(valid_specializations) / sizeof(valid_specializations[0]);

    for (int i = 0; i < graph->num_vertices; i++) {
        if (strcmp(graph->locations[i].name, hospital_name) == 0) {
            // Check if the specialization is valid
            const char* specialization = graph->locations[i].specialization;
            int is_valid_specialization = 0;

            for (int j = 0; j < num_specializations; j++) {
                if (strcmp(specialization, valid_specializations[j]) == 0) {
                    is_valid_specialization = 1;
                    break;
                }
            }

            if (!is_valid_specialization) {
                // printf("Invalid Specialization for the Hospital: %s\n", hospital_name);
                // return -2;
                // printf("Invalid Hospital");
                break;
            }

            return i;
        }
    }
    return -1;
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

        // Load reviews before performing any actions
        load_reviews_from_file(&hash_table, "reviews.txt");

        char location_name[MAX_NAME_LENGTH];
        double max_distance;
        char specialization[MAX_NAME_LENGTH];

        printf("\n\n");
        printf("\n\n======================================================================================================\n\n");
        printf("\t\t\t\t   HOSPITAL FINDER FOR COLLEGES");
        printf("\n\n======================================================================================================\n\n");
        printf("\n\n");

        int user_choice;

        printf("Choose an option:\n");
        printf("1. Get List and information of Hospitals\n");
        printf("2. Give a Review\n");
        printf("3. Add/Display Comment\n");
        printf("\nEnter your choice (1 or 2 or 3): ");
        scanf("%d", &user_choice);

        if (user_choice == 1) {
            // Get names of hospitals
            // get_hospital_names(&graph, &hash_table);
            int c;
            while ((c = getchar()) != '\n' && c != EOF);
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

            // Save reviews before exiting the program
            save_reviews_to_file(&hash_table, "reviews.txt");

        } else if (user_choice == 2) {
            // Give a review
            review_hospitals(&graph, &hash_table);
        }else if (user_choice == 3) {
            // Add or display comment
            printf("1. Add Comment\n");
            printf("2. Display Comments\n");
            printf("Enter your choice (1 or 2): ");
            int comment_choice;
            if (scanf("%d", &comment_choice) != 1) {
                printf("Invalid input. Please enter 1 or 2.\n");
                while (getchar() != '\n');
                    // continue;
            }
            getchar(); // Consume the newline character
            load_comments_from_file(comments, &num_comments, "comments.txt");
if (comment_choice == 1) {
    // Add Comment
    char hospital_name[MAX_NAME_LENGTH];
    char user_name[MAX_NAME_LENGTH];
    char comment_text[MAX_NAME_LENGTH * 2];
    
    display_hospitals(&graph);
    printf("Enter Hospital Name for Comment: ");
    
    // Read the hospital name with fgets and remove the newline character
    fgets(hospital_name, sizeof(hospital_name), stdin);
    hospital_name[strcspn(hospital_name, "\n")] = '\0';

    int hospital_index = get_hospital_index(&graph, hospital_name);
    if (hospital_index == -1) {
        printf("Invalid Hospital Name. The entered hospital is not in the list. Please try again.\n");
        // Handle the error or return to the menu
    } else {
        printf("Enter User Name: ");
        scanf("%s", user_name);
        printf("Enter Comment: ");
        getchar(); // Consume the newline character
        fgets(comment_text, sizeof(comment_text), stdin);
        comment_text[strcspn(comment_text, "\n")] = '\0';

        add_comment(comments, &num_comments, hospital_name, user_name, comment_text);
    }
} else if (comment_choice == 2) {
    // Display Comments
    char hospital_name[MAX_NAME_LENGTH];
    
    display_hospitals(&graph);
    printf("Enter Hospital Name to Display Comments: ");
    
    // Read the hospital name with fgets and remove the newline character
    fgets(hospital_name, sizeof(hospital_name), stdin);
    hospital_name[strcspn(hospital_name, "\n")] = '\0';

    int hospital_index = get_hospital_index(&graph, hospital_name);
    if (hospital_index == -1) {
        printf("Invalid Hospital Name. The entered hospital is not in the list. Please try again.\n");
        // Handle the error or return to the menu
    } else {
        // Display comments if the hospital name is valid
        display_comments(comments, num_comments, hospital_name);
    }
} else {
    printf("Invalid choice for comments.\n");
}





// Save reviews and comments before exiting the program
save_reviews_to_file(&hash_table, "reviews.txt");
save_comments_to_file(comments, num_comments, "comments.txt");

return 0;

    }

    }