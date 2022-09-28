//===----------------------------------------------------------------------===//
//
//                         BusTub
//
// update_executor.cpp
//
// Identification: src/execution/update_executor.cpp
//
// Copyright (c) 2015-2021, Carnegie Mellon University Database Group
//
//===----------------------------------------------------------------------===//
#include <memory>

#include "execution/executors/update_executor.h"

namespace bustub {

UpdateExecutor::UpdateExecutor(ExecutorContext *exec_ctx, const UpdatePlanNode *plan,
                               std::unique_ptr<AbstractExecutor> &&child_executor)
    : AbstractExecutor(exec_ctx), plan_(plan), child_executor_(std::move(child_executor)) {}

void UpdateExecutor::Init() {
  table_info_ = exec_ctx_->GetCatalog()->GetTable(plan_->TableOid());
  child_executor_->Init();
}

auto UpdateExecutor::Next([[maybe_unused]] Tuple *tuple, RID *rid) -> bool {
  // bool res = false;
  auto schema = (table_info_->schema_);
  auto txn = exec_ctx_->GetTransaction();
  // Get the original tuple
  Tuple old_tuple;
  RID old_rid;
  auto indexes = exec_ctx_->GetCatalog()->GetTableIndexes(table_info_->name_);
  // child_executor_->Next(&old_tuple,&old_rid);
  while (child_executor_->Next(&old_tuple, &old_rid)) {
    // Get the updated tuple
    auto updated_tuple = GenerateUpdatedTuple(old_tuple);
    // modify it in the table
    table_info_->table_->UpdateTuple(updated_tuple, old_rid, txn);
    // modify it in the index
    Tuple key_tuple;
    for (auto idx_info : indexes) {
      auto key_schema = idx_info->key_schema_;
      auto key_attrs = idx_info->index_->GetKeyAttrs();
      key_tuple = updated_tuple.KeyFromTuple(schema, key_schema, key_attrs);
      idx_info->index_->InsertEntry(key_tuple, old_rid, txn);
    }
  }
  return false;
}

auto UpdateExecutor::GenerateUpdatedTuple(const Tuple &src_tuple) -> Tuple {
  const auto &update_attrs = plan_->GetUpdateAttr();
  Schema schema = table_info_->schema_;
  uint32_t col_count = schema.GetColumnCount();
  std::vector<Value> values;
  for (uint32_t idx = 0; idx < col_count; idx++) {
    if (update_attrs.find(idx) == update_attrs.cend()) {
      values.emplace_back(src_tuple.GetValue(&schema, idx));
    } else {
      const UpdateInfo info = update_attrs.at(idx);
      Value val = src_tuple.GetValue(&schema, idx);
      switch (info.type_) {
        case UpdateType::Add:
          values.emplace_back(val.Add(ValueFactory::GetIntegerValue(info.update_val_)));
          break;
        case UpdateType::Set:
          values.emplace_back(ValueFactory::GetIntegerValue(info.update_val_));
          break;
      }
    }
  }
  return Tuple{values, &schema};
}

}  // namespace bustub
