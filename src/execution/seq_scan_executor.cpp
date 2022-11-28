//===----------------------------------------------------------------------===//
//
//                         BusTub
//
// seq_scan_executor.cpp
//
// Identification: src/execution/seq_scan_executor.cpp
//
// Copyright (c) 2015-2021, Carnegie Mellon University Database Group
//
//===----------------------------------------------------------------------===//

#include "execution/executors/seq_scan_executor.h"

namespace bustub {

SeqScanExecutor::SeqScanExecutor(ExecutorContext *exec_ctx, const SeqScanPlanNode *plan)
    : AbstractExecutor(exec_ctx), plan_(plan), table_iter_(nullptr, RID(), nullptr) {}
bool SeqScanExecutor::IsSameSchema(const Schema *opt, const Schema *schema) {
  return (opt->ToString() == schema->ToString());
}
void SeqScanExecutor::Init() {
  table_info_ = exec_ctx_->GetCatalog()->GetTable(plan_->GetTableOid());
  table_iter_ = table_info_->table_->Begin(exec_ctx_->GetTransaction());
  auto output_schema = plan_->OutputSchema();
  auto table_schema = table_info_->schema_;
  is_same_schema_ = IsSameSchema(output_schema, &table_schema);
  auto transaction = exec_ctx_->GetTransaction();
  auto lockmanager = exec_ctx_->GetLockManager();
  if (transaction->GetIsolationLevel() == IsolationLevel::REPEATABLE_READ) {
    auto iter = table_info_->table_->Begin(exec_ctx_->GetTransaction());
    while (iter != table_info_->table_->End()) {
      lockmanager->LockShared(transaction, iter->GetRid());
      ++iter;
    }
  }
}
// Catalog
auto SeqScanExecutor::Next(Tuple *tuple, RID *rid) -> bool {
  auto predicate = plan_->GetPredicate();
  auto schema = table_info_->schema_;
  auto opt = plan_->OutputSchema();
  bool res;
  auto txn = exec_ctx_->GetTransaction();
  auto lock_mgr = exec_ctx_->GetLockManager();
  while (table_iter_ != table_info_->table_->End()) {
    res = true;
    auto tp = &(*table_iter_);
    if (txn->GetIsolationLevel() == IsolationLevel::READ_COMMITTED) {
      lock_mgr->LockShared(txn, tp->GetRid());
    }

    if (predicate != nullptr) {
      res = predicate->Evaluate(tp, &schema).GetAs<bool>();
    }
    if (res) {
      if (is_same_schema_) {
        *tuple = *tp;
        *rid = tp->GetRid();
      } else {
        std::vector<Value> values;
        values.reserve(opt->GetColumnCount());
        for (const auto &column : opt->GetColumns()) {
          auto value = column.GetExpr()->Evaluate(tp, &schema);
          values.push_back(value);
        }
        *tuple = Tuple(values, opt);
        *rid = tp->GetRid();
      }
      if (txn->GetIsolationLevel() == IsolationLevel::READ_COMMITTED) {
        lock_mgr->Unlock(txn, tp->GetRid());
      }
    }
    ++table_iter_;
    if (res) {
      return true;
    }
  }
  return false;
}

}  // namespace bustub
