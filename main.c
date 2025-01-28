#include <stdio.h>

#include "person.h"

int main(void) {
    char file[18] = "./data/family.csv";

    People people = read_family_csv(file);
    print_people(&people);


    return 0;
}
