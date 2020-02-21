#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <math.h>

// Person type
struct Person {
    // Each person has 5 preferences;
    char *name;
    char **preferences;
};

struct Person *getPeople(int peopleCount) {
    struct Person *res = (struct Person *) malloc(sizeof(struct Person *) * peopleCount);

    for (int i = 0; i < peopleCount; i++) {
        char *name = (char *) malloc(sizeof(char) * 100);

        char **preferences = (char **) malloc(sizeof(char *) * 5);

        preferences[0] = (char *) malloc(sizeof(char) * 100);
        preferences[1] = (char *) malloc(sizeof(char) * 100);
        preferences[2] = (char *) malloc(sizeof(char) * 100);
        preferences[3] = (char *) malloc(sizeof(char) * 100);
        preferences[4] = (char *) malloc(sizeof(char) * 100);

        /*
        scanf("%s %s %s %s %s %s", name, preferences[0], preferences[1], preferences[2], preferences[3],
              preferences[4]);
        */

        printf("Name: ");
        scanf("%s", name);
        for (int j = 0; j < 5; j++) {
            printf("Option %i: ", j + 1);
            scanf("%s", preferences[j]);
            //printf("\n");
        }

        struct Person person = {.name=name, .preferences=preferences};
        res[i] = person;
    }

    return res;
}

void printStartInfo() {
    printf("\033[31mInput Requirements:\n");
    printf("\033[31m\t1. First input must be person of pupil\n");
    printf("\033[31m\t2. Second to sixth inputs must be the pupil's choices\n");
    printf("\033[31m\t3. Names must be separated by spaces\n");
    printf("\033[31m\t4. Names must not contain spaces\n");
    printf("\033[31m\t5. Names must be exact: John_Smith, John-Smith and john_Smith are all viewed as different people\n");
    printf("\033[31m\t6. Input exactly 6 names, then press enter\033[0m\n");

    printf("\nFor example:\n");

    printf("\033[36m  Name ↓      Option 1 ↓      Option 2 ↓   Option 3 ↓  Option 4 ↓      Option 5 ↓\n");
    printf("\033[35mHarry_Potter Ronald_Weasly Hermione_Granger Cho_Chang Ginny_Weasly Neville_Longbottom\033[0m\n");
}

struct Node {
    struct Person person;
    struct Connection *connections;
    int connectionCount;
};

struct Connection {
    int tailIndex;
    int headIndex;
    double weight;
};

struct NodeMap {
    struct Node *nodes;
};

struct NodeMap generateNodeMap(struct Person *people, int len) {
    struct NodeMap nodeMap;
    struct Node *nodes = (struct Node *) malloc(sizeof(struct Node) * len * 2);
    nodeMap.nodes = nodes;
    int nodeIndex = 0;

    /*
     * 1. Loop all people
     * 2. For each person loop all options
     * 3. For each option check it exists as a person elsewhere
     * 3. For each option get the index
     * 4. For each option find option in people list and find reverse index
     */

    // Loop over all people
    for (int i = 0; i < len; i++) {
        printf("Evaluating %s\n", people[i].name);
        struct Node node = {.person = people[i], .connectionCount=0};
        node.connections = (struct Connection *) malloc(sizeof(struct Connection) * len);
        int connectionIndex = 0;

        // Loop over all options
        for (int j = 0; j < 5; j++) {
            int pupil2Index = -1;
            int pupil1Index = -1;
            double pupil2Rank = 10;
            double pupil1Rank = 10;

            // Check person exists, if so, mark it as a valid option
            for (int k = 0; k < len; k++) {
                if (!strcmp(people[i].preferences[j], people[k].name)) {
                    // Option is a valid person and therefore can be evaluated accordingly
                    printf("%s's option %s was found\n", people[i].name, people[i].preferences[j]);
                    pupil2Index = k;
                    pupil2Rank = j + 1;

                    // Find current pupil in optional pupil's preferences
                    for (int l = 0; l < 5; l++) {
                        if (!strcmp(people[k].preferences[l], people[i].name)) {
                            // Optional pupil also put down original pupil so can be evaluated
                            printf("%s's option %s was found\n", people[i].preferences[j], people[i].name);
                            pupil1Index = i;
                            pupil1Rank = l + 1;
                        }
                    }
                }
            }

            if (pupil1Index != -1 && pupil2Index != -1) {
                printf("Information:\n");
                printf("\tName: %s\n", people[pupil1Index].name);
                printf("\tOption: %s\n", people[pupil2Index].name);
                printf("\tPupil 1 index: %i\n", pupil1Index);
                printf("\tPupil 2 index: %i\n", pupil2Index);
                printf("\tPupil 1 rank: %f\n", pupil1Rank);
                printf("\tPupil 2 rank: %f\n", pupil2Rank);
                double connectionWeight = pow(((double) pupil1Rank + (double) pupil2Rank) / 2, 2);
                struct Connection newConnection = {.tailIndex=pupil1Index, .headIndex=pupil2Index, .weight=connectionWeight};
                node.connections[connectionIndex] = newConnection;
                node.connectionCount++;
                connectionIndex++;
            }
        }
        nodeMap.nodes[nodeIndex] = node;
        nodeIndex++;
    }

