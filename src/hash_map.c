#include "hash_map.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "lexer.h"

static void print_map(Hash_map *map) {
    for (int i = 0; i < MAX_BUCKET_CAPACITY; i++) {
        if (map->buckets[i]) {
            Map_item *current = map->buckets[i];

            while (current) {
                printf("%lu, %s, %d", current->hash, current->key, i);
                printf("\n");
                current = current->next;
            }
        }
    }
}

void hash_map_initialize(Hash_map *map) {
    memset(map->buckets, 0, sizeof MAX_BUCKET_CAPACITY);

    char **current = keywords;
    while (*current != NULL) {
        map_put(map, *current);
        current++;
    }
    print_map(map);
}

size_t hash(char *key) {
    unsigned long hash = 5381;
    int c;

    while ((c = *key++)) {
        hash = ((hash << 5) + hash) + c;
    }

    return hash;
}

int map_put(Hash_map *map, char *key) {
    if (!key) return -1;

    size_t result = hash(key);
    size_t index = result % MAX_BUCKET_CAPACITY;

    Map_item *item = malloc(sizeof(Map_item));
    item->hash = result;
    strncpy(item->key, key, MAX_KEY_LEN);
    item->next = NULL;

    if (!map->buckets[index]) {
        map->buckets[index] = item;
    } else {
        // collision
        Map_item *current = map->buckets[index];

        while (current->next != NULL) {
            current = current->next;
        }

        current->next = item;
    }

    return index;
}

int map_has(Hash_map *map, char *key) {
    if (!key) return -1;

    size_t result = hash(key);
    size_t index = result % MAX_BUCKET_CAPACITY;

    if (!map->buckets[index]) {
        return 0;
    }

    Map_item *current = map->buckets[index];

    while (current) {
        if (strcmp(key, current->key) == 0) {
            return 1;
        }

        current = current->next;
    }

    return 0;
}

void map_free(Hash_map *map) {
    for (int i = 0; i < MAX_BUCKET_CAPACITY; i++) {
        if (map->buckets[i]) {
            Map_item *slow = map->buckets[i];
            Map_item *fast = slow->next;

            while (slow) {
                free(slow);
                slow = fast;

                if (fast) {
                    fast = fast->next;
                }
            }
        }
    }
}
