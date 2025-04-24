#include <algorithm>
#include <cassert>
#include <cstdint>
#include <iostream>
#include <list>
#include <memory>
#include <vector>

namespace {

using Number = std::uint64_t;

class Subset {
 public:
  Subset() : numbers_{}, sum_{} {}
  explicit Subset(const Number number) : numbers_{number}, sum_{number} {}

  Subset(Subset&&) = default;
  Subset& operator=(Subset&&) = default;

  const std::list<Number>& numbers() const { return numbers_; }

  Number sum() const { return sum_; }

  void Merge(Subset other) {
    numbers_.splice(numbers_.end(), other.numbers_);
    sum_ += other.sum_;
  }

 private:
  Subset(const Subset&) = delete;
  Subset& operator=(const Subset&) = delete;

  std::list<Number> numbers_;
  Number sum_;
};

std::ostream& operator<<(std::ostream& stream, const Subset& subset) {
  stream << '[';
  if (!subset.numbers().empty()) {
    auto it{subset.numbers().begin()};
    stream << *it;
    for (++it; it != subset.numbers().end(); ++it) {
      stream << ',' << *it;
    }
  }
  stream << ']';
  return stream;
}

struct SubsetSumGreater {
  bool operator()(const Subset& a, const Subset& b) const {
    return a.sum() > b.sum();
  }
};

class Partition {
 public:
  Partition(const Number number, const std::size_t k) : subsets_(k) {
    assert(k > 0);
    subsets_.front().Merge(Subset{number});
  }

  Partition(Partition&&) = default;
  Partition& operator=(Partition&&) = default;

  const std::vector<Subset>& subsets() const { return subsets_; }

  Number difference() const {
    return subsets_.front().sum() - subsets_.back().sum();
  }

  void Merge(Partition other) {
    assert(subsets_.size() == other.subsets_.size());
    auto it{subsets_.begin()};
    auto other_it{other.subsets_.rbegin()};
    while (it != subsets_.end() && other_it != other.subsets_.rend()) {
      it->Merge(std::move(*other_it));
      ++it;
      ++other_it;
    }
    std::sort(subsets_.begin(), subsets_.end(), SubsetSumGreater{});
  }

 private:
  Partition(const Partition&) = delete;
  Partition& operator=(const Partition&) = delete;

  std::vector<Subset> subsets_;
};

std::ostream& operator<<(std::ostream& stream, const Partition& partition) {
//   stream << '[';
//   if (!partition.subsets().empty()) {
//     auto it{partition.subsets().begin()};
//     stream << *it;
//     for (++it; it != partition.subsets().end(); ++it) {
//       stream << ',' << *it;
//     }
//   }
//   stream << ']';
//   return stream;
    // For each partition, print the sum
    stream << partition.subsets().size() << " partitions: ";
    stream << '[';
    for (const auto& subset : partition.subsets()) {
        stream << subset.sum() << "],[";
    }
    stream << "]";
    return stream;
}

struct DifferenceLess {
  bool operator()(const Partition& a, const Partition& b) const {
    return a.difference() < b.difference();
  }
};

Partition KarmarkarKarp(const std::vector<Number>& numbers,
                        const std::size_t k) {
  assert(!numbers.empty());
  assert(k > 0);
  std::vector<Partition> heap;
  heap.reserve(numbers.size());
  for (const Number number : numbers) {
    heap.emplace_back(number, k);
  }
  std::make_heap(heap.begin(), heap.end(), DifferenceLess{});
  while (heap.size() > 1) {
    std::pop_heap(heap.begin(), heap.end(), DifferenceLess{});
    Partition partition{std::move(heap.back())};
    heap.pop_back();
    std::pop_heap(heap.begin(), heap.end(), DifferenceLess{});
    heap.back().Merge(std::move(partition));
    std::push_heap(heap.begin(), heap.end(), DifferenceLess{});
  }
  return std::move(heap.front());
}

}  // namespace

int main() {
    std::vector<std::vector<Number>> test_cases;
    // Pick a few sizes of numbers, then for each of them, pick random amounts of values of 1, 2, and 4
    std::vector<size_t> sizes = {8, 16, 32, 64, 128, 256};
    std::vector<Number> numbers = {1, 2, 4};
    std::cout << "Building test cases...\n";
    for (size_t size : sizes) {
        std::cout << "Generating test cases for size " << size << '\n';
        std::vector<Number> test_case(size);
        for (size_t i = 0; i < size; ++i) {
            test_case[i] = numbers[i % 3];
        }
        test_cases.push_back(test_case);
        std::cout << "Generating corner case 1" << '\n';
        test_case.clear();
        // Now add a few corner test cases for each size, size as LOTS of "4" and just a couple of "1" and "2"
        for (size_t j = 0; j < size-2; ++j) {
            test_case.push_back(4);
        }
        test_case.push_back(1);
        test_case.push_back(2);
        test_cases.push_back(test_case);
        // Now add a few corner test cases for each size, size as LOTS of "1" and just a couple of "2" and "4"
        std::cout << "Generating corner case 2" << '\n';
        test_case.clear();
        for (size_t j = 0; j < size; ++j) {
            test_case.push_back(1);
        }
        test_case.push_back(2);
        test_case.push_back(4);
        test_cases.push_back(test_case);
    }

    std::cout << "Test cases built.\n";
    std::vector<size_t> gpu_counts = {1, 2, 3, 4, 5, 6};
    for (const auto& gpu_count : gpu_counts) {
        std::cout << "GPU count: " << gpu_count << '\n';
        std::cout << "Test cases:\n";
        for (const auto& test_case : test_cases) {
            std::cout << "\tTest case: ";
            for (const auto& number : test_case) {
                std::cout << number << ' ';
            }
            std::cout << "\n\tResult: " << KarmarkarKarp(test_case, gpu_count) << '\n';
        }
    }
}