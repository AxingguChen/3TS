#ifndef _SILO_H_
#define _SILO_H_

#include "row.h"

class TxnManager;

enum SILOState {
    SILO_RUNNING = 0,
    SILO_VALIDATED,
    SILO_COMMITTED,
    SILO_ABORTED
};

class Silo {
public:
    RC validate_silo(TxnManager * txn);
    RC find_bound(TxnManager * txn);
};

struct SiloTimeTableEntry{
    uint64_t lower;
    uint64_t upper;
    uint64_t key;
    SILOState state;
    SiloTimeTableEntry * next;
    SiloTimeTableEntry * prev;
    void init(uint64_t key) {
        lower = 0;
        upper = UINT64_MAX;
        this->key = key;
        state = SILO_RUNNING;
        next = NULL;
        prev = NULL;
    }
};

struct SiloTimeTableNode {
    SiloTimeTableEntry * head;
    SiloTimeTableEntry * tail;
    pthread_mutex_t mtx;
    void init() {
        head = NULL;
        tail = NULL;
        pthread_mutex_init(&mtx,NULL);
    }
};

class SiloTimeTable {
public:
    void init();
    void init(uint64_t thd_id, uint64_t key);
    void release(uint64_t thd_id, uint64_t key);
    uint64_t get_lower(uint64_t thd_id, uint64_t key);
    uint64_t get_upper(uint64_t thd_id, uint64_t key);
    void set_lower(uint64_t thd_id, uint64_t key, uint64_t value);
    void set_upper(uint64_t thd_id, uint64_t key, uint64_t value);
    SILOState get_state(uint64_t thd_id, uint64_t key);
    void set_state(uint64_t thd_id, uint64_t key, SILOState value);
private:
    // hash table
    uint64_t hash(uint64_t key);
    uint64_t table_size;
    SiloTimeTableNode* table;
    SiloTimeTableEntry* find(uint64_t key);

    SiloTimeTableEntry * find_entry(uint64_t id);

    sem_t     _semaphore;
};

#endif