//===----------------------------------------------------------------------===//
//
//                         BusTub
//
// hash_table_bucket_page.cpp
//
// Identification: src/storage/page/hash_table_bucket_page.cpp
//
// Copyright (c) 2015-2021, Carnegie Mellon University Database Group
//
//===----------------------------------------------------------------------===//
#include "storage/page/hash_table_bucket_page.h"
#include "common/logger.h"
#include "common/util/hash_util.h"
#include "storage/index/generic_key.h"
#include "storage/index/hash_comparator.h"
#include "storage/table/tmp_tuple.h"

namespace bustub {

template <typename KeyType, typename ValueType, typename KeyComparator>
auto HASH_TABLE_BUCKET_TYPE::GetValue(KeyType key, KeyComparator cmp, std::vector<ValueType> *result) -> bool {
  uint32_t array_size = BUCKET_ARRAY_SIZE;
  for (uint32_t i = 0; i < array_size; i++) {
    if (IsReadable(i)) {
      if (cmp(key, array_[i].first) == 0) {
        result->push_back(array_[i].second);
      }
    }
  }
  return (!result->empty());
}

template <typename KeyType, typename ValueType, typename KeyComparator>
auto HASH_TABLE_BUCKET_TYPE::Insert(KeyType key, ValueType value, KeyComparator cmp) -> bool {
  // array_ size condition
  uint32_t array_size = BUCKET_ARRAY_SIZE;
  uint32_t pos = array_size;
  for (uint32_t i = 0; i < array_size; i++) {
    if (IsReadable(i)) {
      if (cmp(key, array_[i].first) == 0 && (value == array_[i].second)) {
        return false;
      }
    } else {
      if (pos == array_size) {
        pos = i;
      }
      if (!IsOccupied(i)) {
        break;
      }
    }
  }
  if (pos == array_size) {
    return false;
  }
  array_[pos].first = key;
  array_[pos].second = value;

  SetReadable(pos);
  SetOccupied(pos);
  return true;
}

template <typename KeyType, typename ValueType, typename KeyComparator>
auto HASH_TABLE_BUCKET_TYPE::Remove(KeyType key, ValueType value, KeyComparator cmp) -> bool {
  uint32_t i = 0;
  uint32_t j = NumReadable();
  uint32_t array_size = BUCKET_ARRAY_SIZE;
  uint32_t cnt = 0;
  for (; i < array_size; i++) {
    if (IsReadable(i)) {
      if (cmp(key, array_[i].first) == 0 && (value == array_[i].second)) {
        break;
      }
      cnt++;
    }
    if (cnt >= j) {
      return false;
    }
  }
  ClearReadable(i);
  SetOccupied(i);
  return true;
}

template <typename KeyType, typename ValueType, typename KeyComparator>
auto HASH_TABLE_BUCKET_TYPE::KeyAt(uint32_t bucket_idx) const -> KeyType {
  if (IsReadable(bucket_idx)) {
    return array_[bucket_idx].first;
  }
  return {};
}

template <typename KeyType, typename ValueType, typename KeyComparator>
auto HASH_TABLE_BUCKET_TYPE::ValueAt(uint32_t bucket_idx) const -> ValueType {
  if (IsReadable(bucket_idx)) {
    return array_[bucket_idx].second;
  }
  return {};
}

template <typename KeyType, typename ValueType, typename KeyComparator>
void HASH_TABLE_BUCKET_TYPE::RemoveAt(uint32_t bucket_idx) {
  uint32_t num_index = bucket_idx / 8;
  uint32_t bit_index = bucket_idx % 8;
  readable_[num_index] &= ~(1 << bit_index);
}

template <typename KeyType, typename ValueType, typename KeyComparator>
auto HASH_TABLE_BUCKET_TYPE::IsOccupied(uint32_t bucket_idx) const -> bool {
  uint32_t num_index = bucket_idx / 8;
  uint32_t bit_index = bucket_idx % 8;
  uint32_t temp = (occupied_[num_index] >> bit_index) & 1;
  return temp == 1;
}

template <typename KeyType, typename ValueType, typename KeyComparator>
void HASH_TABLE_BUCKET_TYPE::SetOccupied(uint32_t bucket_idx) {
  uint32_t num_index = bucket_idx / 8;
  uint32_t bit_index = bucket_idx % 8;
  occupied_[num_index] |= (1 << bit_index);
}

template <typename KeyType, typename ValueType, typename KeyComparator>
auto HASH_TABLE_BUCKET_TYPE::IsReadable(uint32_t bucket_idx) const -> bool {
  uint32_t num_index = bucket_idx / 8;
  uint32_t bit_index = bucket_idx % 8;
  uint32_t temp = (readable_[num_index] >> bit_index) & 1;
  return (temp == 1);
}

template <typename KeyType, typename ValueType, typename KeyComparator>
void HASH_TABLE_BUCKET_TYPE::SetReadable(uint32_t bucket_idx) {
  uint32_t num_index = bucket_idx / 8;
  uint32_t bit_index = bucket_idx % 8;
  readable_[num_index] = readable_[num_index] | (1 << bit_index);
}
template <typename KeyType, typename ValueType, typename KeyComparator>
void HASH_TABLE_BUCKET_TYPE::ClearReadable(uint32_t bucket_idx) {
  uint32_t num_index = bucket_idx / 8;
  uint32_t bit_index = bucket_idx % 8;
  readable_[num_index] &= ~(1 << bit_index);
}

template <typename KeyType, typename ValueType, typename KeyComparator>
auto HASH_TABLE_BUCKET_TYPE::IsFull() -> bool {
  uint32_t len = BUCKET_ARRAY_SIZE;
  return len == NumReadable();
}

template <typename KeyType, typename ValueType, typename KeyComparator>
auto HASH_TABLE_BUCKET_TYPE::NumReadable() -> uint32_t {
  uint32_t j = (BUCKET_ARRAY_SIZE - 1) / 8 + 1;
  uint32_t cnt = 0;
  for (uint32_t i = 0; i < j; i++) {
    int n = readable_[i];
    while (n != 0) {
      cnt++;
      n &= (n - 1);
    }
  }
  return cnt;
}

template <typename KeyType, typename ValueType, typename KeyComparator>
auto HASH_TABLE_BUCKET_TYPE::IsEmpty() -> bool {
  return NumReadable() == 0;
}

template <typename KeyType, typename ValueType, typename KeyComparator>
void HASH_TABLE_BUCKET_TYPE::PrintBucket() {
  uint32_t size = 0;
  uint32_t taken = 0;
  uint32_t free = 0;
  for (size_t bucket_idx = 0; bucket_idx < BUCKET_ARRAY_SIZE; bucket_idx++) {
    if (!IsOccupied(bucket_idx)) {
      break;
    }

    size++;

    if (IsReadable(bucket_idx)) {
      taken++;
    } else {
      free++;
    }
  }

  LOG_INFO("Bucket Capacity: %lu, Size: %u, Taken: %u, Free: %u", BUCKET_ARRAY_SIZE, size, taken, free);
}

// DO NOT REMOVE ANYTHING BELOW THIS LINE
template class HashTableBucketPage<int, int, IntComparator>;

template class HashTableBucketPage<GenericKey<4>, RID, GenericComparator<4>>;
template class HashTableBucketPage<GenericKey<8>, RID, GenericComparator<8>>;
template class HashTableBucketPage<GenericKey<16>, RID, GenericComparator<16>>;
template class HashTableBucketPage<GenericKey<32>, RID, GenericComparator<32>>;
template class HashTableBucketPage<GenericKey<64>, RID, GenericComparator<64>>;

// template class HashTableBucketPage<hash_t, TmpTuple, HashComparator>;

}  // namespace bustub
