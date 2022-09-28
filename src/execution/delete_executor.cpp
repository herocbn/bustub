//===----------------------------------------------------------------------===//
//
//                         BusTub
//
// delete_executor.cpp
//
// Identification: src/execution/delete_executor.cpp
//
// Copyright (c) 2015-2021, Carnegie Mellon University Database Group
//
//===----------------------------------------------------------------------===//

#include <memory>

#include "execution/executors/delete_executor.h"

namespace bustub {

DeleteExecutor::DeleteExecutor(ExecutorContext *exec_ctx, const DeletePlanNode *plan,
                               std::unique_ptr<AbstractExecutor> &&child_executor)
    : AbstractExecutor(exec_ctx), plan_(plan), child_executor_(std::move(child_executor)) {}

void DeleteExecutor::Init() {
  child_executor_->Init();
  table_info_ = exec_ctx_->GetCatalog()->GetTable(plan_->TableOid());
}

auto DeleteExecutor::Next([[maybe_unused]] Tuple *tuple, RID *rid) -> bool {
  Tuple del_tuple;
  RID del_rid;
  auto txn = exec_ctx_->GetTransaction();
  auto schema = table_info_->schema_;
  auto indexes = exec_ctx_->GetCatalog()->GetTableIndexes(table_info_->name_);
  while (child_executor_->Next(&del_tuple, &del_rid)) {
    bool res = table_info_->table_->MarkDelete(del_rid, txn);
    if (!res) {
      throw("Bad delete!");
    }
    Tuple key_tuple;
    for (auto idx_info : indexes) {
      auto key_schema = idx_info->key_schema_;
      auto key_attrs = idx_info->index_->GetKeyAttrs();
      key_tuple = del_tuple.KeyFromTuple(schema, key_schema, key_attrs);
      idx_info->index_->DeleteEntry(key_tuple, del_rid, txn);
    }
  }
  return false;
}

}  // namespace bustub
