#include <tvm/relay/attrs/nn.h>
#include <tvm/relay/expr_functor.h>
#include <tvm/relay/transform.h>
#include <tvm/relay/type.h>
#include <tvm/runtime/module.h>
#include <tvm/runtime/registry.h>

#include <fstream>
#include <iostream>
#include <numeric>
#include <sstream>

#include "../../utils.h"

#include "../../../../runtime/contrib/json/json_node.h"
#include "../codegen_json/codegen_json.h"

namespace tvm {
namespace relay {
namespace contrib {

using namespace backend;

class ILAJSONSerializer : public backend::contrib::JSONSerializer {
  using JSONGraphNode = tvm::runtime::json::JSONGraphNode;
  using JSONGraphNodeEntry = tvm::runtime::json::JSONGraphNodeEntry;

 public:
  ILAJSONSerializer(const std::string& symbol, const Expr& expr)
      : JSONSerializer(symbol, expr) {}

  std::vector<JSONGraphNodeEntry> VisitExpr_(const CallNode* cn) override {
    Expr expr = GetRef<Expr>(cn);
    std::string name;
    // const CallNode* call = cn;
    if (const auto* op_node = cn->op.as<OpNode>()) {
      name = op_node->name;
    } else if (const auto* fn = cn->op.as<FunctionNode>()) {
      auto comp = fn->GetAttr<String>(attr::kComposite);
      CHECK(comp.defined())
          << "JSON runtime only supports composite functions.";
      name = comp.value();

      if (name == "ila.conv2d") {
        // TODO
      } else if (name == "ila.batch_matmul") {
        // TODO
      } else {
        LOG(FATAL) << "Unrecognized pattern: " << name;
      }
    } else {
      LOG(FATAL) << "ILA JSON runtime does not support calls to "
                 << cn->op->GetTypeKey();
    }
    LOG(INFO) << "Visiting " << name;

    std::vector<JSONGraphNodeEntry> inputs;
    for (const auto& arg : cn->args) {
      auto res = VisitExpr(arg);
      inputs.insert(inputs.end(), res.begin(), res.end());
    }
    auto node = std::make_shared<JSONGraphNode>(name,     /* name_ */
                                                "kernel", /* op_type_ */
                                                inputs, 1 /* num_outputs_ */);
    // SetCallNodeAttribute(node, call);
    return AddNode(node, GetRef<Expr>(cn));
  }

};  // class ILAJSONSerializer

runtime::Module ILACompiler(const ObjectRef& ref) {
  CHECK(ref->IsInstance<FunctionNode>());
  auto func = Downcast<Function>(ref);
  auto func_name = GetExtSymbol(func);

  ILAJSONSerializer serializer(func_name, func);
  serializer.serialize();
  std::string graph_json = serializer.GetJSON();
  auto params = serializer.GetParams();

  const auto* pf = runtime::Registry::Get("runtime.ILARuntimeCreate");
  CHECK(pf != nullptr) << "Cannot find ILA runtime module to create";
  auto mod = (*pf)(func_name, graph_json, params);
  return mod;
}

TVM_REGISTER_GLOBAL("relay.ext.ila").set_body_typed(ILACompiler);

}  // namespace contrib
}  // namespace relay
}  // namespace tvm
