#include <tvm/runtime/ndarray.h>
#include <tvm/runtime/registry.h>

#include <cstddef>
#include <iostream>
#include <string>
#include <vector>

#include "../json/json_node.h"
#include "../json/json_runtime.h"

namespace tvm {
namespace runtime {
namespace contrib {

using namespace tvm::runtime;
using namespace tvm::runtime::json;

class ILARuntime : public JSONRuntimeBase {
 public:
  ILARuntime(const std::string& symbol_name, const std::string& graph_json,
             const Array<String> const_names)
      : JSONRuntimeBase(symbol_name, graph_json, const_names) {}

  const char* type_key() const { return "ila"; }

  void Init(const Array<NDArray>& consts) override {
    // TODO
  }

  void Run() override {
    // TODO
  }

 protected:
 private:
};  // class ILARuntime

runtime::Module ILARuntimeCreate(String symbol_name, String graph_json,
                                 const Array<String>& const_names) {
  auto n = make_object<ILARuntime>(symbol_name, graph_json, const_names);
  return runtime::Module(n);
}

TVM_REGISTER_GLOBAL("runtime.ILARuntimeCreate")
    .set_body_typed(ILARuntimeCreate);

TVM_REGISTER_GLOBAL("runtime.module.loadbinary_ila")
    .set_body_typed(JSONRuntimeBase::LoadFromBinary<ILARuntime>);

}  // namespace contrib
}  // namespace runtime
}  // namespace tvm
