struct Table* createTable(int size);

int hashFunction(char* key, int size);

struct Table* insert(char* Newkey, struct Table* hashTable);

int contains(char* key, struct Table* hashTable);

void delete(char* key, struct Table* hashTable);

struct Table* hashTableResize(struct Table* oldHashTable);

void clearTable(struct Table* hashTable);