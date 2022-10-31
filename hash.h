#include <cstddef>
#include <cstdint>
#include <functional>
#include <unordered_set>
#include <vector>

using hash_function_t = auto (*)(uint64_t const * v, size_t n) -> uint64_t;
using vector_t = std::vector<uint64_t>;

struct seq {
    vector_t v;
    hash_function_t hash_function;

    bool operator==(const seq &other) const {
        return v == other.v;
    }
};
using seq_t = struct seq;

namespace std {
    template <> struct hash<seq> {
        size_t operator()(const seq& s) const {
            return s.hash_function(s.v.data(), s.v.size());
        }
    };
}

using hash_set_t = std::unordered_set<seq>;
using function_set_pair_t = std::pair<hash_function_t, hash_set_t>;
using main_map_t = std::unordered_map<unsigned long, function_set_pair_t>;

unsigned long hash_create(hash_function_t hash_function);

void hash_delete(unsigned long id);

size_t hash_size(unsigned long id);

bool hash_insert(unsigned long id, uint64_t const * seq, size_t size);

bool hash_remove(unsigned long id, uint64_t const * seq, size_t size);

void hash_clear(unsigned long id);

bool hash_test(unsigned long id, uint64_t const * seq, size_t size);

