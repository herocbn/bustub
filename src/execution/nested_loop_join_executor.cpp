//===----------------------------------------------------------------------===//
//
//                         BusTub
//
// nested_loop_join_executor.cpp
//
// Identification: src/execution/nested_loop_join_executor.cpp
//
// Copyright (c) 2015-2021, Carnegie Mellon University Database Group
//
//===----------------------------------------------------------------------===//

#include "execution/executors/nested_loop_join_executor.h"

namespace bustub {

NestedLoopJoinExecutor::NestedLoopJoinExecutor(ExecutorContext *exec_ctx, const NestedLoopJoinPlanNode *plan,
                                               std::unique_ptr<AbstractExecutor> &&left_executor,
                                               std::unique_ptr<AbstractExecutor> &&right_executor)
    : AbstractExecutor(exec_ctx), plan_(plan), left_executor_(std::move(left_executor)),right_executor_(std::move(right_executor)) {}

void NestedLoopJoinExecutor::Init() {
	catalog_ = exec_ctx_->GetCatalog();
	
	left_plan_ = plan_->GetLeftPlan();
	right_plan_ = plan_->GetRightPlan();

	right_executor_->Init();
	left_executor_->Init();
	
	flag_ = false; 
}

auto NestedLoopJoinExecutor::Next(Tuple *tuple, RID *rid) -> bool {
	const Schema* output_schema = GetOutputSchema();
	auto predicate = plan_->Predicate();
	RID right_rid;
	Tuple right_tuple;
	bool res;
	auto left_schema = left_plan_->OutputSchema();
	auto right_schema = right_plan_->OutputSchema();

	while(flag_||left_executor_->Next(&left_tuple_, &left_rid_)){
		while(right_executor_->Next(&right_tuple, &right_rid)){
			res = true;
			if(predicate!=nullptr){
				res = predicate->EvaluateJoin(&left_tuple_, left_schema,&right_tuple, right_schema).GetAs<bool>();
			}
			if(res){
				std::vector<Value> values;
				values.reserve(output_schema->GetColumnCount());
				for(const auto &column:output_schema->GetColumns()){
					values.push_back(column.GetExpr()->EvaluateJoin(&left_tuple_, left_schema, &right_tuple, right_schema));
				}
				*tuple = Tuple(values,output_schema);
				flag_ = true;
				return true;
			}
		}
		flag_ = false;
		right_executor_->Init();
	}
	return false;
}

}  // namespace bustub
