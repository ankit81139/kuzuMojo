// src/chrome/services/kuzu/kuzu_service.h
#pragma once

#include <vector>
#include "chrome/services/kuzu/public/mojom/kuzu_service.mojom.h"
#include "kuzu.h"
#include "kuzu_wrapper.cc"
#include "base/json/json_writer.h"
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
  void CreateDatabase(const std::string& db_path, CreateDatabaseCallback callback) override;
  void CreateConnection(CreateConnectionCallback callback) override;
  void Query(const std::string& query, QueryCallback callback) override;
  void SetMaxNumberOfThread(const std::string& num_threads, SetMaxNumberOfThreadCallback callback) override;
  void GetMaxNumberOfThread(GetMaxNumberOfThreadCallback callback) override;
  void WriteToCSV(const std::string& query, const std::string& file_path, const std::string& delimiter, const std::string& escape_char, const std::string& new_line, WriteToCSVCallback callback) override;

  std::unique_ptr<kuzu_database> database_;
  std::unique_ptr<kuzu_connection> connection_;
  Wrapper kuzu_wrapper_;
  mojo::Receiver<mojom::KuzuDatabase> receiver_;
};

}  // namespace kuzu
