#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <time.h>

#define CAR_WEIGHT 200
#define VAN_WEIGHT 300
#define MAX_BRIDGE_WEIGHT 1200
#define MAX_VEHICLES 30


typedef enum { CAR, VAN } VehicleType;                          // defining vehicle type & direction
typedef enum { NORTH, SOUTH } VehicleDirection;


typedef struct {                                                // defining vehicle struct
    int id;
    VehicleType type;
    VehicleDirection direction;
} Vehicle;


typedef struct {                                                // defining bridge structure
    int weight;
    VehicleDirection direction;
    pthread_mutex_t lock;
    pthread_cond_t northQueue, southQueue;
    int northWaiting, southWaiting;                             // track waiting vehicles in each direction
} Bridge;

Bridge bridge;

float rand_prob() {
    return (float) rand() / RAND_MAX;
}

void *vehicle_routine(void *arg);
void arrive(Vehicle *v);
void cross(Vehicle *v);
void leave(Vehicle *v);

int main() {
    srand(time(NULL));                                              // seed the random number generator
    pthread_t vehicles[MAX_VEHICLES];
    Vehicle v[MAX_VEHICLES];

    pthread_mutex_init(&bridge.lock, NULL);                    // initializing the bridge
    pthread_cond_init(&bridge.northQueue, NULL);
    pthread_cond_init(&bridge.southQueue, NULL);
    bridge.weight = 0;
    bridge.direction = NORTH;
    bridge.northWaiting = 0;
    bridge.southWaiting = 0;

    printf("Enter number of groups in the schedule: ");
    int num_groups;
    scanf("%d", &num_groups);

    for (int i = 0; i < num_groups; i++) {
        printf("Enter number of vehicles in group %d: ", i+1);
        int num_vehicles_in_group;
        scanf("%d", &num_vehicles_in_group);

        printf("Enter Northbound/Southbound probability for group %d: ", i+1);
        float north_prob, south_prob;
        scanf("%f/%f", &north_prob, &south_prob);

        printf("Enter the delay before the next group (if applicable): ");
        int delay;
        scanf("%d", &delay);

        for (int j = 0; j < num_vehicles_in_group; j++) {
            v[j].id = j+1;

            float direction_prob = rand_prob();
            if (direction_prob < north_prob) {
                v[j].direction = NORTH;
            } else {
                v[j].direction = SOUTH;
            }
            if (rand_prob() < 0.5) {                            // randomly determine vehicle type
                v[j].type = CAR;
            } 
            else {                                              
                v[j].type = VAN;
                }               

            pthread_create(&vehicles[j], NULL, vehicle_routine, (void*) &v[j]);
        }

        for (int j = 0; j < num_vehicles_in_group; j++) {
            pthread_join(vehicles[j], NULL);
        }

        if (delay > 0) {
            sleep(delay);
        }
    }


    pthread_mutex_destroy(&bridge.lock);                        // cleanup
    pthread_cond_destroy(&bridge.northQueue);
    pthread_cond_destroy(&bridge.southQueue);

    return 0;
}

void *vehicle_routine(void *arg) {
    Vehicle *v = (Vehicle*) arg;
    arrive(v);
    cross(v);
    leave(v);
    return NULL;
}

void arrive(Vehicle *v) {
    pthread_mutex_lock(&bridge.lock);                           // lock the bridge

    int weight;
    if (v->type == CAR) {
        weight = CAR_WEIGHT;
    }
    else {
        weight = VAN_WEIGHT;
    }

    if(v->direction == NORTH) {
        bridge.northWaiting++;                                  // increment north waiting count
        while((bridge.weight + weight > MAX_BRIDGE_WEIGHT) || (bridge.direction == SOUTH && bridge.weight > 0)) {
            pthread_cond_wait(&bridge.northQueue, &bridge.lock);
        }
        bridge.northWaiting--;                                  // decrement north waiting count
    } else {   // v->direction == SOUTH
        bridge.southWaiting++;                                  // increment south waiting count
        while((bridge.weight + weight > MAX_BRIDGE_WEIGHT) || (bridge.direction == NORTH && bridge.weight > 0)) {
            pthread_cond_wait(&bridge.southQueue, &bridge.lock);
        }
        bridge.southWaiting--;                                  // decrement south waiting count
    }

    bridge.direction = v->direction;
    bridge.weight += weight;

    if (v->type == CAR) {
        printf("Vehicle #%d (", v->id);
        if (v->direction == NORTH) {
            printf("Northbound car) has arrived on the bridge.\n");
        } 
        else {
            printf("Southbound car) has arrived on the bridge.\n");
         }
        }
    else {
        printf("Vehicle #%d (", v->id);
        if (v->direction == NORTH) {
            printf("Northbound van) has arrived on the bridge.\n");
        } 
        else {
            printf("Southbound van) has arrived on the bridge.\n");
        }
    }

    pthread_mutex_unlock(&bridge.lock);                         // unlock the bridge
}

void cross(Vehicle *v) {
    printf("Vehicle #%d is crossing the bridge.\n", v->id);
    sleep(1);
}

void leave(Vehicle *v) {
    pthread_mutex_lock(&bridge.lock);                           // lock the bridge

    int weight;
    if (v->type == CAR) {
        weight = CAR_WEIGHT;
    } else {
        weight = VAN_WEIGHT;
    }

    bridge.weight -= weight;

    if (bridge.direction == NORTH) {
        if (bridge.southWaiting > 0) {
            pthread_cond_signal(&bridge.southQueue);
        } else if (bridge.northWaiting > 0) {
            pthread_cond_signal(&bridge.northQueue);
        }
    } else {   // bridge.direction == SOUTH
        if (bridge.northWaiting > 0) {
            pthread_cond_signal(&bridge.northQueue);
        } else if (bridge.southWaiting > 0) {
            pthread_cond_signal(&bridge.southQueue);
        }
    }

    printf("Vehicle #%d has left the bridge.\n", v->id);

    pthread_mutex_unlock(&bridge.lock);                         // unlock the bridge
}


