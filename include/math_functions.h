#pragma once

#include <cmath>
#include <vector>

template <typename T> void merge(int* array, int const left, int const mid, int const right, std::vector<T>& vector);

template <typename T> void mergeSort(int* array, int const begin, int const end, std::vector<T>& vector);

template <typename T> void orderVectors(std::vector<T>& particles);

