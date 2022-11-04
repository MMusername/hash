#include "hash.h"

#include <cassert>
#include <sstream>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

using jnp1::const_sequence_t;
using jnp1::element_t;
using jnp1::hash_function_t;
using jnp1::table_id_t;

using vector_t = ::std::vector<element_t>;
using function_vector_pair_t = ::std::pair<hash_function_t, vector_t>;

namespace {

#ifdef NDEBUG
const bool debug = false;
#else
const bool debug = true;
#endif

struct HashingStruct {
  hash_function_t hash_function;

  size_t operator()(const vector_t &v) const {
    return hash_function(v.data(), v.size());
  }
};

using hash_set_t = ::std::unordered_set<vector_t, HashingStruct>;
using function_set_pair_t = ::std::pair<hash_function_t, hash_set_t>;
using tables_t = ::std::unordered_map<table_id_t, function_set_pair_t>;

tables_t &tables() {
  // To prevent order initalization fiasco, the object is created on first use.
  static tables_t *x = new tables_t();
  return *x;
}

table_id_t next_id = 0;

std::string table_id_to_string(table_id_t id) {
  return "hash table #" + std::to_string(id);
}

bool table_contains_sequence(table_id_t id, const_sequence_t seq, size_t size) {
  auto pair = tables().at(id);

  return pair.second.find({seq, seq + size}) != pair.second.end();
}

bool check_if_table_exists(table_id_t id, std::string source_function) {
  bool result = tables().count(id);

  if (!result && debug) {
    if (!source_function.empty()) {
      std::cerr << source_function << ": " << table_id_to_string(id);
    }
    std::cerr << " does not exist\n";
  }

  return result;
}

bool check_if_sequence_is_valid(const_sequence_t seq, size_t size,
                                std::string source_function) {
  bool result = true;
  if (seq == NULL) {
    result = false;
    if (debug) {
      std::cerr << source_function << ": invalid pointer (NULL)\n";
    }
  }
  if (size == 0) {
    result = false;
    if (debug) {
      std::cerr << source_function << ": invalid size (0)\n";
    }
  }
  return result;
}

bool check_if_sequence_is_valid_and_table_exists(table_id_t id,
                                                 const_sequence_t seq,
                                                 size_t size,
                                                 std::string source_function) {
  if (!check_if_sequence_is_valid(seq, size, source_function)) return false;
  if (!check_if_table_exists(id, source_function)) return false;
  return true;
}

std::string sequence_to_string(const_sequence_t seq, size_t size) {
  if (seq == NULL) return "NULL";

  std::stringstream ss("");
  ss << '"';
  for (size_t i = 0; i < size; i++) {
    if (i > 0) ss << " ";
    ss << seq[i];
  }
  ss << '"';
  return ss.str();
}

void print_table_and_sequence_arguments(table_id_t id, const_sequence_t seq,
                                        size_t size) {
  std::cerr << "(" << id << ", " << sequence_to_string(seq, size) << ", "
            << size << ")\n";
}

}  // namespace

namespace jnp1 {
table_id_t hash_create(hash_function_t hash_function) {
  assert(hash_function != NULL);
  if (debug) std::cerr << __func__ << "(" << hash_function << ")\n";

  table_id_t id = next_id++;
  tables()[id] = {hash_function, hash_set_t(0, {hash_function})};

  if (debug)
    std::cerr << __func__ << ": " << table_id_to_string(id) << " created\n";
  return id;
}

void hash_delete(table_id_t id) {
  if (debug) std::cerr << __func__ << "(" << id << ")\n";

  if (!check_if_table_exists(id, __func__)) return;

  if (debug)
    std::cerr << __func__ << ": " << table_id_to_string(id) << " deleted\n";
  tables().erase(id);
}

size_t hash_size(table_id_t id) {
  if (debug) std::cerr << __func__ << "(" << id << ")\n";

  if (!check_if_table_exists(id, __func__)) return 0;

  size_t size = tables()[id].second.size();

  if (debug)
    std::cerr << __func__ << ": " << table_id_to_string(id) << " contains "
              << size << " element(s)\n";

  return size;
}

bool hash_insert(table_id_t id, const_sequence_t seq, size_t size) {
  if (debug) {
    std::cerr << "hash_insert";
    print_table_and_sequence_arguments(id, seq, size);
  }

  if (!check_if_sequence_is_valid_and_table_exists(id, seq, size, __func__))
    return false;

  if (debug)
    std::cerr << "hash_insert: " << table_id_to_string(id) << ", sequence "
              << sequence_to_string(seq, size);

  if (table_contains_sequence(id, seq, size)) {
    if (debug) std::cerr << " was present\n";
    return false;
  }

  function_set_pair_t pair = tables().at(id);
  tables()[id].second.insert({seq, seq + size});

  if (debug) std::cerr << " inserted\n";
  return true;
}

bool hash_remove(table_id_t id, const_sequence_t seq, size_t size) {
  if (debug) {
    std::cerr << __func__;
    print_table_and_sequence_arguments(id, seq, size);
  }

  if (!check_if_sequence_is_valid_and_table_exists(id, seq, size, __func__))
    return false;

  if (debug)
    std::cerr << __func__ << ": " << table_id_to_string(id) << ", sequence "
              << sequence_to_string(seq, size);

  if (!table_contains_sequence(id, seq, size)) {
    if (debug) std::cerr << " was not present\n";
    return false;
  }

  function_set_pair_t pair = tables().at(id);
  tables().at(id).second.erase({seq, seq + size});

  if (debug) std::cerr << " removed\n";
  return true;
}

void hash_clear(table_id_t id) {
  if (debug)
    std::cerr << __func__ << "(" << id << ")\n"
              << __func__ << ": " << table_id_to_string(id);

  if (check_if_table_exists(id, "")) {
    if (tables().at(id).second.empty()) {
      if (debug) std::cerr << " was empty\n";
      return;
    }

    tables().at(id).second.clear();

    if (debug) std::cerr << " cleared\n";
  }
}

bool hash_test(table_id_t id, const_sequence_t seq, size_t size) {
  if (debug) {
    std::cerr << __func__;
    print_table_and_sequence_arguments(id, seq, size);
  }

  if (!check_if_sequence_is_valid_and_table_exists(id, seq, size, __func__))
    return false;

  if (debug)
    std::cerr << __func__ << ": " << table_id_to_string(id) << ", sequence "
              << sequence_to_string(seq, size);

  if (table_contains_sequence(id, seq, size)) {
    if (debug) std::cerr << " is present\n";
    return true;
  }

  if (debug) std::cerr << " is not present\n";
  return false;
}

}  // namespace jnp1
