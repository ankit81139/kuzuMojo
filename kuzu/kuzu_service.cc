// src/chrome/services/kuzu/kuzu_service.cc
#include "chrome/services/kuzu/kuzu_service.h"

namespace kuzu {
KuzuDatabase::KuzuDatabase(mojo::PendingReceiver<mojom::KuzuDatabase> receiver)
    : database(nullptr),
      connection(nullptr),
      receiver_(this, std::move(receiver)) {
}
KuzuDatabase::~KuzuDatabase(){
  LOG(ERROR) << __func__ << " called";
}

void KuzuDatabase::CreateDatabase(const std::string& db_path,CreateDatabaseCallback callback) {
  LOG(ERROR) << __func__ << " 1";
  if(this->database == nullptr)
    this->database = kuzu_wrapper.DatabaseInitWrapper(db_path);

  if(!this->database){
    LOG(ERROR) << "Database could not be initiated\n";
  }
  else{
    LOG(ERROR) << "Database initiated successfully\n";
  }
  std::move(callback).Run();
}

void KuzuDatabase::CreateConnection(CreateConnectionCallback callback) {
  LOG(ERROR) << __func__ << " 1";

  if(this->connection == nullptr)
    this->connection = kuzu_wrapper.ConnectionInitWrapper(this->database.get());

  if(!this->connection){
    LOG(ERROR) << "Connection could not be established\n";
  }
  else{
    LOG(ERROR) << "Connection Established successfully\n";
  }
  std::move(callback).Run();
}

void KuzuDatabase::Query(const std::string& query, QueryCallback callback) {
  LOG(ERROR) << __func__ << " 1";
  raw_ptr<kuzu_query_result> query_result =kuzu_connection_query(this->connection.get(), query.c_str());
  if(!kuzu_wrapper.QueryResultIsSuccessWrapper(query_result.get())){
    std::string str = kuzu_wrapper.QueryResultGetErrorMessageWrapper(query_result.get());
    std::move(callback).Run(str);
    return;
  }
  LOG(ERROR) << __func__ << " 2";
  if(!query_result){
    LOG(ERROR) << "Query could not be Performed\n";
  }
  else{
    LOG(ERROR) << "Query Result was saved successfully\n";
    if(query.substr(0,5) == "MATCH"){
      // this->LogResult(base::BindOnce([]() { LOG(INFO) << "hurrah"; }));
      auto value = kuzu_wrapper.GetDataWrapper(query_result.get());
      std::string str;
      base::JSONWriter::Write(value, &str);
      std::move(callback).Run(str);
      return;
    }
  }
  LOG(ERROR) << __func__ << " 3";
  kuzu_query_result_destroy(query_result);
  std::move(callback).Run(" chhh");
}

// void KuzuDatabase::SetResultData(SetResultDataCallback callback) {
//   LOG(ERROR) << __func__ << " 1";
//   this->result_data = kuzu_wrapper.GetDataWrapper(query_result.get());
//   if(result_data.size() == 0){
//     LOG(ERROR) << "Result is empty\n";
//   }
//   else{
//     LOG(ERROR) << "Result contains "<< result_data.size() <<" tuples\n";
//   }
//   std::move(callback).Run();
// }

// void KuzuDatabase::LogResult(LogResultCallback callback) {
//   LOG(ERROR) << __func__ << " 1";
//   this->SetResultData(base::BindOnce([]() { LOG(INFO) << "hurrah"; }));
//   for (auto& value_pair : this->result_data) {
//     LOG(ERROR) << "name: " << value_pair.first << " age: " << value_pair.second;
//   }
//   std::move(callback).Run();
// }

}  // namespace kuzu