    return nodeMap;
}


int friendIndex = 0;
struct Node *evaluated;
int evalCount = 0;

void getFriends(struct NodeMap nodeMap, struct Person *people, int index, double threashold, double recurseDelta,
                struct Person *friends) {
    struct Node node = nodeMap.nodes[index];
    printf("Node name: %s\n", node.person.name);
    printf("Node connections: %i\n", node.connectionCount);
    printf("Parent node: %s\n", evaluated[evalCount - 1].person.name);

    for (int i = 0; i < node.connectionCount; i++) {
        printf("\n\nNAMES: %s, %s\n\n", people[node.connections[i].headIndex].name,
               evaluated[evalCount - 1].person.name);

        bool eval = true;
        for (int j = 0; j < evalCount; j++) {
            printf("Some important testing data: %s, %s\n", node.person.name,
                   evaluated[j].person.name);
            if (!strcmp(node.person.name, evaluated[j].person.name)) {
                eval = false;
                printf("Not evaluating this node\n");
            }
        }

        if (eval) {
            printf("Node connection %i: %s\n", i, nodeMap.nodes[node.connections[i].headIndex].person.name);
            printf("Node connects to index: %i\n", node.connections[i].headIndex);

            friends[friendIndex] = people[node.connections[i].headIndex];
            friendIndex++;

            printf("Got to here my dudes\n");

            evaluated[evalCount] = node;
            evalCount++;

            if (threashold - node.connections[i].weight > 0) {
                getFriends(nodeMap, people, node.connections[i].headIndex,
                           threashold - node.connections[i].weight - recurseDelta,
                           recurseDelta, friends);
            }
        }
    }
}

int main(void) {
    printf("Class Creator\n\n");

    printStartInfo();

    // Get input names
    printf("Number of people to evaluate: >");
    int peopleCount;
    scanf("%i", &peopleCount);

    printf("Please input the person of the person followed by their choices \n");

    struct Person *people = getPeople(peopleCount);

    for (int i = 0; i < peopleCount; i++) {
        printf("Name: %s\n", people[i].name);
        for (int j = 0; j < 5; j++) {
            printf("Option %i: %s\n", j + 1, people[i].preferences[j]);
        }
    }

    printf("\033[0m");

    printf("Generating node map...\n");
    struct NodeMap nodeMap = generateNodeMap(people, peopleCount);

    printf("Information: %s\n", nodeMap.nodes[0].person.name);

    for (int i = 0; i < peopleCount; i++) {
        printf("Name: %s\n", nodeMap.nodes[i].person.name);

        for (int j = 0; j < nodeMap.nodes[i].connectionCount; j++) {
            printf("Connection %i: %s -- %f\n", j, people[nodeMap.nodes[i].connections[j].headIndex].name,
                   nodeMap.nodes[i].connections[j].weight);
        }
    }

    printf("\n\nFinding friends\n");

    // NEEDED!
    friendIndex = 0;
    struct Person *friends = (struct Person *) malloc(sizeof(struct Person) * peopleCount);
    struct Person emptyPerson = {.name="EMPTY_PERSON"};
    evaluated = (struct Node *) malloc(sizeof(struct Node) * peopleCount);
    struct Node emptyNode = {.person=emptyPerson};
    evaluated[0] = emptyNode;
    evalCount = 1;
    getFriends(nodeMap, people, 1, 15, 0, friends);

    for (int i = 0; i < friendIndex; i++) {
        printf("Found friend: %s\n", friends[i].name);
    }

    return 0;
}
