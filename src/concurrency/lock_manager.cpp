//===----------------------------------------------------------------------===//
//
//                         BusTub
//
// lock_manager.cpp
//
// Identification: src/concurrency/lock_manager.cpp
//
// Copyright (c) 2015-2019, Carnegie Mellon University Database Group
//
//===----------------------------------------------------------------------===//

#include "concurrency/lock_manager.h"

#include <utility>
#include <vector>

namespace bustub {

auto LockManager::LockShared(Transaction *txn, const RID &rid) -> bool {
  // 这里的请求是：txn需要rid进行读取
  // 将该请求加入队列
  // 接着扫描该rid的队列，如果当前没有写锁则返回成功，否则等待
  if (txn->GetState() == TransactionState::SHRINKING) {
    txn->SetState(TransactionState::ABORTED);
    return false;
  }
  if (txn->IsSharedLocked(rid) || txn->IsExclusiveLocked(rid)) {
    return true;
  }
  if (txn->GetIsolationLevel() == IsolationLevel::READ_UNCOMMITTED) {
    txn->SetState(TransactionState::ABORTED);
    return false;
  }

  auto lockmode = LockMode::SHARED;
  auto txn_id = txn->GetTransactionId();
  auto lock_req = LockRequest(txn, txn_id, lockmode);
  std::unique_lock<std::mutex> guard(latch_);
  auto iter = lock_table_.find(rid);
  if (iter == lock_table_.end()) {
    lock_table_.emplace(std::piecewise_construct, std::forward_as_tuple(rid), std::forward_as_tuple());
    LockRequestQueue *lockque = &(lock_table_[rid]);
    lockque->request_queue_.emplace_back(lock_req);
    txn->GetSharedLockSet()->emplace(rid);
    return true;
  }
  auto val = &(iter->second);
  std::list<LockRequest> *lock_list = &(val->request_queue_);
  lock_list->emplace_back(lock_req);
  for (auto it = lock_list->begin(); it != lock_list->end();) {
    if (it->lock_mode_ == LockMode::EXCLUSIVE) {
      if (it->txn_id_ > txn_id) {
        // 老年人不能等年轻人，杀掉所有年轻的锁
        it->txn_->SetState(TransactionState::ABORTED);
        it = lock_list->erase(it);
        val->cv_.notify_all();
      } else {
        ++it;
      }
    } else {
      ++it;
    }
  }
  for (auto it = lock_list->begin(); it != lock_list->end();) {
    if (it->lock_mode_ == LockMode::EXCLUSIVE) {
      val->cv_.wait(guard);
      it = lock_list->begin();
    } else {
      it++;
    }
  }
  if (txn->GetState() == TransactionState::ABORTED) {
    return false;
  }
  txn->GetSharedLockSet()->emplace(rid);
  guard.unlock();
  return true;
}

auto LockManager::LockExclusive(Transaction *txn, const RID &rid) -> bool {
  // 这里的请求是：txn需要rid进行写入
  // 将该请求加入队列
  // 接着扫描该rid的队列，如果当前没有读和写锁则返回成功，否则等待
  // Wound-wait strategy: 为了防止死锁，采用年轻人等老年人的方案（ti>tj）
  if (txn->GetState() == TransactionState::SHRINKING) {
    txn->SetState(TransactionState::ABORTED);
    return false;
  }
  if (txn->IsExclusiveLocked(rid)) {
    return true;
  }
  auto lockmode = LockMode::EXCLUSIVE;
  auto txn_id = txn->GetTransactionId();
  auto lock_req = LockRequest(txn, txn_id, lockmode);
  std::unique_lock<std::mutex> guard(latch_);
  auto iter = lock_table_.find(rid);
  if (iter == lock_table_.end()) {
    // 之前在官网上查看要使用以下方法进行插入，后来才知道可以直接使用索引赋值，泪目~
    lock_table_.emplace(std::piecewise_construct, std::forward_as_tuple(rid), std::forward_as_tuple());
    LockRequestQueue *lockque = &(lock_table_[rid]);
    lockque->request_queue_.emplace_back(lock_req);
    txn->GetExclusiveLockSet()->emplace(rid);
    return true;
  }
  auto val = &(iter->second);
  std::list<LockRequest> *lock_list = &(val->request_queue_);
  lock_list->emplace_back(lock_req);
  // 扫描lock_list，如果有txn的线程号大于当前txn的线程号的，读或者写都要中止,最后插入并等待
  for (auto it = lock_list->begin(); it != lock_list->end();) {
    if (it->txn_id_ > txn_id) {
      // 老年人不能等年轻人，杀掉所有年轻的锁
      it->txn_->SetState(TransactionState::ABORTED);
      it = lock_list->erase(it);
      val->cv_.notify_all();
    } else {
      ++it;
    }
  }
  while (txn->GetState() != TransactionState::ABORTED && lock_list->size() != 1) {
    val->cv_.wait(guard);
  }
  if (txn->GetState() == TransactionState::ABORTED) {
    return false;
  }
  txn->GetExclusiveLockSet()->emplace(rid);
  guard.unlock();
  return true;
}

auto LockManager::LockUpgrade(Transaction *txn, const RID &rid) -> bool {
  if (txn->GetState() == TransactionState::SHRINKING) {
    assert(false);
    txn->SetState(TransactionState::ABORTED);
    return false;
  }
  if (!txn->IsSharedLocked(rid)) {
    assert(false);
    txn->SetState(TransactionState::ABORTED);
    return false;
  }
  std::unique_lock<std::mutex> guard(latch_);
  auto iter = lock_table_.find(rid);
  auto val = &(iter->second);
  if (val->upgrading_ != INVALID_TXN_ID) {
    assert(false);
    txn->SetState(TransactionState::ABORTED);
    return false;
  }
  if (iter == lock_table_.end()) {
    assert(false);
    return false;
  }
  auto txn_id = txn->GetTransactionId();
  std::list<LockRequest> *lock_list = &(val->request_queue_);
  val->upgrading_ = txn_id;
  // kill all lower-request
  auto lock_iter = lock_list->begin();
  while (lock_iter != lock_list->end()) {
    auto old_id = lock_iter->txn_id_;
    auto old_txn = lock_iter->txn_;
    if (old_id > txn_id) {
      old_txn->SetState(TransactionState::ABORTED);
      lock_iter = lock_list->erase(lock_iter);
      val->cv_.notify_all();
    } else {
      lock_iter++;
    }
  }
  while (txn->GetState() != TransactionState::ABORTED && lock_list->size() != 1) {
    val->cv_.wait(guard);
  }
  val->upgrading_ = INVALID_TXN_ID;
  if (txn->GetState() == TransactionState::ABORTED) {
    return false;
  }
  auto request_location = lock_table_[rid].request_queue_.begin();
  assert(request_location->txn_id_ == txn_id);  // 队列第一位即该更新请求，此时没有中止事务持有锁
  request_location->lock_mode_ = LockMode::EXCLUSIVE;
  txn->GetSharedLockSet()->erase(rid);
  txn->GetExclusiveLockSet()->emplace(rid);
  return true;
}

auto LockManager::Unlock(Transaction *txn, const RID &rid) -> bool {
  std::unique_lock<std::mutex> guard(latch_);
  if (txn->GetIsolationLevel() == IsolationLevel::REPEATABLE_READ && txn->GetState() == TransactionState::GROWING) {
    txn->SetState(TransactionState::SHRINKING);
  }
  auto target = txn->GetTransactionId();
  // 从队列中删除rid的锁
  auto iter = lock_table_.find(rid);
  if (iter == lock_table_.end()) {
    return false;
  }
  auto val = &(iter->second);
  std::list<LockRequest> *lock_list = &(val->request_queue_);
  auto pos = lock_list->begin();
  for (auto it = lock_list->begin(); it != lock_list->end();) {
    if (it->txn_id_ == target) {
      if (it->lock_mode_ == LockMode::SHARED) {
        pos = it;
        txn->GetSharedLockSet()->erase(rid);
      } else {
        pos = it;
        txn->GetExclusiveLockSet()->erase(rid);
      }
      lock_list->erase(pos);
      break;
    }
    it++;
  }
  val->cv_.notify_all();
  guard.unlock();
  return true;
}
}  // namespace bustub
