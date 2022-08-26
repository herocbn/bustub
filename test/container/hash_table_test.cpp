//===----------------------------------------------------------------------===//
//
//                         BusTub
//
// hash_table_test.cpp
//
// Identification: test/container/hash_table_test.cpp
//
// Copyright (c) 2015-2021, Carnegie Mellon University Database Group
//
//===----------------------------------------------------------------------===//

#include <thread>  // NOLINT
#include <vector>

#include "buffer/buffer_pool_manager_instance.h"
#include "common/logger.h"
#include "container/hash/extendible_hash_table.h"
#include "gtest/gtest.h"
#include "murmur3/MurmurHash3.h"

namespace bustub {

// NOLINTNEXTLINE

// NOLINTNEXTLINE
TEST(HashTableTest, SampleTest) {
  auto *disk_manager = new DiskManager("test.db");
  auto *bpm = new BufferPoolManagerInstance(50, disk_manager);
  ExtendibleHashTable<int, int, IntComparator> ht("blah", bpm, IntComparator(), HashFunction<int>());

  // insert a few values
  for (int i = 0; i < 5; i++) {
    ht.Insert(nullptr, i, i);
    std::vector<int> res;
    ht.GetValue(nullptr, i, &res);
    EXPECT_EQ(1, res.size()) << "Failed to insert " << i << std::endl;
    EXPECT_EQ(i, res[0]);
  }

  ht.VerifyIntegrity();

  // check if the inserted values are all there
  for (int i = 0; i < 5; i++) {
    std::vector<int> res;
    ht.GetValue(nullptr, i, &res);
    EXPECT_EQ(1, res.size()) << "Failed to keep " << i << std::endl;
    EXPECT_EQ(i, res[0]);
  }

  ht.VerifyIntegrity();

  // insert one more value for each key
  for (int i = 0; i < 5; i++) {
    if (i == 0) {
      // duplicate values for the same key are not allowed
      EXPECT_FALSE(ht.Insert(nullptr, i, 2 * i));
    } else {
      EXPECT_TRUE(ht.Insert(nullptr, i, 2 * i));
    }
    ht.Insert(nullptr, i, 2 * i);
    std::vector<int> res;
    ht.GetValue(nullptr, i, &res);
    if (i == 0) {
      // duplicate values for the same key are not allowed
      EXPECT_EQ(1, res.size());
      EXPECT_EQ(i, res[0]);
    } else {
      EXPECT_EQ(2, res.size());
      if (res[0] == i) {
        EXPECT_EQ(2 * i, res[1]);
      } else {
        EXPECT_EQ(2 * i, res[0]);
        EXPECT_EQ(i, res[1]);
      }
    }
  }

  ht.VerifyIntegrity();

  // look for a key that does not exist
  std::vector<int> res;
  ht.GetValue(nullptr, 20, &res);
  EXPECT_EQ(0, res.size());

  // delete some values
  for (int i = 0; i < 5; i++) {
    EXPECT_TRUE(ht.Remove(nullptr, i, i));
    std::vector<int> res;
    ht.GetValue(nullptr, i, &res);
    if (i == 0) {
      // (0, 0) is the only pair with key 0
      EXPECT_EQ(0, res.size());
    } else {
      EXPECT_EQ(1, res.size());
      EXPECT_EQ(2 * i, res[0]);
    }
  }

  ht.VerifyIntegrity();

  // delete all values
  for (int i = 0; i < 5; i++) {
    if (i == 0) {
      // (0, 0) has been deleted
      EXPECT_FALSE(ht.Remove(nullptr, i, 2 * i));
    } else {
      EXPECT_TRUE(ht.Remove(nullptr, i, 2 * i));
    }
  }

  ht.VerifyIntegrity();

  disk_manager->ShutDown();
  remove("test.db");
  delete disk_manager;
  delete bpm;
}
TEST(HashTablePageTest, HashTablePageIntegratedTest) {
  size_t buffer_pool_size = 3;
  size_t hash_table_size = 500;
  auto *disk_manager = new DiskManager("test.db");
  auto *bpm = new BufferPoolManagerInstance(buffer_pool_size, disk_manager);

  // setup: one directory page and two bucket pages
  page_id_t directory_page_id = INVALID_PAGE_ID;
  auto directory_page =
      reinterpret_cast<HashTableDirectoryPage *>(bpm->NewPage(&directory_page_id, nullptr)->GetData());

  page_id_t bucket_page_id_1 = INVALID_PAGE_ID;
  page_id_t bucket_page_id_2 = INVALID_PAGE_ID;

  directory_page->IncrGlobalDepth();

  auto bucket_page_1 = reinterpret_cast<HashTableBucketPage<int, int, IntComparator> *>(
      bpm->NewPage(&bucket_page_id_1, nullptr)->GetData());
  directory_page->SetLocalDepth(0, 0);

  auto bucket_page_2 = reinterpret_cast<HashTableBucketPage<int, int, IntComparator> *>(
      bpm->NewPage(&bucket_page_id_2, nullptr)->GetData());

  directory_page->SetLocalDepth(0, 1);
  directory_page->SetLocalDepth(1, 1);
  directory_page->SetBucketPageId(0, bucket_page_id_1);
  directory_page->SetBucketPageId(1, bucket_page_id_2);

  // add to first bucket page until it is full
  size_t pairs_total_page_1 = hash_table_size / 2;
  for (unsigned i = 0; i < pairs_total_page_1; i++) {
    assert(bucket_page_1->Insert(i, i, IntComparator()));
  }

  // add to second bucket page until it is full
  size_t pairs_total_page_2 = hash_table_size / 2;
  for (unsigned i = 0; i < pairs_total_page_2; i++) {
    assert(bucket_page_2->Insert(i, i, IntComparator()));
  }

  // remove every other pair
  for (unsigned i = 0; i < pairs_total_page_1; i++) {
    if (i % 2 == 1) {
      bucket_page_1->Remove(i, i, IntComparator());
    }
  }

  for (unsigned i = 0; i < pairs_total_page_2; i++) {
    if (i % 2 == 1) {
      bucket_page_2->Remove(i, i, IntComparator());
    }
  }

  // check for the flags
  for (unsigned i = 0; i < pairs_total_page_1 + pairs_total_page_1 / 2; i++) {
    if (i < pairs_total_page_1) {
      EXPECT_TRUE(bucket_page_1->IsOccupied(i));
      if (i % 2 == 1) {
        EXPECT_FALSE(bucket_page_1->IsReadable(i));
      } else {
        EXPECT_TRUE(bucket_page_1->IsReadable(i));
      }
    } else {
      EXPECT_FALSE(bucket_page_1->IsOccupied(i));
    }
  }

  for (unsigned i = 0; i < pairs_total_page_2 + pairs_total_page_2 / 2; i++) {
    if (i < pairs_total_page_2) {
      EXPECT_TRUE(bucket_page_2->IsOccupied(i));
      if (i % 2 == 1) {
        EXPECT_FALSE(bucket_page_2->IsReadable(i));
      } else {
        EXPECT_TRUE(bucket_page_2->IsReadable(i));
      }
    } else {
      EXPECT_FALSE(bucket_page_2->IsOccupied(i));
    }
  }

  bpm->UnpinPage(bucket_page_id_1, true, nullptr);
  bpm->UnpinPage(bucket_page_id_2, true, nullptr);
  bpm->UnpinPage(directory_page_id, true, nullptr);

  disk_manager->ShutDown();
  remove("test.db");
  delete disk_manager;
  delete bpm;
}

}  // namespace bustub
