// src/chrome/services/kuzu/kuzu_service.cc
#include "chrome/services/kuzu/kuzu_service.h"

namespace kuzu {
  KuzuDatabase::KuzuDatabase(mojo::PendingReceiver<mojom::KuzuDatabase> receiver)
    : database_(nullptr),
      connection_(nullptr),
      receiver_(this, std::move(receiver)) {
  }
  KuzuDatabase::~KuzuDatabase(){
    LOG(ERROR) << __func__ << " called";
  }

  void KuzuDatabase::CreateDatabase(const std::string& db_path,CreateDatabaseCallback callback) {
    LOG(ERROR) << __func__ << " 1";
    if(database_ == nullptr)
      database_ = kuzu_wrapper_.DatabaseInitWrapper(db_path);
    std::string success_message;
    if(!database_){
      success_message = "Database could not be initiated";
    }
    else{
      success_message = "Database initiated successfully";
    }
    std::move(callback).Run(success_message);
  }

  void KuzuDatabase::CreateConnection(CreateConnectionCallback callback) {
    LOG(ERROR) << __func__ << " 1";

    if(connection_ == nullptr)
      connection_ = kuzu_wrapper_.ConnectionInitWrapper(database_.get());
    std::string success_message;
    if(!connection_){
      success_message = "Connection could not be established";
    }
    else{
      success_message = "Connection Established successfully";
    }
    std::move(callback).Run(success_message);
  }

  void KuzuDatabase::Query(const std::string& query, QueryCallback callback) {
    LOG(ERROR) << __func__ << " 1";
    raw_ptr<kuzu_query_result> query_result = kuzu_connection_query(connection_.get(), query.c_str());
    if(!kuzu_wrapper_.QueryResultIsSuccessWrapper(query_result.get())){
      std::string str = kuzu_wrapper_.QueryResultGetErrorMessageWrapper(query_result.get());
      std::move(callback).Run(str);
      return;
    }
    std::string message;
    if(!query_result){
      message = "Query could not be Performed";
    }
    else{
      if(query.substr(0,5) == "MATCH"){
        base::Value::List value = kuzu_wrapper_.GetDataWrapper(query_result.get());
        std::string str;
        base::JSONWriter::Write(value, &str);
        std::move(callback).Run(str);
        return;
      }
    }
    LOG(ERROR) << __func__ << " 2";
    kuzu_query_result_destroy(query_result);
    std::move(callback).Run(message);
  }

  void KuzuDatabase::SetMaxNumberOfThread(const std::string& num_threads, SetMaxNumberOfThreadCallback callback){
    int num_thread = stoi(num_threads);
    kuzu_connection_set_max_num_thread_for_exec(connection_.get(), num_thread);
  }

  void KuzuDatabase::GetMaxNumberOfThread(GetMaxNumberOfThreadCallback callback){
    base::Value value = base::Value((int)kuzu_connection_get_max_num_thread_for_exec(connection_.get()));
    std::string str;
    base::JSONWriter::Write(value, &str);
    std::move(callback).Run(str);
  }

  void KuzuDatabase::WriteToCSV(const std::string& query, const std::string& file_path, const std::string& delimiter, const std::string& escape_char, const std::string& new_line, WriteToCSVCallback callback){
    raw_ptr<kuzu_query_result> query_result =kuzu_connection_query(connection_.get(), query.c_str());
    if(!kuzu_wrapper_.QueryResultIsSuccessWrapper(query_result.get())){
      std::string str = kuzu_wrapper_.QueryResultGetErrorMessageWrapper(query_result.get());
      std::move(callback).Run(str);
      return;
    }
    LOG(ERROR) << __func__ << " 2";
    if(!query_result){
      LOG(ERROR) << "Query could not be Performed\n";
    }
    else{
      kuzu_query_result_write_to_csv(query_result.get(), file_path.c_str(), delimiter[0], escape_char[0], new_line[0]);
      std::string str = "Result written on the csv correctly";
      std::move(callback).Run(str);
      return;
    }
    LOG(ERROR) << __func__ << " 3";
    kuzu_query_result_destroy(query_result);
    std::move(callback).Run(" chhh");
  }
}  // namespace kuzu


