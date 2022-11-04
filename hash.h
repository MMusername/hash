#ifndef HASH_H
#define HASH_H

#ifdef __cplusplus
#include <cstddef>
#include <cstdint>

// This must be included here, so that the static stream objects are created
// before any function can be called.
#include <iostream>

#else
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#endif

#ifdef __cplusplus
namespace jnp1 {
extern "C" {
#endif

typedef unsigned long table_id_t;  // Id jakiejś tablicy haszującej.

typedef uint64_t element_t;  // Liczba całkowita, element ciągu.
typedef element_t const *const_sequence_t;  // Ciąg liczb całkowitych.

typedef size_t hash_value_t;  // Wartość zwracana przez funkcję haszującą.
typedef hash_value_t (*hash_function_t)(const_sequence_t,
                                        size_t);  // Funkcja haszująca ciągi.

/// Tworzy tablicę haszującą i zwraca jej identyfikator.
table_id_t hash_create(hash_function_t f);

/// Usuwa tablicę haszującą o identyfikatorze id, o ile ona istnieje. W
/// przeciwnym przypadku nic nie robi.
void hash_delete(table_id_t id);

/// Daje liczbę ciągów przechowywanych w tablicy haszującej o
/// identyfikatorze id lub 0, jeśli taka tablica nie istnieje.
size_t hash_size(table_id_t id);

/// Wstawia do tablicy haszującej o identyfikatorze id ciąg liczb całkowitych
/// seq o długości size. Wynikiem jest informacja, czy operacja się powiodła.
/// Operacja się nie powiedzie, jeśli nie ma takiej tablicy haszującej, jeśli
/// tablica haszująca zawiera już taki ciąg, jeśli parametr seq ma wartość NULL
/// lub parametr size ma wartość 0.
bool hash_insert(table_id_t id, const_sequence_t seq, size_t size);

/// Usuwa z tablicy haszującej o identyfikatorze id ciąg liczb całkowitych
/// seq o długości size. Wynikiem jest informacja, czy operacja się
/// powiodła. Operacja się nie powiedzie, jeśli nie ma takiej tablicy
/// haszującej, jeśli tablica haszująca nie zawiera takiego ciągu,
/// jeśli parametr seq ma wartość NULL lub parametr size ma wartość 0.
bool hash_remove(table_id_t id, const_sequence_t seq, size_t size);

/// Jeśli tablica haszująca o identyfikatorze id istnieje i nie jest pusta,
/// to usuwa z niej wszystkie elementy. W przeciwnym przypadku nic nie robi.
void hash_clear(table_id_t id);

/// Daje wynik true, jeśli istnieje tablica haszująca o identyfikatorze id
/// i zawiera ona ciąg liczb całkowitych seq o długości size. Daje wynik
/// false w przeciwnym przypadku oraz gdy parametr seq ma wartość NULL lub
/// parametr size ma wartość 0.
bool hash_test(table_id_t id, const_sequence_t seq, size_t size);

#ifdef __cplusplus
}  // extern
}  // namespace
#endif

#endif
