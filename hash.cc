#include "hash.h"

#include <unordered_map>
#include <unordered_set>
#include <vector>
using jnp1::const_sequence_t;
using jnp1::element_t;
using jnp1::hash_function_t;
using jnp1::table_id_t;

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
using main_map_t = ::std::unordered_map<table_id_t, function_set_pair_t>;

main_map_t& main_map() {
  static main_map_t x;
  return x;
}
unsigned long next_id = 0;

bool id_exist(unsigned long id) { return main_map().find(id) != main_map().end(); }


bool contains_seq(unsigned long id, const_sequence_t seq, size_t size) {
  function_set_pair_t pair = main_map().at(id);
  return pair.second.find({{seq, seq + size}, pair.first}) != pair.second.end();
}

bool first_test(unsigned long id, const_sequence_t seq, size_t size) {
  if (!id_exist(id) || seq == NULL || size == 0) {
    return false;
  }

  return true;
}
}  // namespace

extern "C" {

table_id_t hash_create(hash_function_t hash_function) {
  table_id_t id = next_id++;
  main_map()[id] = {hash_function, {}};
  return id;
}

void hash_delete(table_id_t id) {
  if (id_exist(id)) {
    main_map().erase(id);
  }
}

size_t hash_size(table_id_t id) {
  if (!id_exist(id)) {
    return 0;
  }

  return main_map().at(id).second.size();
}

bool hash_insert(table_id_t id, const_sequence_t seq, size_t size) {
  if (!first_test(id, seq, size) || contains_seq(id, seq, size)) {
    return false;
  }

  function_set_pair_t pair = main_map().at(id);
  main_map().at(id).second.insert({{seq, seq + size}, pair.first});

  return true;
}

bool hash_remove(table_id_t id, const_sequence_t seq, size_t size) {
  if (!first_test(id, seq, size) || !contains_seq(id, seq, size)) {
    return false;
  }

  function_set_pair_t pair = main_map().at(id);
  main_map().at(id).second.erase({{seq, seq + size}, pair.first});

  return true;
}

void hash_clear(table_id_t id) {
  if (id_exist(id)) {
    main_map().at(id).second.clear();
  }
}

bool hash_test(table_id_t id, const_sequence_t seq, size_t size) {
  if (!first_test(id, seq, size)) {
    return false;
  }

  return contains_seq(id, seq, size);
}
}