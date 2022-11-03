#include "hash.h"

#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <string>

using jnp1::const_sequence_t;
using jnp1::element_t;
using jnp1::hash_function_t;
using jnp1::table_id_t;

using vector_t = ::std::vector<element_t>;
using function_vector_pair_t = ::std::pair<hash_function_t, vector_t>;

#ifdef NDEBUG
const bool debug = false;
#else
const bool debug = true;
#endif

// Struktura hashująca
struct Hash {
  hash_function_t hash_function;

  size_t operator()(const vector_t& v) const {
    return hash_function(v.data(), v.size());
  }
};

namespace {

using hash_set_t = ::std::unordered_set<vector_t, Hash>;
using function_set_pair_t = ::std::pair<hash_function_t, hash_set_t>;
using tables_t = ::std::unordered_map<table_id_t, function_set_pair_t>;
using ::std::string;

tables_t& tables() {
  // To prevent order initalization fiasco, the object is created on first use.
  static tables_t* x = new tables_t();
  return *x;
}

table_id_t next_id = 0;

void print_table_id(table_id_t id) {
  std::cerr << "hash table #" << id;
}

bool table_contains_sequence(table_id_t id, const_sequence_t seq, size_t size) {
  function_set_pair_t pair = tables().at(id);

  return pair.second.find({seq, seq + size}) != pair.second.end();
}

bool table_exists(table_id_t id, string function) {
  bool result = tables().count(id);
  if (!result && debug) {
    if (!function.empty()) {
      std::cerr << function << ": ";
      print_table_id(id);
    }
    std::cerr << " does not exist\n";
  }
  return result;
}

bool table_and_sequence_are_valid(table_id_t id, const_sequence_t seq,
                                  size_t size, string function) {
  bool result = true;
  if (!table_exists(id, function)) {
    result = false;
  }
  if (seq == NULL) {
    result = false;
    if (debug) {
      std::cerr << function << ": invalid pointer (NULL)\n";
    }
  }
  if (size == 0) {
    result = false;
    if (debug) {
      std::cerr << function << ": invalid size (0)\n";
    }
  }
  return result;
}

void print_sequence(const_sequence_t seq, size_t size) {
  if (seq == NULL) {
    std::cerr << "NULL";
  } else {
    std::cerr << "\"";
    if (size != 0) {
      for (size_t i = 0; i < size - 1; i++) {
        std::cerr << seq[i] << " ";
      }
      std::cerr << seq[size - 1];
    }
    std::cerr << "\"";
  }
}

void print_id_seq_size(table_id_t id, const_sequence_t seq, size_t size) {
  std::cerr << "(" << id << ", ";
  print_sequence(seq, size);
  std::cerr << ", " << size << ")\n";
}

}   // namespace

namespace jnp1 {
table_id_t hash_create(hash_function_t hash_function) {
  if (debug) {
    std::cerr << "hash_create(" << hash_function << ")\n";
  }

  table_id_t id = next_id++;
  tables()[id] = {hash_function, hash_set_t(0, {hash_function})};
  if (debug) {
    std::cerr << "hash_create: "; 
    print_table_id(id);
    std::cerr << " created\n";
  }

  return id;
}

void hash_delete(table_id_t id) {
  if (debug) {
    std::cerr << "hash_delete(" << id << ")\n";
    std::cerr << "hash_delete: ";
    print_table_id(id);
  }

  if (table_exists(id, "")) {
    if (debug) {
      std::cerr << " deleted\n";
    }
    tables().erase(id);
  }
}

size_t hash_size(table_id_t id) {
  if (debug) {
    std::cerr << "hash_size(" << id << ")\n";
    std::cerr << "hash_size: ";
    print_table_id(id);
  }
    
  size_t size = 0;
  if (table_exists(id, "")) {
    size = tables()[id].second.size();
    if (debug) {
      std::cerr << " contains " << size << " element(s)\n";
    }
  }

  return size;
}

bool hash_insert(table_id_t id, const_sequence_t seq, size_t size) {
  if (debug) {
      std::cerr << "hash_insert";
      print_id_seq_size(id, seq, size);
  }

  if (!table_and_sequence_are_valid(id, seq, size, "hash_insert")) {
    return false;
  }
  if (debug) {
    std::cerr << "hash_insert: ";
    print_table_id(id);
    std::cerr << ", sequence ";
    print_sequence(seq, size); 
  }
  if (table_contains_sequence(id, seq, size)) {
    if (debug) {
      std::cerr << " was present\n";
    }
    return false;
  }

  function_set_pair_t pair = tables().at(id);
  tables()[id].second.insert({seq, seq + size});

  if (debug) {
    std::cerr << " inserted\n";
  }
  return true;
}

bool hash_remove(table_id_t id, const_sequence_t seq, size_t size) {
  if (debug) {
      std::cerr << "hash_remove";
      print_id_seq_size(id, seq, size);
  }
  if (!table_and_sequence_are_valid(id, seq, size, "hash_remove")) {
    return false;
  }
  if (debug) {
    std::cerr << "hash_remove: ";
    print_table_id(id);
    std::cerr << ", sequence ";
    print_sequence(seq, size); 
  }
  if (!table_contains_sequence(id, seq, size)) {
    std::cerr << " was not present\n";
    return false;
  }

  function_set_pair_t pair = tables().at(id);
  tables().at(id).second.erase({seq, seq + size});

  if (debug) {
    std::cerr << " removed\n";
  }
  return true;
}

void hash_clear(table_id_t id) {
  if (debug) {
    std::cerr << "hash_clear(" << id << ")\n";
    std::cerr << "hash_clear: ";
    print_table_id(id);
  }
    
  if (table_exists(id, "")) {
    if (!tables().at(id).second.empty()) {
      tables().at(id).second.clear();
      if (debug) {
        std::cerr << "cleared\n";
      }
    } else if (debug) {
      std::cerr << " was empty\n";
    }
  }
}

bool hash_test(table_id_t id, const_sequence_t seq, size_t size) {
  if (debug) {
      std::cerr << "hash_test";
      print_id_seq_size(id, seq, size);
  }
  if (!table_and_sequence_are_valid(id, seq, size, "hash_test")) {
    return false;
  }
  if (debug) {
    std::cerr << "hash_test: ";
    print_table_id(id);
    std::cerr << ", sequence ";
    print_sequence(seq, size);
  }
  if (table_contains_sequence(id, seq, size)) {
    std::cerr << " is present\n";
    return true;
  }
  std::cerr << " is not present\n";
  return false;
}
}  // namespace jnp1

