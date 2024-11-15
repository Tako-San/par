#include <algorithm>
#include <cmath>
#include <iostream>
#include <thread>
#include <vector>

using Poly = std::vector<int>;

void print(const Poly &poly) {
  for (size_t i = 0; i < poly.size(); i++) {
    if (i > 0 && poly[i] >= 0)
      std::cout << " + ";
    if (poly[i] != 0)
      std::cout << poly[i];
    if (i > 0 && poly[i] != 0)
      std::cout << "x^" << i;
  }
  std::cout << std::endl;
}

// O(n * m) time complexity
Poly naiveMultiply(const Poly &A, const Poly &B) {
  int n = A.size();
  int m = B.size();
  Poly result(n + m - 1, 0);

  for (int i = 0; i < n; i++) {
    for (int j = 0; j < m; j++) {
      result[i + j] += A[i] * B[j];
    }
  }
  return result;
}

Poly add(const Poly &A, const Poly &B) {
  size_t n = std::max(A.size(), B.size());
  Poly result(n, 0);
  for (size_t i = 0; i < A.size(); i++)
    result[i] += A[i];
  for (size_t i = 0; i < B.size(); i++)
    result[i] += B[i];
  return result;
}

Poly sub(const Poly &A, const Poly &B) {
  size_t n = std::max(A.size(), B.size());
  Poly result(n, 0);
  for (size_t i = 0; i < A.size(); i++)
    result[i] += A[i];
  for (size_t i = 0; i < B.size(); i++)
    result[i] -= B[i];
  return result;
}

// Karatsuba Polynomial Multiplication
Poly karatsubaMultiply(const Poly &A, const Poly &B) {
  size_t n = A.size();
  size_t m = B.size();

  if (n == 1 && m == 1) {
    return {A[0] * B[0]};
  }

  size_t size = std::max(n, m);
  size_t newSize = 1 << static_cast<int>(std::ceil(std::log2(size)));
  Poly A_resized = A;
  Poly B_resized = B;
  A_resized.resize(newSize, 0);
  B_resized.resize(newSize, 0);

  size_t mid = newSize / 2;
  Poly A_low(A_resized.begin(), A_resized.begin() + mid);
  Poly A_high(A_resized.begin() + mid, A_resized.end());
  Poly B_low(B_resized.begin(), B_resized.begin() + mid);
  Poly B_high(B_resized.begin() + mid, B_resized.end());

  Poly low, high, middle;

  std::thread t1([&]() { low = karatsubaMultiply(A_low, B_low); });
  std::thread t2([&]() { high = karatsubaMultiply(A_high, B_high); });

  Poly sumA = add(A_low, A_high);
  Poly sumB = add(B_low, B_high);
  middle = karatsubaMultiply(sumA, sumB);

  t1.join();
  t2.join();

  middle = sub(middle, low);
  middle = sub(middle, high);

  Poly result(2 * newSize, 0);
  for (size_t i = 0; i < low.size(); i++)
    result[i] += low[i];
  for (size_t i = 0; i < middle.size(); i++)
    result[i + mid] += middle[i];
  for (size_t i = 0; i < high.size(); i++)
    result[i + 2 * mid] += high[i];

  result.resize(n + m - 1);
  return result;
}

void testPolyMultiplication() {
  Poly A = {3, 2, 5}; // 3 + 2x + 5x^2
  Poly B = {5, 1, 2}; // 5 + x + 2x^2

  std::cout << "Naive Multiplication:" << std::endl;
  auto result_naive = naiveMultiply(A, B);
  print(result_naive);

  std::cout << "Karatsuba Multiplication (Parallel):" << std::endl;
  auto result_karatsuba = karatsubaMultiply(A, B);
  print(result_karatsuba);
}

template <typename Func>
double benchmark(Func func, const Poly &A, const Poly &B, int iterations = 10) {
  double totalTime = 0.0;
  for (int i = 0; i < iterations; i++) {
    auto start = std::chrono::high_resolution_clock::now();
    func(A, B); // Call the function
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> duration = end - start;
    totalTime += duration.count();
  }
  return totalTime / iterations; 
}

void runBenchmarks() {
  Poly polyA(4000, 2);
  Poly polyB(4000, 2);

  double naiveTime = benchmark(naiveMultiply, polyA, polyB);
  std::cout << "Naive Multiply: " << naiveTime << " seconds\n";

  double karatsubaTime = benchmark(karatsubaMultiply, polyA, polyB);
  std::cout << "Karatsuba Multiply (Parallel): " << karatsubaTime
            << " seconds\n";
}

int main() {
  runBenchmarks();
  return 0;
}
