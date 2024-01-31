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
    LOG(ERROR) << __func__ << " 1";
    return std::unique_ptr<kuzu_query_result>(kuzu_connection_query(connection, query.c_str()));
  }

  std::string QueryResultGetErrorMessageWrapper(raw_ptr<kuzu_query_result> result){
    return std::string(kuzu_query_result_get_error_message(result));
  }

  bool QueryResultIsSuccessWrapper(raw_ptr<kuzu_query_result> result){
    return kuzu_query_result_is_success(result);
  }


  base::Value KuzuLogicalTypeToBaseValue(kuzu_data_type_id type_id, raw_ptr<kuzu_value> value){
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
      // case KUZU_BLOB:
      //   return base::Value((uint8_t)kuzu_value_get_blob(value));
      //   break;
      // case KUZU_NODE:{
      //   kuzu_value* node_val = kuzu_node_val_get_id_val(value);
      //   kuzu_logical_type* data_type = kuzu_value_get_data_type(node_val);
      //   kuzu_data_type_id type =  kuzu_data_type_get_id(data_type);
      //   LogKuzuLogicalType(type, node_val);
      // }
      // break;

      case KUZU_BOOL:
        return base::Value(kuzu_value_get_bool(value));

      // case KUZU_INT128:{
      //   kuzu_int128_t input = kuzu_value_get_int128(value);
      // //   LOG(ERROR) << __func__ << " " << type_id << " " <<  val.low << " " << val.high;
      //   return base::Value(static_cast<uint128_t>(static_cast<uint64_t>(input.high)) << 64 | input.low);
      // }
      // break;

      case KUZU_INT64:
        return base::Value((int)kuzu_value_get_int64(value));

      case KUZU_INT32:
        return base::Value((int)kuzu_value_get_int32(value));

      case KUZU_INT16:
      //   LOG(ERROR) << __func__ << " " << type_id << " " <<  kuzu_value_get_int16(value);
        return base::Value((int)kuzu_value_get_int16(value));
      //   break;

      case KUZU_INT8:
      //   LOG(ERROR) << __func__ << " " << type_id << " " <<  kuzu_value_get_int8(value);
        return base::Value((int)kuzu_value_get_int8(value));
      //   break;

      case KUZU_UINT64:
      //   LOG(ERROR) << __func__ << " " << type_id << " " <<  kuzu_value_get_uint64(value);
        return base::Value((int)kuzu_value_get_uint64(value));
      //   break;

      case KUZU_UINT32:
      //   LOG(ERROR) << __func__ << " " << type_id << " " <<  kuzu_value_get_uint32(value);
        return base::Value((int)kuzu_value_get_uint32(value));
      //   break;
    
      case KUZU_UINT16:
      //   LOG(ERROR) << __func__ << " " << type_id << " " <<  kuzu_value_get_uint16(value);
        return base::Value((int)kuzu_value_get_uint16(value));
      //   break;

      case KUZU_UINT8:
      //   LOG(ERROR) << __func__ << " " << type_id << " " <<  kuzu_value_get_uint8(value);
        return base::Value((int)kuzu_value_get_uint8(value));
      //   break;

      case KUZU_DOUBLE:
      //   LOG(ERROR) << __func__ << " " << type_id << " " <<  kuzu_value_get_double(value);
        return base::Value((double)kuzu_value_get_double(value));
      //   break;

      case KUZU_FLOAT:
      //   LOG(ERROR) << __func__ << " " << type_id << " " <<  kuzu_value_get_float(value);
        return base::Value((double)kuzu_value_get_float(value));
      //   break;

      case KUZU_DATE:
        return base::Value((int)kuzu_value_get_date(value).days);
      //   break;

      case KUZU_TIMESTAMP:
        return base::Value((int)kuzu_value_get_timestamp(value).value);
      //   break;

      case KUZU_STRING:
        return base::Value(kuzu_value_get_string(value));
    
      default:
        return base::Value(0);
    }
  }

  base::Value::List KuzuLogicalTypeToBaseValueList(kuzu_data_type_id type_id, raw_ptr<kuzu_value> value){
    switch(type_id)
    {
      case KUZU_INTERVAL:{
        kuzu_interval_t val = kuzu_value_get_interval(value);
      //   LOG(ERROR) << __func__ << " " << type_id << " " << val.months << " " << val.days << " " << val.micros;
        base::Value::List list;
        list.Append(base::Value((int)val.months));
        list.Append(base::Value((int)val.days));
        list.Append(base::Value((int)val.micros));
        return list;
      }
      // break;

      // [[1,2,3,4,5,6],[1,[2,3,3],3,4,5,[8,9,6]]]

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
      // break;

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
      // break;

      case KUZU_INTERNAL_ID:{
        kuzu_internal_id_t val = kuzu_value_get_internal_id(value);
      //   LOG(ERROR) << __func__ << " " << type_id << " " << val.offset << " " << val.table_id;
        base::Value::List list;
        list.Append(base::Value((int)val.offset));
        list.Append(base::Value((int)val.table_id));
        return list;
      }
      default: {
        base::Value::List list;
        return list;
      }
      //   return LogKuzuLogicalType(type_id, value);
      // break;   
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
