#include "hash.h"

main_map_t main_map;
queue_t id_queue;

namespace {
    bool id_exist(unsigned long id) {
        return main_map.find(id) != main_map.end();
    }

    unsigned long next_id() {
        if (!id_queue.empty()) {
            unsigned long id = id_queue.front();
            id_queue.pop();
            return id;
        }

        return main_map.size();
    }

    seq_t new_seq(hash_function_t hash_function, uint64_t const * seq, size_t size) {
        seq_t s;
        s.hash_function = hash_function;
        vector_t vec(seq, seq + size);
        s.v = vec;
        return s;
    }

    bool contains_seq(unsigned long id, uint64_t const * seq, size_t size) {
        function_set_pair_t pair = main_map.at(id);
        seq_t s = new_seq(pair.first, seq, size);
        return pair.second.find(s) != pair.second.end();
    }

    // poprawność danych
    bool first_test(unsigned long id, uint64_t const * seq, size_t size) {
        if (!id_exist(id) || seq == NULL || size == 0) {
            return false;
        }

        return true;
    }
}

unsigned long hash_create(hash_function_t hash_function) {
    unsigned long id = next_id();
    hash_set_t hash_set;
    function_set_pair_t pair(hash_function, hash_set);
    main_map.insert({id, pair});
    return id;
}

void hash_delete(unsigned long id) {
    if (id_exist(id)) {
        id_queue.push(id);
        main_map.erase(id);
    }
}

size_t hash_size(unsigned long id) {
    if (!id_exist(id)) {
        return 0;
    }

    return main_map.at(id).second.size();
}
 
bool hash_insert(unsigned long id, uint64_t const * seq, size_t size) {
    if (!first_test(id, seq, size) || contains_seq(id, seq, size)) {
        return false;
    }
    
    function_set_pair_t pair = main_map.at(id);
    seq_t s = new_seq(pair.first, seq, size);
    main_map.at(id).second.insert(s);

    return true;
}

bool hash_remove(unsigned long id, uint64_t const * seq, size_t size) {
    if (!first_test(id, seq, size) || !contains_seq(id, seq, size)) {
        return false;
    }

    function_set_pair_t pair = main_map.at(id);
    seq_t s = new_seq(pair.first, seq, size);
    main_map.at(id).second.erase(s);

    return true;
}

void hash_clear(unsigned long id) {
    if (id_exist(id)) {
        main_map.at(id).second.clear();
    }
}

bool hash_test(unsigned long id, uint64_t const * seq, size_t size) {
    if (!first_test(id, seq, size)) {
        return false;
    }
    
    return contains_seq(id, seq, size);
}

