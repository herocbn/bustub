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
 if(txn->GetState()  == TransactionState::SHRINKING){
  
 }
	std::unique_lock<std::mutex> guard(latch_);
  auto lockmode = LockMode::SHARED;
  auto txn_id = txn->GetTransactionId();
  auto lock_req = LockRequest(txn_id,lockmode);

  auto iter = lock_table_.find(rid);
  if(iter == lock_table_.end()){
    lock_table_.emplace(std::piecewise_construct, std::forward_as_tuple(rid),
    std::forward_as_tuple());
    LockRequestQueue *lockque = &(lock_table_[rid]);
    lockque->request_queue_.emplace_back(lock_req);
    txn->GetSharedLockSet()->emplace(rid);
	  return true;
  }else{
    auto val = &(iter->second);
    std::list<LockRequest>* lock_list = &(val->request_queue_);
    lock_list->emplace_back(lock_req);
    for(auto it = lock_list->begin();it!=lock_list->end();it++){
      while(it->lock_mode_ == LockMode::EXCLUSIVE){
        val->cv_.wait(guard);
      }
    }
    txn->GetSharedLockSet()->emplace(rid);
	  return true;
  }
}

auto LockManager::LockExclusive(Transaction *txn, const RID &rid) -> bool {
  
  
  
  
  
  txn->GetExclusiveLockSet()->emplace(rid);
  return true;
}

auto LockManager::LockUpgrade(Transaction *txn, const RID &rid) -> bool {
  txn->GetSharedLockSet()->erase(rid);
  txn->GetExclusiveLockSet()->emplace(rid);
  return true;
}

auto LockManager::Unlock(Transaction *txn, const RID &rid) -> bool {
  std::unique_lock<std::mutex> guard(latch_);
  auto target = txn->GetTransactionId();
  //从队列中删除rid的锁
  auto iter = lock_table_.find(rid);
  if(iter == lock_table_.end()){
    assert(false);
    return false;
  }
  auto val = &(iter->second);
  std::list<LockRequest>* lock_list = &(val->request_queue_);
  auto pos = lock_list->begin();
  for(auto it=lock_list->begin();it!=lock_list->end();it++){
    if(it->txn_id_ == target){
      if(it->lock_mode_ == LockMode::SHARED){
        pos = it;
        txn->GetSharedLockSet()->erase(rid);
      }else{
        pos = it;
        txn->GetExclusiveLockSet()->erase(rid);
      }
      break;
    }
  }
  lock_list->erase(pos);
  txn->SetState(TransactionState::SHRINKING);
  val->cv_.notify_all();
  return true;
}

}  // namespace bustub
