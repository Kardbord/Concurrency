#include <Benchmark.h>

namespace Benchmark {

  std::string Result::csv_header() {
    return "operation,map_type,key_type,val_type,shard_count,total_operations,thread_count,avg_operations_per_ms,total_elapsed_ms\n";
  }

  std::string Result::csv_row() const {
    std::stringstream s;
    s << operation << "," << map_type << "," << key_type << "," << val_type << "," << shard_count << "," << total_operations << "," << thread_count << "," << avg_operations_per_ms << ","
      << total_elapsed_ms.count() << "\n";
    return s.str();
  }

  std::string Result::results_to_csv(std::vector<Result> const &results) {
    std::stringstream s;
    s << Result::csv_header();
    for (auto const &r: results) {
      s << r.csv_row();
    }
    return s.str();
  }

} // namespace Benchmark
