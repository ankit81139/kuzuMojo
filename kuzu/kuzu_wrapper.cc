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

  std::string QueryResultGetErrorMessageWrapper(raw_ptr<kuzu_query_result> result){
    return std::string(kuzu_query_result_get_error_message(result));
  }

  bool QueryResultIsSuccessWrapper(raw_ptr<kuzu_query_result> result){
    return kuzu_query_result_is_success(result);
  }

  std::unique_ptr<kuzu_query_result> ConnectionQueryWrapper(raw_ptr<kuzu_connection> connection,const std::string& query) {
    LOG(ERROR) << __func__ << " 1";
    return std::unique_ptr<kuzu_query_result>(kuzu_connection_query(connection, query.c_str()));
  }

  base::Value::List KuzuLogicalTypeToBaseValueList(kuzu_data_type_id type_id, raw_ptr<kuzu_value> value){
    switch(type_id){
      case KUZU_INTERVAL:{
        kuzu_interval_t val = kuzu_value_get_interval(value);
        base::Value::List list;
        list.Append(base::Value(static_cast<int>(val.months)));
        list.Append(base::Value(static_cast<int>(val.days)));
        list.Append(base::Value(static_cast<int>(val.micros)));
        return list;
      }

      case KUZU_FIXED_LIST:{
        base::Value::List list;
        uint64_t size_of_list = kuzu_value_get_list_size(value);
        for(uint64_t index = 0; index < size_of_list; index++){
          kuzu_value* val = kuzu_value_get_list_element(value, index);
          kuzu_logical_type* data_type = kuzu_value_get_data_type(val);
          kuzu_data_type_id type =  kuzu_data_type_get_id(data_type);
          
          if(type == KUZU_INTERNAL_ID || type == KUZU_FIXED_LIST || type == KUZU_VAR_LIST || type == KUZU_INTERVAL){
            auto val1 = KuzuLogicalTypeToBaseValueList(type, value);
            list.Append(std::move(val1));
          }
          else{
            auto val1 = KuzuLogicalTypeToBaseValue(type, value);
            list.Append(std::move(val1));
          }

          kuzu_value_destroy(value);
          kuzu_data_type_destroy(data_type);
        }
        return list;
      }

      case KUZU_INTERNAL_ID:{
        kuzu_internal_id_t val = kuzu_value_get_internal_id(value);
        base::Value::List list;
        list.Append(base::Value(static_cast<int> (val.offset)));
        list.Append(base::Value(static_cast<int>(val.table_id)));
        return list;
      }

      case KUZU_VAR_LIST:{
        base::Value::List list;
        uint64_t size_of_list = kuzu_value_get_list_size(value);
        for(uint64_t index = 0; index < size_of_list; index++){
          kuzu_value* val = kuzu_value_get_list_element(value, index);
          kuzu_logical_type* data_type = kuzu_value_get_data_type(val);
          kuzu_data_type_id type =  kuzu_data_type_get_id(data_type);

          if(type == KUZU_INTERNAL_ID || type == KUZU_FIXED_LIST || type == KUZU_VAR_LIST || type == KUZU_INTERVAL){
            auto val1 = KuzuLogicalTypeToBaseValueList(type, value);
            list.Append(std::move(val1));
          } 
          else{
            auto val1 = KuzuLogicalTypeToBaseValue(type, value);
            list.Append(std::move(val1));
          }

          kuzu_value_destroy(value);
          kuzu_data_type_destroy(data_type);
        }
        return list;
      }

      default: {
        base::Value::List list;
        return list;
      } 
    }
  }


  base::Value KuzuLogicalTypeToBaseValue(kuzu_data_type_id type_id, raw_ptr<kuzu_value> value){
    switch (type_id){
      case KUZU_BOOL:
        return base::Value(kuzu_value_get_bool(value));

      case KUZU_INT128:{
        kuzu_int128_t input = kuzu_value_get_int128(value);
        return base::Value(kuzu_int128_t_to_string(input));
      }

      case KUZU_INT64:{
        return base::Value(static_cast<int>(kuzu_value_get_int64(value)));
      }

      case KUZU_INT32:
        return base::Value(static_cast<int>(kuzu_value_get_int32(value)));

      case KUZU_INT16:
        return base::Value(static_cast<int>(kuzu_value_get_int16(value)));

      case KUZU_INT8:
        return base::Value(static_cast<int>(kuzu_value_get_int8(value)));

      case KUZU_UINT64:
        return base::Value(static_cast<int>(kuzu_value_get_uint64(value)));

      case KUZU_UINT32:
        return base::Value(static_cast<int>(kuzu_value_get_uint32(value)));
    
      case KUZU_UINT16:
        return base::Value(static_cast<int>(kuzu_value_get_uint16(value)));

      case KUZU_UINT8:
        return base::Value(static_cast<int>(kuzu_value_get_uint8(value)));

      case KUZU_DOUBLE:
        return base::Value(static_cast<double>(kuzu_value_get_double(value)));

      case KUZU_FLOAT:
        return base::Value(static_cast<double>(kuzu_value_get_float(value)));

      case KUZU_DATE:
        return base::Value(static_cast<int>(kuzu_value_get_date(value).days));

      case KUZU_TIMESTAMP:
        return base::Value(static_cast<int>(kuzu_value_get_timestamp(value).value));

      case KUZU_STRING:
        return base::Value(kuzu_value_get_string(value));
    
      default:
        return base::Value(0);
    }
  }

  base::Value::List GetDataWrapper(raw_ptr<kuzu_query_result> result) {
    kuzu_query_result* res = result;
    base::Value::List data;
    uint64_t result_num_columns = kuzu_query_result_get_num_columns(res);
    while (kuzu_query_result_has_next(res)) {
      kuzu_flat_tuple* tuple = kuzu_query_result_get_next(res);
      base::Value::List tup;
      for(uint64_t index = 0; index < result_num_columns; index++){
        kuzu_value* value = kuzu_flat_tuple_get_value(tuple, index);
        kuzu_logical_type* data_type = kuzu_value_get_data_type(value);
        kuzu_data_type_id type_id =  kuzu_data_type_get_id(data_type);
        if(type_id == KUZU_INTERNAL_ID || type_id == KUZU_FIXED_LIST || type_id == KUZU_VAR_LIST || type_id == KUZU_INTERVAL){
          auto val = KuzuLogicalTypeToBaseValueList(type_id, value);
          tup.Append(std::move(val));
        }
        else{
          auto val = KuzuLogicalTypeToBaseValue(type_id, value);
          tup.Append(std::move(val));
        }
        kuzu_value_destroy(value);
      }
      data.Append(std::move(tup));
      kuzu_flat_tuple_destroy(tuple);
    }
    kuzu_query_result_destroy(res);
    return data;
  }

  
};
