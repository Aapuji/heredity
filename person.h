#ifndef FAMILY_H
#define FAMILY_H

/// @brief Represents the data for a person (not the probabilities). Just stores the text from the csv file
typedef struct {
    char *name;
    char *mother;
    char *father;
    char *genotype;
    char *phenotype;
} Person;

typedef struct {
    Person *data;
    size_t length;
    size_t capacity;
} People;

Person init_person(char *name, char *mother, char *father, char *genotype, char *phenotype);
void print_person(Person *person);

People init_people(size_t capacity);
void free_people(People *people);
void add_person(People *people, Person person);
void print_people(People *people);
People read_family_csv(char *file);

#endif
