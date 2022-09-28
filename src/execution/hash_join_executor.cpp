//===----------------------------------------------------------------------===//
//
//                         BusTub
//
// hash_join_executor.cpp
//
// Identification: src/execution/hash_join_executor.cpp
//
// Copyright (c) 2015-2021, Carnegie Mellon University Database Group
//
//===----------------------------------------------------------------------===//

#include "execution/executors/hash_join_executor.h"
#include "execution/expressions/abstract_expression.h"
namespace bustub {
HashJoinExecutor::HashJoinExecutor(ExecutorContext *exec_ctx, const HashJoinPlanNode *plan,
                                   std::unique_ptr<AbstractExecutor> &&left_child,
                                   std::unique_ptr<AbstractExecutor> &&right_child)
    : AbstractExecutor(exec_ctx),
      plan_(plan),
      left_child_(std::move(left_child)),
      right_child_(std::move(right_child)) {}

void HashJoinExecutor::Init() {
  left_child_->Init();
  right_child_->Init();
  Tuple tuple;
  RID rid;
  l_schema_ = left_child_->GetOutputSchema();
  while (left_child_->Next(&tuple, &rid)) {
    auto left_key = plan_->LeftJoinKeyExpression()->Evaluate(&tuple, l_schema_);
    HashJoinKey key;
    key.keys_ = left_key;
    if (h_map_.count(key) == 0) {
      h_map_.insert({key, std::vector<Tuple>{tuple}});
    } else {
      h_map_[key].push_back(tuple);
    }
  }
  flag_ = false;
}

auto HashJoinExecutor::Next(Tuple *tuple, RID *rid) -> bool {
  auto r_schema = right_child_->GetOutputSchema();
  auto schema = GetOutputSchema();
  auto r_expr = plan_->RightJoinKeyExpression();
  while (flag_ || right_child_->Next(&right_tuple_, &right_rid_)) {
    auto right_k = (r_expr->Evaluate(&right_tuple_, r_schema));
    HashJoinKey right_key;
    right_key.keys_ = right_k;
    if (h_map_.count((right_key)) != 0) {
      if (left_tuples_.empty()) {
        left_tuples_ = h_map_[right_key];
      }
      std::vector<Value> values;
      auto columns = schema->GetColumns();
      values.reserve(columns.size());
      auto left_tuple = left_tuples_.back();
      left_tuples_.pop_back();
      for (const auto &col : columns) {
        values.push_back(col.GetExpr()->EvaluateJoin(&left_tuple, l_schema_, &right_tuple_, r_schema));
      }

      *tuple = Tuple(values, schema);
      flag_ = !left_tuples_.empty();
      return true;
    }
    flag_ = false;
  }
  return false;
}

}  // namespace bustub
