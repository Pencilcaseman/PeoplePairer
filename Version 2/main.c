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

        if (people[i].number != person.number && ret) {
            ret = false;
        }

        if (people[i].connectionCount != person.connectionCount && ret) {
            ret = false;
        }

        for (int j = 0; j < person.connectionCount; j++ && ret) {
            if (people[i].connections[j] != person.connections[j]) {
                ret = false;
            }
        }

        for (int j = 0; j < 5; j++) {
            if (people[i].preferences[j] != person.preferences[j] && ret) {
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
        char *data = (char *) malloc(sizeof(char) * 100);
        fgets(data, 100, peopleAndNumbers);

        char *numb = (char *) malloc(sizeof(char) * 100);
        int numbIndex = 0;
        char *name = (char *) malloc(sizeof(char) * 100);
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
        char *data = (char *) malloc(sizeof(char) * 100);
        fgets(data, 100, numbersAndPreferences);

        int number = 0;
        int *preferences = (int *) malloc(sizeof(int) * 5);
        char *current = (char *) malloc(sizeof(char) * 100);
        int currentIndex = 0;
        int option = -1;

        int dataLen = (int) strlen(data);
        for (int j = 0; j < dataLen; j++) {
            // j == dataLen - 1
            if (data[j] == ',' || data[j] == '\n') {
                int value = atoi(current);
                currentIndex = 0;

                for (int k = 0; k < 100; k++) {
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
                    // Convert person's number to list index
                    people[j].preferences[k] = findPersonInList(people, findPersonFromNumber(people, preferences[k]));
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
            if (listContains(people[people[i].preferences[j]].preferences, 5, i) !=
                -1) {

                people[i].connections[people[i].connectionCount] = people[i].preferences[j];
                people[i].connectionCount++;
            } else {
                // Option did not put them down...

                /*

                // Creates connections regardless of whether option put them down too
                // Results in more densely connected node-map
                // Results in fewer and larger communities
                // Nobody will end up in a community alone
                //######################################################################################
                //######################################################################################
                //######################################################################################

                people[i].connections[people[i].connectionCount] = people[i].preferences[j];
                people[people[i].preferences[j]].connections[people[people[i].preferences[j]].connectionCount] = i;

                people[i].connectionCount++;
                people[people[i].preferences[j]].connectionCount++;

                //######################################################################################
                //######################################################################################
                //######################################################################################

                */
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
    int **evaluated = (int **) malloc(sizeof(int *) * peopleCount * 10);
    int evalCount = 0;
    for (int i = 0; i < peopleCount * 10; i++) {
        evaluated[i] = (int *) malloc(sizeof(int) * 2);
        evaluated[i][0] = -1;
        evaluated[i][1] = -1;
    }

    for (int i = 0; i < peopleCount; i++) {
        if (people[i].label == label || label == -123) {
            for (int j = 0; j < people[i].connectionCount; j++) {
                if (people[people[i].connections[j]].label == label || label == -123) {
                    //printf("Got to here => 2\n");
                    bool previouslyEvaluated = false;
                    for (int k = 0; k < evalCount; k++) {
                        if ((evaluated[k][0] == i && evaluated[k][1] == people[i].connections[j]) ||
                            (evaluated[k][0] == people[i].connections[j] && evaluated[k][1] == i)) {
                            previouslyEvaluated = true;
                        }
                    }

                    if (!previouslyEvaluated) {
                        //printf("Got to here => 3\n");
                        //printf("Data: %i\n", evalCount);
                        evaluated[evalCount][0] = i; //people[i].number;
                        //printf("Got to here => 4\n");
                        evaluated[evalCount][1] = people[i].connections[j];

                        //printf("Found a connection between: %s <> %s\n", people[i].name,
                               //people[people[i].connections[j]].name);

                        evalCount++;

                        //printf("Got to here => 5\n");

                        res++;
                    }
                }
            }
        }
    }

    return res;
}

double sqr(double val) {
    return val * val;
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

    //for (int i = 0; i < labelCount; i++) {
        //printf("Label %i: %i\n", i, labels[i]);
    //}

    double res = 0;
    for (int i = 0; i < labelCount; i++) {
        double partitionRelationships = (double) connectionCount(people, labels[i]);
        double totalRelationships = (double) connectionCount(people, -123);
        double degreeNodes = (double) degreeOfNodes(people, labels[i]);

        double modularity =
                (partitionRelationships / totalRelationships) - sqr(degreeNodes / (2 * totalRelationships));
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

            /*
            if (iterations % 10000 == 0) {
                for (int i = 0; i < peopleCount; i++) {
                    printf("%i, ", permutation[i]);
                }
                printf("\n");
                printf("Modularity: %f\n", modularity);
            }
             */
        }

        iterations++;
    }

    //for (int i = 0; i < peopleCount; i++) {
        //printf("%i ", optimum[i]);
        //people[i].label = optimum[i];
    //}
    //printf("\n");
    return people;
}

int **louvain(struct Person *people) {
    bool continueToFindCommunities = true;
    int label = 0;

    while (continueToFindCommunities) {
        // Find a single community
        bool cont = true;
        int startNode = -1;
        int maxLabel = -1;

        // Find the first unlabeled node
        for (int i = 0; i < peopleCount; i++) {
            if (people[i].label == -1) {
                startNode = i;
                printf("%f%% ###### %i/%i\n", (double) i / peopleCount * 100, i, peopleCount);
                break;
            }
        }

        if (startNode == -1) {
            // No more free nodes
            continueToFindCommunities = false;
        } else {
            people[startNode].label = label;

            while (cont) {
                //printf("Evaluating %s\n", people[startNode].name);

                // Current modularity score
                double currentModularity = calculateModularity(people);
                double maxModularityDelta = -1000;
                int maxIndex = 0;

                for (int i = 0; i < people[startNode].connectionCount; i++) {
                    int optionIndex = people[startNode].connections[i];

                    //printf("Checking option %s\n", people[optionIndex].name);

                    int prevLabel = people[optionIndex].label;
                    people[optionIndex].label = label;

                    double newModularity = calculateModularity(people);

                    //printf("Got a modularity score of: %f\n", newModularity);

                    if ((newModularity - currentModularity) > maxModularityDelta) {
                        //printf("Found a new high score for modularity: %s\n", people[optionIndex].name);

                        maxModularityDelta = (newModularity - currentModularity);
                        maxIndex = optionIndex;
                        maxLabel = people[optionIndex].label;
                    }

                    people[optionIndex].label = prevLabel;
                }

                if (maxModularityDelta <= 0) {
                    //printf("Found an entire community, moving on to the next one now\n");
                    cont = false;
                } else {
                    // Check if label is different
                    if (people[maxIndex].label < 0) {
                        people[maxIndex].label = label;
                    } else {
                        people[maxIndex].label = maxLabel;
                    }
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
    printf("Generate Classes function does not do anything yet...\n");

    return NULL;
}

int main(int argumentCount, char **arguments) {
    if (argumentCount != 4) {
        printf("Invalid number of arguments passed...\n");
        printf("Must pass:\n");
        printf("\t> Number of people\n");
        printf("\t> Directory to file containing name of person and number\n");
        printf("\t> Directory to file containing number of person and options\n");

        printf("Press enter to exit\n");
        char *stop = malloc(sizeof(char) * 1);
        fgets(stop, 1, stdin);

        return 1;
    } else {
        peopleCount = atoi(arguments[1]);

        // "./namesAndNumbers.csv"
        peopleAndNumbers = fopen(arguments[2], "r");

        // "./numbersAndPreferences.csv"
        numbersAndPreferences = fopen(arguments[3], "r");
    }

    struct Person *people = ReadFileInput();

    int **communities = louvain(people);

    printf("Press enter to exit\n");
    char *getInputNow = malloc(sizeof(char) * 5);
    fgets(getInputNow, 5, stdin);

    return 0;
}
