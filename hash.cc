#include "hash.h"

#include <unordered_map>
#include <unordered_set>
#include <vector>

using jnp1::const_sequence_t;
using jnp1::element_t;
using jnp1::hash_function_t;
using jnp1::table_id_t;

#ifdef NDEBUG
const bool debug = false;
#else
const bool debug = true;
#endif

namespace {
struct sequence {
  ::std::vector<element_t> v;
  hash_function_t hash_function;

  bool operator==(const sequence& other) const { return v == other.v; }
};
}  // namespace

namespace std {
template <>
struct hash<sequence> {
  size_t operator()(const sequence& s) const {
    return s.hash_function(s.v.data(), s.v.size());
  }
};
}  // namespace std

namespace {

using hash_set_t = ::std::unordered_set<sequence>;
using function_set_pair_t = ::std::pair<hash_function_t, hash_set_t>;
using tables_t = ::std::unordered_map<table_id_t, function_set_pair_t>;

tables_t& tables() {
  // To prevent order initalization fiasco, the object is created on first use.
  static tables_t* x = new tables_t();
  return *x;
}

table_id_t next_id = 0;

bool table_exists(table_id_t id) { return tables().count(id); }

bool table_contains_sequence(table_id_t id, const_sequence_t seq, size_t size) {
  function_set_pair_t pair = tables().at(id);
  return pair.second.find({{seq, seq + size}, pair.first}) != pair.second.end();
}

bool table_and_sequence_are_valid(table_id_t id, const_sequence_t seq,
                                  size_t size) {
  return table_exists(id) && seq != NULL && size > 0;
}
}  // namespace

namespace jnp1 {

table_id_t hash_create(hash_function_t hash_function) {
  if (debug) {
    std::cerr << "hash_create(" << hash_function << ")\n";
  }

  table_id_t id = next_id++;
  tables()[id] = {hash_function, {}};

  if (debug) {
    std::cerr << "hash_create: hash table #" << id << " created\n";
  }

  return id;
}

void hash_delete(table_id_t id) {
  if (table_exists(id)) {
    tables().erase(id);
  }
}

size_t hash_size(table_id_t id) {
  if (!table_exists(id)) {
    return 0;
  }

  return tables()[id].second.size();
}

bool hash_insert(table_id_t id, const_sequence_t seq, size_t size) {
  if (!table_and_sequence_are_valid(id, seq, size) ||
      table_contains_sequence(id, seq, size)) {
    return false;
  }

  function_set_pair_t pair = tables().at(id);
  tables()[id].second.insert({{seq, seq + size}, pair.first});

  return true;
}

bool hash_remove(table_id_t id, const_sequence_t seq, size_t size) {
  if (!table_and_sequence_are_valid(id, seq, size) ||
      !table_contains_sequence(id, seq, size)) {
    return false;
  }

  function_set_pair_t pair = tables().at(id);
  tables().at(id).second.erase({{seq, seq + size}, pair.first});

  return true;
}

void hash_clear(table_id_t id) {
  if (table_exists(id)) {
    tables().at(id).second.clear();
  }
}

bool hash_test(table_id_t id, const_sequence_t seq, size_t size) {
  if (!table_and_sequence_are_valid(id, seq, size)) {
    return false;
  }

  return table_contains_sequence(id, seq, size);
}
}  // namespace jnp1