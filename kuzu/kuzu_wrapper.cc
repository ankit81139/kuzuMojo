#include "chrome/services/kuzu/public/mojom/kuzu_service.mojom.h"
#include "kuzu.h"



class Wrapper {

 public:
  Wrapper() {}
  ~Wrapper() {}


  std::unique_ptr<kuzu_database> DatabaseInitWrapper(std::string db_path) {
    return std::unique_ptr<kuzu_database>(kuzu_database_init(db_path.c_str(), kuzu_default_system_config()));
  }


  std::unique_ptr<kuzu_connection> ConnectionInitWrapper(raw_ptr<kuzu_database> database) {
    return std::unique_ptr<kuzu_connection>(kuzu_connection_init(database));
  }


  std::unique_ptr<kuzu_query_result> ConnectionQueryWrapper(raw_ptr<kuzu_connection> connection,const std::string& query) {
    return std::unique_ptr<kuzu_query_result>(kuzu_connection_query(connection, query.c_str()));
  }

  std::string QueryResultGetErrorMessageWrapper(raw_ptr<kuzu_query_result> result){
    return std::string(kuzu_query_result_get_error_message(result));
  }

  bool QueryResultIsSuccessWrapper(raw_ptr<kuzu_query_result> result){
    return kuzu_query_result_is_success(result);
  }


  void LogKuzuLogicalType(kuzu_data_type_id type_id, raw_ptr<kuzu_value> value){
    // KUZU_BOOL = 22,
    // KUZU_INT64 = 23,
    // KUZU_INT32 = 24,
    // KUZU_INT16 = 25,
    // KUZU_INT8 = 26,
    // KUZU_UINT64 = 27,
    // KUZU_UINT32 = 28,
    // KUZU_UINT16 = 29,
    // KUZU_UINT8 = 30,
    // KUZU_INT128 = 31,
    // KUZU_DOUBLE = 32,
    // KUZU_FLOAT = 33,
    // KUZU_DATE = 34,
    // KUZU_TIMESTAMP = 35,
    // KUZU_INTERVAL = 36,
    // KUZU_FIXED_LIST = 37,
    // KUZU_INTERNAL_ID = 40,
    // KUZU_ARROW_COLUMN = 41,
    // KUZU_STRING = 50
    

    switch (type_id)
    {
    case KUZU_BOOL:
      LOG(ERROR) << __func__ << " " << kuzu_value_get_bool(value);
      break;

    case KUZU_INT128:
      {
        kuzu_int128_t val = kuzu_value_get_int128(value);
        LOG(ERROR) << __func__ << " " << val.low << " " << val.high;
      }
      break;

    case KUZU_INT64:
      LOG(ERROR) << __func__ << " " << kuzu_value_get_int64(value);
      break;

    case KUZU_INT32:
      LOG(ERROR) << __func__ << " " << kuzu_value_get_int32(value);
      break;

    case KUZU_INT16:
      LOG(ERROR) << __func__ << " " << kuzu_value_get_int16(value);
      break;

    case KUZU_INT8:
      LOG(ERROR) << __func__ << " " << kuzu_value_get_int8(value);
      break;

    case KUZU_UINT64:
      LOG(ERROR) << __func__ << " " << kuzu_value_get_uint64(value);
      break;

    case KUZU_UINT32:
      LOG(ERROR) << __func__ << " " << kuzu_value_get_uint32(value);
      break;
    
    case KUZU_UINT16:
      LOG(ERROR) << __func__ << " " << kuzu_value_get_uint16(value);
      break;

    case KUZU_UINT8:
      LOG(ERROR) << __func__ << " " << kuzu_value_get_uint8(value);
      break;

    case KUZU_DOUBLE:
      LOG(ERROR) << __func__ << " " << kuzu_value_get_double(value);
      break;

    case KUZU_FLOAT:
      LOG(ERROR) << __func__ << " " << kuzu_value_get_float(value);
      break;

    case KUZU_DATE:
      LOG(ERROR) << __func__ << " " << kuzu_value_get_date(value).days;
      break;

    case KUZU_TIMESTAMP:
      LOG(ERROR) << __func__ << " " << kuzu_value_get_timestamp(value).value;
      break;

    case KUZU_INTERVAL:
      {
        kuzu_interval_t val = kuzu_value_get_interval(value);
        LOG(ERROR) << __func__ << " " << val.months << " " << val.days << " " << val.micros;
      }
      break;

    case KUZU_FIXED_LIST:
      {
        uint64_t size_of_list = kuzu_value_get_list_size(value);
        for(uint64_t index = 0; index < size_of_list; index++){
          kuzu_value* val = kuzu_value_get_list_element(value, index);
          kuzu_logical_type* data_type = kuzu_value_get_data_type(val);
          kuzu_data_type_id type =  kuzu_data_type_get_id(data_type);
          LogKuzuLogicalType(type, val);
          kuzu_value_destroy(value);
        }
      }
      break;

    case KUZU_INTERNAL_ID:
      {
        kuzu_internal_id_t val = kuzu_value_get_internal_id(value);
        LOG(ERROR) << __func__ << " " << val.offset << " " << val.table_id;
      }
      break;

    case KUZU_STRING:
      LOG(ERROR) << __func__ << " " << kuzu_value_get_string(value);
      break;
    
    default:
      break;
    }
  }

  std::vector<std::pair<std::string, int64_t>> GetDataWrapper(raw_ptr<kuzu_query_result> result) {
    kuzu_query_result* res = result;
    std::vector<std::pair<std::string, int64_t>> data;


    uint64_t result_num_columns = kuzu_query_result_get_num_columns(res);
    while (kuzu_query_result_has_next(res)) {
      kuzu_flat_tuple* tuple = kuzu_query_result_get_next(res);
      for(uint64_t index = 0; index < result_num_columns; index++){
        kuzu_value* value = kuzu_flat_tuple_get_value(tuple, index);
        kuzu_logical_type* data_type = kuzu_value_get_data_type(value);
        kuzu_data_type_id type_id =  kuzu_data_type_get_id(data_type);
        LogKuzuLogicalType(type_id, value);
        kuzu_value_destroy(value);
      }
      
      // std::string name(kuzu_value_get_string(value));
      
      // value = kuzu_flat_tuple_get_value(tuple, 1);
      // int64_t age = kuzu_value_get_int64(value);
      // kuzu_value_destroy(value);
      // data.push_back({name, age});
      kuzu_flat_tuple_destroy(tuple);



    //   kuzu_query_result* res = result;
    // std::vector<std::pair<std::string, int64_t>> data;
    // while (kuzu_query_result_has_next(res)) {
    //   kuzu_flat_tuple* tuple = kuzu_query_result_get_next(res);
    //   kuzu_value* value = kuzu_flat_tuple_get_value(tuple, 0);
    //   std::string name(kuzu_value_get_string(value));
    //   kuzu_value_destroy(value);
    //   value = kuzu_flat_tuple_get_value(tuple, 1);
    //   int64_t age = kuzu_value_get_int64(value);
    //   kuzu_value_destroy(value);
    //   data.push_back({name, age});
    //   kuzu_flat_tuple_destroy(tuple);
    // }
    // kuzu_query_result_destroy(res);
    // return data;
    }
    kuzu_query_result_destroy(res);
    return data;
  }

  
};
