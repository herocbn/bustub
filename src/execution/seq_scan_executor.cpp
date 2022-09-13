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
//#include "storage/table/table_iterator.h"

namespace bustub {

SeqScanExecutor::SeqScanExecutor(ExecutorContext *exec_ctx, const SeqScanPlanNode *plan) : AbstractExecutor(exec_ctx),plan_(plan),tb_iter_(nullptr,RID(),nullptr) {
		
}

void SeqScanExecutor::Init() {
	tb_info_ = exec_ctx_->GetCatalog()->GetTable(plan_->GetTableOid());
	tb_iter_ = tb_info_->table_->Begin(exec_ctx_->GetTransaction());
}

auto SeqScanExecutor::Next(Tuple *tuple, RID *rid) -> bool { 
	auto predicate = plan_->GetPredicate();
	auto schema = tb_info_->schema_;
	bool res;
	while(tb_iter_!=tb_info_->table_->End()){
		auto tp = &(*tb_iter_);
		if(predicate != nullptr){
			res = predicate->Evaluate(tp,&schema).GetAs<bool>();
		}
		//begin doing
		if(res){
			*tuple = *tp;
			*rid = tuple->GetRid();
		}
		tb_iter_++;
		if(res){
			return true;
		}
	}
	return false;
}

}  // namespace bustub
