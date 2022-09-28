//===----------------------------------------------------------------------===//
//
//                         BusTub
//
// insert_executor.cpp
//
// Identification: src/execution/insert_executor.cpp
//
// Copyright (c) 2015-2021, Carnegie Mellon University Database Group
//
//===----------------------------------------------------------------------===//

#include <memory>

#include "execution/executors/insert_executor.h"

namespace bustub {

InsertExecutor::InsertExecutor(ExecutorContext *exec_ctx, const InsertPlanNode *plan,
                               std::unique_ptr<AbstractExecutor> &&child_executor)
    : AbstractExecutor(exec_ctx), plan_(plan), child_executor_(std::move(child_executor)) {}

void InsertExecutor::Init() {
  if (!plan_->IsRawInsert()) {
    child_executor_->Init();
  } else {
    value_iter_ = plan_->RawValues().cbegin();
  }
  tb_info_ = exec_ctx_->GetCatalog()->GetTable(plan_->TableOid());
  indexes_ = exec_ctx_->GetCatalog()->GetTableIndexes(tb_info_->name_);
}
// Catalog
// TableHeap
auto InsertExecutor::Next([[maybe_unused]] Tuple *tuple, RID *rid) -> bool {
  bool res = true;
  Tuple insert_tuple;
  RID insert_rid;
  auto txn = exec_ctx_->GetTransaction();
  auto schema = (tb_info_->schema_);
  while (res) {
    if (!plan_->IsRawInsert()) {
      res = child_executor_->Next(&insert_tuple, &insert_rid);
    } else {
      if (value_iter_ == plan_->RawValues().cend()) {
        res = false;
      } else {
        insert_tuple = Tuple(*value_iter_, &schema);
        ++value_iter_;
        res = true;
      }
    }
    if (res) {
      // insert into the table
      tb_info_->table_->InsertTuple(insert_tuple, &insert_rid, txn);

      // update index
      Tuple key_tuple;
      for (auto idx_info : indexes_) {
        auto key_schema = idx_info->key_schema_;
        auto key_attrs = idx_info->index_->GetKeyAttrs();
        key_tuple = insert_tuple.KeyFromTuple(schema, key_schema, key_attrs);
        idx_info->index_->InsertEntry(key_tuple, insert_rid, txn);
      }
    }
  }
  return res;
}
}  // namespace bustub
