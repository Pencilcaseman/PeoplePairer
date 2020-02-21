#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

FILE *peopleAndNumbers;
FILE *numbersAndPreferences;
int peopleCount;

struct Person {
    char *name;
    int number;
    int *preferences;

    int connectionCount;
    int *connections;

    int label;
};

struct Person findPersonFromNumber(struct Person *people, int number) {
    for (int i = 0; i < peopleCount; i++) {
        // printf("Person: %s\n", people[i].name);
        // printf("Number: %i\n", people[i].number);
        if (people[i].number == number) {
            return people[i];
        }
    }

    // Shouldn't get to here
    printf("Unable to find person with number: %i\n", number);
    exit(1);
}

int findPersonInList(struct Person *people, struct Person person) {
    for (int i = 0; i < peopleCount; i++) {
        bool ret = true;

        if (people[i].name != person.name) {
            ret = false;
        }

        if (people[i].number != person.number) {
            ret = false;
        }

        if (people[i].connectionCount != person.connectionCount) {
            ret = false;
        }

        for (int j = 0; j < person.connectionCount; j++) {
            if (people[i].connections[j] != person.connections[j]) {
                ret = false;
            }
        }

        for (int j = 0; j < 5; j++) {
            if (people[i].preferences[j] != person.preferences[j]) {
                ret = false;
            }
        }

        if (ret) {
            return i;
        }
    }

    return -1;
}

int listContains(const int *list, int len, int val) {
    for (int i = 0; i < len; i++) {
        if (list[i] == val) {
            return i;
        }
    }

    return -1;
}

struct Person *ReadFileInput() {
    struct Person *people = (struct Person *) malloc(sizeof(struct Person) * peopleCount);

    // Read the people and number data
    for (int i = 0; i < peopleCount; i++) {
        char *data = (char *) malloc(sizeof(char) * 500);
        fgets(data, 500, peopleAndNumbers);

        char *numb = (char *) malloc(sizeof(char) * 500);
        int numbIndex = 0;
        char *name = (char *) malloc(sizeof(char) * 500);
        bool getName = true;
        for (int j = 0; j < strlen(data); j++) {
            if (data[j] == ',') {
                getName = false;
            } else {
                if (getName) {
                    name[j] = data[j];
                } else {
                    if (data[j] != ' ') {
                        numb[numbIndex] = data[j];
                        numbIndex++;
                    }
                }
            }
        }

        int *connections = (int *) malloc(sizeof(int) * 5);
        int *preferences = (int *) malloc(sizeof(int) * 5);
        struct Person person = {.name=name, .number=atoi(
                numb), .connectionCount=0, .connections=connections, .preferences=preferences, .label = -1};
        people[i] = person;
    }

    // Read the numbers and the options
    for (int i = 0; i < peopleCount; i++) {
        char *data = (char *) malloc(sizeof(char) * 500);
        fgets(data, 500, numbersAndPreferences);

        int number = 0;
        int *preferences = (int *) malloc(sizeof(int) * 5);
        char *current = (char *) malloc(sizeof(char) * 500);
        int currentIndex = 0;
        int option = -1;

        int dataLen = (int) strlen(data);
        for (int j = 0; j < dataLen; j++) {
            // j == dataLen - 1
            if (data[j] == ',' || data[j] == '\n') {
                int value = atoi(current);
                currentIndex = 0;

                for (int k = 0; k < 500; k++) {
                    current[k] = 0;
                }

                if (option == -1) {
                    number = value;
                } else {
                    preferences[option] = value;
                }

                option++;
            } else {
                if (data[j] != ' ') {
                    current[currentIndex] = data[j];
                    currentIndex++;
                }
            }

            if (i + 1 == peopleCount && j + 1 == dataLen && preferences[4] == 0) {
                preferences[4] = atoi(current);
            }
        }

        for (int j = 0; j < peopleCount; j++) {
            if (people[j].number == number) {
                // Found preferences for this person
                for (int k = 0; k < 5; k++) {
                    people[j].preferences[k] = preferences[k];
                }

                // Exit the loop
                j = peopleCount;
            }
        }
    }

