#ifndef _HMAP

#define _HMAP
#define MAX_BUCKET_CAPACITY 100
#define MAX_KEY_LEN 256

typedef struct Map_item {
    char key[MAX_KEY_LEN];
    struct Map_item *next;
    unsigned long hash;
} Map_item;

typedef struct {
    Map_item *buckets[MAX_BUCKET_CAPACITY];
} Hash_map;

void hash_map_initialize(Hash_map *map);
int map_put(Hash_map *map, char *key);
// Map_item *map_get(char *key);
int map_has(Hash_map *map, char *key);
unsigned long hash(char *key);
void map_free(Hash_map *map);

#endif