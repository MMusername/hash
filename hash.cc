#include "hash.h"

main_map_t main_map;
unsigned long next_id = 0;

namespace {
    bool id_exist(unsigned long id) {
        return main_map.find(id) != main_map.end();
    }

    bool contains_seq(unsigned long id, uint64_t const * seq, size_t size) {
        function_set_pair_t pair = main_map.at(id);
        seq_t s = {{seq, seq + size}, pair.first};
        return pair.second.find(s) != pair.second.end();
    }

    bool first_test(unsigned long id, uint64_t const * seq, size_t size) {
        if (!id_exist(id) || seq == NULL || size == 0) {
            return false;
        }

        return true;
    }
}

unsigned long hash_create(hash_function_t hash_function) {
    unsigned long id = next_id;
    next_id++;
    hash_set_t hash_set;
    function_set_pair_t pair(hash_function, hash_set);
    main_map.insert({id, pair});
    return id;
}

void hash_delete(unsigned long id) {
    if (id_exist(id)) {
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
    seq_t s = {{seq, seq + size}, pair.first};
    main_map.at(id).second.insert(s);

    return true;
}

bool hash_remove(unsigned long id, uint64_t const * seq, size_t size) {
    if (!first_test(id, seq, size) || !contains_seq(id, seq, size)) {
        return false;
    }

    function_set_pair_t pair = main_map.at(id);
    seq_t s = {{seq, seq + size}, pair.first};
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