    // Initialize connections
    for (int i = 0; i < peopleCount; i++) {
        // See if preferences put them down too
        for (int j = 0; j < 5; j++) {
            // See if options put down person

            //
            // Preferences                          [b, c, d, e, f, g]
            // For each preference                   b
            // See if preferences match name         b > [a, c, d, e, f, g]
            //
            if (listContains(findPersonFromNumber(people, people[i].preferences[j]).preferences, 5, people[i].number) !=
                -1) {

                people[i].connections[people[i].connectionCount] = people[i].preferences[j];
                people[i].connectionCount++;
            } else {
                // Option did not put them down...

                //*

                // Creates connections regardless of whether option put them down too
                // Results in more densely connected node-map
                // Results in fewer and larger communities
                // Nobody will end up in a community alone
                //######################################################################################
                //######################################################################################
                //######################################################################################

                people[i].connections[people[i].connectionCount] = people[i].preferences[j];
                int optionIndex = findPersonInList(people, findPersonFromNumber(people, people[i].connections[people[i].connectionCount]));
                people[optionIndex].connections[people[optionIndex].connectionCount] = people[i].number;

                people[i].connectionCount++;
                people[optionIndex].connectionCount++;

                //######################################################################################
                //######################################################################################
                //######################################################################################

                //*/
            }
        }
    }

    fclose(peopleAndNumbers);

    return people;
}

int degreeOfNodes(struct Person *people, int label) {
    int res = 0;
    for (int i = 0; i < peopleCount; i++) {
        if (people[i].label == label) {

            res += people[i].connectionCount;
        }
    }
    return res;
}

int nodeCount(struct Person *people, int label) {
    int res = 0;
    for (int i = 0; i < peopleCount; i++) {
        if (people[i].label == label) {
            res++;
        }
    }
    return res;
}

int connectionCount(struct Person *people, int label) {
    int res = 0;
    int **evaluated = (int **) malloc(sizeof(int *) * peopleCount * 5);
    int evalCount = 0;
    for (int i = 0; i < peopleCount * 5; i++) {
        evaluated[i] = (int *) malloc(sizeof(int) * 2);
        evaluated[i][0] = -1;
        evaluated[i][1] = -1;
    }

    for (int i = 0; i < peopleCount; i++) {
        if (people[i].label == label || label == -12345) {
            for (int j = 0; j < people[i].connectionCount; j++) {
                if (findPersonFromNumber(people, people[i].connections[j]).label == label || label == -12345) {
                    bool previouslyEvaluated = false;
                    for (int k = 0; k < evalCount; k++) {
                        if ((evaluated[k][0] == people[i].number && evaluated[k][1] == people[i].connections[j]) ||
                            (evaluated[k][0] == people[i].connections[j] && evaluated[k][1] == people[i].number)) {
                            previouslyEvaluated = true;
                        }
                    }

                    if (!previouslyEvaluated) {
                        evaluated[evalCount][0] = people[i].number;
                        evaluated[evalCount][1] = people[i].connections[j];
                        evalCount++;

                        res++;
                    }
                }
            }
        }
    }

    return res;
}

double _pow(double val, int pow) {
    double res = val;
    for (int i = 0; i < pow; i++) {
        res *= val;
    }

    return res;
}

double calculateModularity(struct Person *people) {
    // Find existing labels
    int labelCount = 0;
    int *labels = (int *) malloc(sizeof(int) * peopleCount);
    for (int i = 0; i < peopleCount; i++) {
        bool evaluated = false;
        int personLabel = people[i].label;
        for (int j = 0; j < labelCount; j++) {
            if (personLabel == labels[j]) {
                evaluated = true;
            }
        }

        if (!evaluated) {
            labels[labelCount] = personLabel;
            labelCount++;
        }
    }

    double res = 0;
    for (int i = 0; i < labelCount; i++) {
        double partitionRelationships = (double) connectionCount(people, labels[i]);
        double totalRelationships = (double) connectionCount(people, -12345);
        double degreeNodes = (double) degreeOfNodes(people, labels[i]);

        double modularity =
                (partitionRelationships / totalRelationships) - _pow(degreeNodes / (2 * totalRelationships), 2);
        res += modularity;
    }

    return res;
}

// Buggy and VERY slow
struct Person *bruteForceLouvain(struct Person *people, int maxCommunities) {
    int *permutation = (int *) malloc(sizeof(int) * peopleCount);
    for (int i = 0; i < peopleCount; i++) {
        permutation[i] = 0;
    }

    double maxModularity = -100000;
    int *optimum = (int *) malloc(sizeof(int) * peopleCount);
    for (int i = 0; i < peopleCount; i++) {
        optimum[i] = 0;
    }

