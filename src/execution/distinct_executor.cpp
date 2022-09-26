//===----------------------------------------------------------------------===//
//
//                         BusTub
//
// distinct_executor.cpp
//
// Identification: src/execution/distinct_executor.cpp
//
// Copyright (c) 2015-2021, Carnegie Mellon University Database Group
//
//===----------------------------------------------------------------------===//

#include "execution/executors/distinct_executor.h"

namespace bustub {

DistinctExecutor::DistinctExecutor(ExecutorContext *exec_ctx, const DistinctPlanNode *plan,
                                   std::unique_ptr<AbstractExecutor> &&child_executor)
    : AbstractExecutor(exec_ctx),plan_(plan), child_executor_(std::move(child_executor)) {}

void DistinctExecutor::Init() {
	child_executor_->Init();
}

auto DistinctExecutor::Next(Tuple *tuple, RID *rid) -> bool { 
	auto child_schema = child_executor_->GetOutputSchema();
	auto output_schema = GetOutputSchema();
	while(child_executor_->Next(tuple,rid)){
		hash_key key;
		std::vector<Value> values;
		/*auto columns = output_schema->GetColumns();
		values.reserve(columns.size());
		for(const auto &col:columns){
			values.push_back(col.GetExpr()->Evaluate(tuple,child_schema));
		}
		*/
		for(uint32_t i = 0;i<child_schema->GetColumnCount();i++){
			values.push_back(tuple->GetValue(child_schema,i));
		}
		key.keys_ = values;
		bool res = h_set_.insert(key).second;
		if(res){
			*tuple = Tuple(values,output_schema);
			return true;
		}
	}
	return false;
}

}  // namespace bustub
