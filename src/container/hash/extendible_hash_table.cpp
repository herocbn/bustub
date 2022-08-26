//===----------------------------------------------------------------------===//
//
//                         BusTub
//
// extendible_hash_table.cpp
//
// Identification: src/container/hash/extendible_hash_table.cpp
//
// Copyright (c) 2015-2021, Carnegie Mellon University Database Group
//
//===----------------------------------------------------------------------===//

#include <iostream>
#include <string>
#include <utility>
#include <vector>

#include "common/exception.h"
#include "common/logger.h"
#include "common/rid.h"
#include "container/hash/extendible_hash_table.h"

namespace bustub {

template <typename KeyType, typename ValueType, typename KeyComparator>
HASH_TABLE_TYPE::ExtendibleHashTable(const std::string &name, BufferPoolManager *buffer_pool_manager,
                                     const KeyComparator &comparator, HashFunction<KeyType> hash_fn)
    : buffer_pool_manager_(buffer_pool_manager), comparator_(comparator), hash_fn_(std::move(hash_fn)) {
  //  implement me!
  auto directory_page = reinterpret_cast<HashTableDirectoryPage *>(buffer_pool_manager_->NewPage(&directory_page_id_));
  page_id_t bucket_page_id;
  buffer_pool_manager_->NewPage(&bucket_page_id);
  directory_page->SetPageId(directory_page_id_);
  directory_page->SetBucketPageId(0, bucket_page_id);
  buffer_pool_manager_->UnpinPage(directory_page_id_, true);
  buffer_pool_manager_->UnpinPage(bucket_page_id, true);
}

/*****************************************************************************
 * HELPERS
 *****************************************************************************/
/**
 * Hash - simple helper to downcast MurmurHash's 64-bit hash to 32-bit
 * for extendible hashing.
 *
 * @param key the key to hash
 * @return the downcasted 32-bit hash
 */
template <typename KeyType, typename ValueType, typename KeyComparator>
auto HASH_TABLE_TYPE::Hash(KeyType key) -> uint32_t {
  return static_cast<uint32_t>(hash_fn_.GetHash(key));
}

template <typename KeyType, typename ValueType, typename KeyComparator>
inline auto HASH_TABLE_TYPE::KeyToDirectoryIndex(KeyType key, HashTableDirectoryPage *dir_page) -> uint32_t {
  auto hash_key = Hash(key);
  auto global_mask = dir_page->GetGlobalDepthMask();
  auto mask = global_mask & hash_key;
  return mask;
}

template <typename KeyType, typename ValueType, typename KeyComparator>
inline auto HASH_TABLE_TYPE::KeyToPageId(KeyType key, HashTableDirectoryPage *dir_page) -> page_id_t {
  uint32_t idx = KeyToDirectoryIndex(key, dir_page);

  return dir_page->GetBucketPageId(idx);
}

template <typename KeyType, typename ValueType, typename KeyComparator>
auto HASH_TABLE_TYPE::FetchDirectoryPage() -> HashTableDirectoryPage * {
  Page *pg = buffer_pool_manager_->FetchPage(directory_page_id_);
  return reinterpret_cast<HashTableDirectoryPage *>(pg);
}

template <typename KeyType, typename ValueType, typename KeyComparator>
auto HASH_TABLE_TYPE::FetchBucketPage(page_id_t bucket_page_id) -> HASH_TABLE_BUCKET_TYPE * {
  auto pg = buffer_pool_manager_->FetchPage(bucket_page_id);
  return reinterpret_cast<HASH_TABLE_BUCKET_TYPE *>(pg);
}

/*****************************************************************************
 * SEARCH
 *****************************************************************************/
template <typename KeyType, typename ValueType, typename KeyComparator>
auto HASH_TABLE_TYPE::GetValue(Transaction *transaction, const KeyType &key, std::vector<ValueType> *result) -> bool {
  auto directory_page = FetchDirectoryPage();
  table_latch_.RLock();
  auto bucket_page_id = KeyToPageId(key, directory_page);
  auto bucket_page = FetchBucketPage(bucket_page_id);
  Page *pg = reinterpret_cast<Page *>(bucket_page);
  pg->RLatch();
  auto it = bucket_page->GetValue(key, comparator_, result);
  pg->RUnlatch();
  table_latch_.RUnlock();
  assert(buffer_pool_manager_->UnpinPage(bucket_page_id, false));
  assert(buffer_pool_manager_->UnpinPage(directory_page_id_, false));
  return it;
}

/*****************************************************************************
 * INSERTION
 *****************************************************************************/
template <typename KeyType, typename ValueType, typename KeyComparator>
auto HASH_TABLE_TYPE::Insert(Transaction *transaction, const KeyType &key, const ValueType &value) -> bool {
  auto directory_page = FetchDirectoryPage();
  table_latch_.RLock();
  uint32_t bucket_idx = KeyToDirectoryIndex(key, directory_page);
  page_id_t bucket_page_id = directory_page->GetBucketPageId(bucket_idx);
  auto bucket_page = FetchBucketPage(bucket_page_id);
  Page *pg = reinterpret_cast<Page *>(bucket_page);
  pg->WLatch();
  //-------------------------
  if (bucket_page->IsFull()) {
    // target bucket full
    pg->WUnlatch();
    table_latch_.RUnlock();
    assert(buffer_pool_manager_->UnpinPage(bucket_page_id, true));
    assert(buffer_pool_manager_->UnpinPage(directory_page_id_, true));
    return SplitInsert(transaction, key, value);
  }
  auto it = bucket_page->Insert(key, value, comparator_);
  pg->WUnlatch();
  table_latch_.RUnlock();
  assert(buffer_pool_manager_->UnpinPage(bucket_page_id, true));
  assert(buffer_pool_manager_->UnpinPage(directory_page_id_, true));
  return it;
}

template <typename KeyType, typename ValueType, typename KeyComparator>
auto HASH_TABLE_TYPE::SplitInsert(Transaction *transaction, const KeyType &key, const ValueType &value) -> bool {
  auto directory_page = FetchDirectoryPage();
  while (true) {
    table_latch_.WLock();
    uint32_t bucket_idx = KeyToDirectoryIndex(key, directory_page);
    page_id_t bucket_page_id = directory_page->GetBucketPageId(bucket_idx);
    auto bucket_page = FetchBucketPage(bucket_page_id);

    if (bucket_page->IsFull()) {
      page_id_t split_page_id;
      auto split_index = directory_page->GetSplitImageIndex(bucket_idx);
      auto split_page = reinterpret_cast<HASH_TABLE_BUCKET_TYPE *>(buffer_pool_manager_->NewPage(&split_page_id));
      auto global_depth = directory_page->GetGlobalDepth();
      auto local_depth = directory_page->GetLocalDepth(bucket_idx);

      if (global_depth == local_depth) {
        uint32_t nums = 1 << global_depth;
        for (uint32_t i = 0; i < nums; i++) {
          auto id = directory_page->GetBucketPageId(i);
          auto depth = directory_page->GetLocalDepth(i);
          directory_page->SetBucketPageId(i + nums, id);
          directory_page->SetLocalDepth(i + nums, depth);
        }
        directory_page->IncrGlobalDepth();
        directory_page->SetBucketPageId(split_index, split_page_id);
        directory_page->SetLocalDepth(bucket_idx, directory_page->GetGlobalDepth());
        directory_page->SetLocalDepth(split_index, directory_page->GetGlobalDepth());
      } else {
        // just split the bucket
        local_depth++;
        uint32_t nums = 1 << global_depth;
        uint32_t local_nums = 1 << local_depth;
        for (uint32_t i = split_index; i < nums; i += local_nums) {
          directory_page->SetBucketPageId(i, split_page_id);
          directory_page->SetLocalDepth(i, local_depth);
        }
        for (int i = split_index; i >= 0; i -= local_nums) {
          directory_page->SetBucketPageId(i, split_page_id);
          directory_page->SetLocalDepth(i, local_depth);
        }
        for (uint32_t i = bucket_idx; i < nums; i += local_nums) {
          directory_page->SetBucketPageId(i, bucket_page_id);
          directory_page->SetLocalDepth(i, local_depth);
        }
        for (int i = bucket_idx; i >= 0; i -= local_nums) {
          directory_page->SetBucketPageId(i, bucket_page_id);
          directory_page->SetLocalDepth(i, local_depth);
        }
      }

      uint32_t array_size = BUCKET_ARRAY_SIZE;
      for (uint32_t i = 0; i < array_size; i++) {
        auto k = bucket_page->KeyAt(i);
        auto v = bucket_page->ValueAt(i);
        bucket_page->RemoveAt(i);
        if ((KeyToPageId(k, directory_page) == bucket_page_id)) {
          bucket_page->Insert(k, v, comparator_);
        } else {
          split_page->Insert(k, v, comparator_);
        }
      }

      table_latch_.WUnlock();
      assert(buffer_pool_manager_->UnpinPage(bucket_page_id, true));
      assert(buffer_pool_manager_->UnpinPage(split_page_id, true));
    } else {
      table_latch_.WUnlock();
      Page *pg = reinterpret_cast<Page *>(bucket_page);
      pg->WLatch();
      auto it = bucket_page->Insert(key, value, comparator_);
      pg->WUnlatch();
      assert(buffer_pool_manager_->UnpinPage(bucket_page_id, true));
      assert(buffer_pool_manager_->UnpinPage(directory_page_id_, true));
      return it;
    }
  }
  return false;
}

/*****************************************************************************
 * REMOVE
 *****************************************************************************/
template <typename KeyType, typename ValueType, typename KeyComparator>
auto HASH_TABLE_TYPE::Remove(Transaction *transaction, const KeyType &key, const ValueType &value) -> bool {
  auto directory_page = FetchDirectoryPage();
  table_latch_.RLock();
  uint32_t bucket_idx = KeyToDirectoryIndex(key, directory_page);
  page_id_t bucket_page_id = directory_page->GetBucketPageId(bucket_idx);
  auto bucket_page = FetchBucketPage(bucket_page_id);
  Page *pg_1 = reinterpret_cast<Page *>(bucket_page);
  pg_1->WLatch();
  auto it = bucket_page->Remove(key, value, comparator_);
  pg_1->WUnlatch();
  if (bucket_page->IsEmpty() && (directory_page->GetLocalDepth(bucket_idx) != 0)) {
    // merge
    table_latch_.RUnlock();
    Merge(transaction, key, value);
  } else {
    table_latch_.RUnlock();
  }
  assert(buffer_pool_manager_->UnpinPage(bucket_page_id, true));
  assert(buffer_pool_manager_->UnpinPage(directory_page_id_, true));
  return it;
}

/*****************************************************************************
 * MERGE
 *****************************************************************************/
template <typename KeyType, typename ValueType, typename KeyComparator>
void HASH_TABLE_TYPE::Merge(Transaction *transaction, const KeyType &key, const ValueType &value) {
  auto directory_page = FetchDirectoryPage();
  table_latch_.WLock();
  uint32_t bucket_idx = KeyToDirectoryIndex(key, directory_page);
  auto depth_1 = directory_page->GetLocalDepth(bucket_idx);
  if (depth_1 > 0) {
    uint32_t global_depth = directory_page->GetGlobalDepth();
    uint32_t merge_idx = bucket_idx ^ (1 << (global_depth - 1));
    auto merge_page_id = directory_page->GetBucketPageId(merge_idx);
    auto depth_2 = directory_page->GetLocalDepth(merge_idx);
    if (depth_2 == depth_1 && (depth_2 == global_depth)) {
      if (bucket_idx < merge_idx) {
        directory_page->SetBucketPageId(bucket_idx, merge_page_id);
        directory_page->DecrLocalDepth(bucket_idx);
        directory_page->DecrLocalDepth(merge_idx);
      } else {
        directory_page->SetBucketPageId(merge_idx, merge_page_id);
        directory_page->DecrLocalDepth(merge_idx);
        directory_page->DecrLocalDepth(bucket_idx);
      }
      if (directory_page->CanShrink()) {
        directory_page->DecrGlobalDepth();
      }
    }
  }
  table_latch_.WUnlock();
  assert(buffer_pool_manager_->UnpinPage(directory_page_id_, true));
}

/*****************************************************************************
 * GETGLOBALDEPTH - DO NOT TOUCH
 *****************************************************************************/
template <typename KeyType, typename ValueType, typename KeyComparator>
auto HASH_TABLE_TYPE::GetGlobalDepth() -> uint32_t {
  table_latch_.RLock();
  HashTableDirectoryPage *dir_page = FetchDirectoryPage();
  uint32_t global_depth = dir_page->GetGlobalDepth();
  assert(buffer_pool_manager_->UnpinPage(directory_page_id_, false, nullptr));
  table_latch_.RUnlock();
  return global_depth;
}

/*****************************************************************************
 * VERIFY INTEGRITY - DO NOT TOUCH
 *****************************************************************************/
template <typename KeyType, typename ValueType, typename KeyComparator>
void HASH_TABLE_TYPE::VerifyIntegrity() {
  table_latch_.RLock();
  HashTableDirectoryPage *dir_page = FetchDirectoryPage();
  dir_page->VerifyIntegrity();
  assert(buffer_pool_manager_->UnpinPage(directory_page_id_, false, nullptr));
  table_latch_.RUnlock();
}

/*****************************************************************************
 * TEMPLATE DEFINITIONS - DO NOT TOUCH
 *****************************************************************************/
template class ExtendibleHashTable<int, int, IntComparator>;

template class ExtendibleHashTable<GenericKey<4>, RID, GenericComparator<4>>;
template class ExtendibleHashTable<GenericKey<8>, RID, GenericComparator<8>>;
template class ExtendibleHashTable<GenericKey<16>, RID, GenericComparator<16>>;
template class ExtendibleHashTable<GenericKey<32>, RID, GenericComparator<32>>;
template class ExtendibleHashTable<GenericKey<64>, RID, GenericComparator<64>>;

}  // namespace bustub