    bool cont = true;
    int iterations = 0;
    while (cont) {
        permutation[peopleCount - 1] += 1;

        for (int i = peopleCount - 1; i > 0; i--) {
            if (permutation[i] >= maxCommunities) {
                permutation[i] = 0;
                permutation[i - 1] += 1;
            }
        }

        if (permutation[0] >= maxCommunities) {
            cont = false;
        } else {
            for (int i = 0; i < peopleCount; i++) {
                people[i].label = permutation[i];
            }

            double modularity = calculateModularity(people);
            if (modularity > maxModularity) {
                maxModularity = modularity;
                for (int i = 0; i < peopleCount; i++) {
                    optimum[i] = permutation[i];
                }
            }

            if (iterations % 10000 == 0) {
                for (int i = 0; i < peopleCount; i++) {
                    printf("%i, ", permutation[i]);
                }
                printf("\n");
                printf("Modularity: %f\n", modularity);
            }
        }

        iterations++;
    }

    for (int i = 0; i < peopleCount; i++) {
        printf("%i ", optimum[i]);
        people[i].label = optimum[i];
    }
    printf("\n");
    return people;
}

int **louvain(struct Person *people) {
    bool continueToFindCommunities = true;
    int label = 0;

    while (continueToFindCommunities) {
        // Find a single community
        bool cont = true;
        int startNode = -1;

        // Find the first unlabeled node
        for (int i = 0; i < peopleCount; i++) {
            if (people[i].label == -1) {
                startNode = i;
                break;
            }
        }

        if (startNode == -1) {
            continueToFindCommunities = false;
        } else {
            people[startNode].label = label;

            while (cont) {
                printf("Evaluating %s\n", people[startNode].name);

                // Current modularity score
                double currentModularity = calculateModularity(people);
                double maxModularityDelta = -1000;
                int maxIndex = 0;

                for (int i = 0; i < people[startNode].connectionCount; i++) {
                    int optionIndex = findPersonInList(people,
                                                       findPersonFromNumber(people, people[startNode].connections[i]));

                    printf("Checking option %s\n", people[optionIndex].name);

                    int prevLabel = people[optionIndex].label;
                    people[optionIndex].label = label;

                    double newModularity = calculateModularity(people);

                    printf("Got a modularity score of: %f\n", newModularity);

                    if ((newModularity - currentModularity) > maxModularityDelta) {
                        printf("Found a new high score for modularity: %s\n", people[optionIndex].name);

                        maxModularityDelta = (newModularity - currentModularity);
                        maxIndex = optionIndex;
                    }

                    people[optionIndex].label = prevLabel;
                }

                if (people[maxIndex].label == label) {
                    printf("Found an entire community, moving on to the next one now\n");
                    cont = false;
                } else {
                    people[maxIndex].label = label;
                    startNode = maxIndex;
                }
            }

            label++;
        }
    }

    int **res = (int **) malloc(sizeof(int *) * label);
    int resIndex = 0;

    printf("\nResult of louvain algorithm...\n");
    for (int i = 0; i < label; i++) {
        printf("Community %i:\n", i);
        int *community = (int *) malloc(sizeof(int) * peopleCount);
        int communityIndex = 0;
        for (int j = 0; j < peopleCount; j++) {
            if (people[j].label == i) {
                printf("\t> %s (%i)\n", people[j].name, people[j].number);
                community[communityIndex] = people[j].number;
                communityIndex++;
            }
        }
        res[resIndex] = community;
        resIndex++;
    }

    return res;
}

int **generateClasses(int **communities) {
    printf("Doesnt do anything yet...\n");
}

int main(int argumentCount, char **arguments) {
    if (argumentCount != 4) {
        printf("Invalid number of arguments passed...\n");
        printf("Must pass:\n");
        printf("\t> Number of people\n");
        printf("\t> Directory to file containing name of person and number\n");
        printf("\t> Directory to file containing number of person and options\n");

        return 1;
    } else {
        peopleCount = atoi(arguments[1]);

        // "./namesAndNumbers.csv"
        peopleAndNumbers = fopen(arguments[2], "r");

        // "./numbersAndPreferences.csv"
        numbersAndPreferences = fopen(arguments[3], "r");
    }

    struct Person *people = ReadFileInput();

    printf("Connection Testing: %i\n", people[0].connections[0]);
    printf("Total degree of nodes with label -1: %i\n", degreeOfNodes(people, -1));
    printf("Total number of connections: %i\n", connectionCount(people, -1));

    printf("Modularity of current partition arrangement: %f\n", calculateModularity(people));


    /*
    printf("Brute forcing...\n");

    int communities = 2;
    people = bruteForceLouvain(people, communities);
    for (int i = 0; i < communities; i++) {
        printf("Community: %i\n", i);
        for (int j = 0; j < peopleCount; j++) {
            if (people[j].label == i) {
                printf("> %s\n", people[j].name);
            }
        }
    }
    */

    printf("Locating testing: %i\n", findPersonInList(people, people[0]));

    int **communities = louvain(people);

    return 0;
}
