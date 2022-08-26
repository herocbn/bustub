//===----------------------------------------------------------------------===//
//
//                         BusTub
//
// lru_replacer.cpp
//
// Identification: src/buffer/lru_replacer.cpp
//
// Copyright (c) 2015-2019, Carnegie Mellon University base Group
//
//===----------------------------------------------------------------------===//

#include "buffer/lru_replacer.h"

namespace bustub {

LRUReplacer::LRUReplacer(size_t num_pages) { size_ = num_pages; }

LRUReplacer::~LRUReplacer() = default;

auto LRUReplacer::Victim(frame_id_t *frame_id) -> bool {
  if (data_.empty()) {
    return false;
  }
  std::lock_guard<std::mutex> lock(lock_);
  *frame_id = data_.back();
  maps_.erase(data_.back());
  data_.pop_back();

  return true;
}

void LRUReplacer::Pin(frame_id_t frame_id) {
  std::lock_guard<std::mutex> lock(lock_);
  // data_.remove_if([=](frame_id_t it) { return frame_id == it; });
  if (maps_.count(frame_id) == 0U) {
    return;
  }
  auto iter = maps_[frame_id];
  maps_.erase(frame_id);
  data_.erase(iter);
}

void LRUReplacer::Unpin(frame_id_t frame_id) {
  std::lock_guard<std::mutex> lock(lock_);
  if (maps_.count(frame_id) != 0U) {
    return;
  }
  data_.emplace_front(frame_id);
  maps_.insert({frame_id, data_.begin()});
}

auto LRUReplacer::Size() -> size_t { return data_.size(); }

}  // namespace bustub
