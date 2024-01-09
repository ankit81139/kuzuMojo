// src/chrome/services/kuzu/kuzu_service.h
#pragma once

#include <vector>
#include "chrome/services/kuzu/public/mojom/kuzu_service.mojom.h"
#include "kuzu.h"
#include "kuzu_wrapper.cc"
#include "mojo/public/cpp/bindings/pending_receiver.h"
#include "mojo/public/cpp/bindings/receiver.h"

namespace kuzu {

class KuzuDatabase : public mojom::KuzuDatabase {
 public:
  explicit KuzuDatabase(mojo::PendingReceiver<mojom::KuzuDatabase> receiver);
  KuzuDatabase(const KuzuDatabase&) = delete;
  KuzuDatabase& operator=(const KuzuDatabase&) = delete;
  ~KuzuDatabase() override;

 private:
  // mojom::KuzuDatabase:
  void CreateDatabase(const std::string& db_path, CreateDatabaseCallback callback) override;
  void CreateConnection(CreateConnectionCallback callback) override;
  void Query(const std::string& query, QueryCallback callback) override;
  void SetResultData(SetResultDataCallback callback) override;
  void LogResult(LogResultCallback callback) override;

  std::unique_ptr<kuzu_database> database;
  std::unique_ptr<kuzu_connection> connection;
  std::unique_ptr<kuzu_query_result> query_result;
  std::vector<std::pair<std::string, int64_t>> result_data;

  Wrapper kuzu_wrapper;
  mojo::Receiver<mojom::KuzuDatabase> receiver_;
};

}  // namespace kuzu
