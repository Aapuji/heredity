#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "person.h"

#define MAX_LEN 64
#define NUM_ENTRIES 5

Person init_person(char *name, char *mother, char *father, char *genotype, char *phenotype) {
    #define ASSIGN_ATTR(NAME) .NAME=(char *) malloc((1 + strlen(NAME)) * sizeof(char))
    #define CHECK_ATTR(NAME) if (person.NAME == NULL) { \
        perror("malloc failed"); \
        exit(EXIT_FAILURE); \
    }
    #define SET_ATTR(NAME) strcpy(person.NAME, NAME)
    
    Person person = (Person) {
        ASSIGN_ATTR(name),
        ASSIGN_ATTR(mother),
        ASSIGN_ATTR(father),
        ASSIGN_ATTR(genotype),
        ASSIGN_ATTR(phenotype)
    };

    CHECK_ATTR(name)
    CHECK_ATTR(mother)
    CHECK_ATTR(father)
    CHECK_ATTR(genotype)
    CHECK_ATTR(phenotype)

    SET_ATTR(name);
    SET_ATTR(mother);
    SET_ATTR(father);
    SET_ATTR(genotype);
    SET_ATTR(phenotype);

    return person;

    #undef ASSIGN_ATTR
    #undef CHECK_ATTR
    #undef SET_ATTR
}

void free_person(Person *person) {
    free(person->name);
    free(person->mother);
    free(person->mother);
    free(person->genotype);
    free(person->phenotype);
}

void print_person(Person *person) {
    printf("Person { name: %s, mother: %s, father: %s, g: %s, ph: %s }", 
        person->name,
        person->mother,
        person->father,
        person->genotype,
        person->phenotype
    );
}

/// @brief Outputs whether or not the passed `Person` is valid.
/// @param person Pointer to `Person` to test.
/// @return `0` if invalid `1` if valid.
int verify_person(Person *person) {
    if (person->name == "") {
        return 0;
    }

    if (person->mother == "" && person->father != "" || person->mother != "" && person->father == "") {
        return 0;
    }

    return 1;   
}


People init_people(size_t capacity) {
    Person *data = (Person *) malloc(capacity * sizeof(Person));
    if (!data) {
        perror("malloc failed");
        exit(EXIT_FAILURE);
    }
    
    return (People) {
        .data = data,
        .length = 0,
        .capacity = capacity
    };
}

void free_people(People *people) {
    return free(people->data);
}

void add_person(People *people, Person person) {
    if (people->length == people->capacity) {
        Person *mem = (Person *) realloc(people->data, 2 * people->capacity * sizeof(Person));
        if (!mem) {
            perror("realloc failed");
            exit(EXIT_FAILURE);
        }

        people->data = mem;
    }

    people->data[people->length] = person;
    people->length++;
}

void print_people(People *people) {
    printf("[");
    for (size_t i = 0; i < people->length; i++) {
        print_person(&people->data[i]);
        printf("%s", i == people->length-1 ? "" : ", ");
    }
    printf("]");
}

/// @brief  Reads in the contents from a family csv file, with 5 headings (name, mother, father, genotype, phenotype). Each entry can be up to 64 characters long (including the null terminator).
/// @param file File for the family csv data.
/// @return A `People` struct with a collection of all the `person`s.
People read_family_csv(char *file) {
    FILE *family_csv = fopen(file, "r");
    if (!family_csv) {
        perror("fopen failed");
        exit(EXIT_FAILURE);
    }

    // Skip header line
    while (fgetc(family_csv) != '\n');

    People people = init_people(1);
    char ch;
    char *current_data[5];
    char buf[MAX_LEN];
    buf[0] = '\0';
    int i = 0;

    // Allocate memory for `current_data`
    for (int j = 0; j < NUM_ENTRIES; j++) {
        current_data[j] = (char *) malloc(MAX_LEN * sizeof(char));
        if (current_data[j] == NULL) {
            perror("malloc failed");
            exit(EXIT_FAILURE);
        }

        current_data[j][0] = '\0';
    }

    while ((ch = fgetc(family_csv)) != EOF) {
        if (ch == '\n') {
            // End of line; finalize current data
            if (i < NUM_ENTRIES) {
                strncpy(current_data[i], buf, MAX_LEN);
                current_data[i][MAX_LEN-1] = '\0';
            }

            Person person = init_person(current_data[0], current_data[1], current_data[2], current_data[3], current_data[4]);

            if (verify_person(&person)) {
                add_person(&people, person);
            } else {
                free_person(&person);
            }
            
            // Clear `current_data` for reuse
            for (int j = 0; j < NUM_ENTRIES; j++) {
                current_data[j][0] = '\0';
            }

            buf[0] = '\0';
            i = 0;
        } else if (ch == ',') {
            // End of entry, copy buffer to `current_data`
            if (i < NUM_ENTRIES) {
                strncpy(current_data[i], buf, MAX_LEN);
                current_data[i][MAX_LEN-1] = '\0';
            }

            buf[0] = '\0';
            i++;
        } else {
            // Append character to buffer
            size_t len = strlen(buf);
            if (len < MAX_LEN - 1) {
                buf[len] = ch;
                buf[len + 1] = '\0';
            }
        }
    }

    if (strlen(buf) > 0) {
        if (i < NUM_ENTRIES) {
            strncpy(current_data[i], buf, MAX_LEN);
            current_data[i][MAX_LEN-1] = '\0';
        }

        Person person = init_person(current_data[0], current_data[1], current_data[2], current_data[3], current_data[4]);
        if (verify_person(&person)) {
            add_person(&people, person);
        } else {
            free(person.name);
            free(person.mother);
            free(person.father);
            free(person.genotype);
            free(person.phenotype);
        }
    }

    // printf("\n<buffer>\n%s\n</buffer>\n", buf);

    // for (int j = 0; j < NUM_ENTRIES; j++) {
    //     printf("\n<arr[%i]>\n%s\n</arr[%i]>\n", j, current_data[j], j);
    // }

    // Free all the data
    for (int j = 0; j < NUM_ENTRIES; j++) {
        free(current_data[j]);
    }

    fclose(family_csv);
    return people;
}
