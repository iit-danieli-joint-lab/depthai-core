#include "Common.hpp"
#include "NodeBindings.hpp"
#include "depthai/pipeline/ThreadedHostNode.hpp"
#include "depthai/pipeline/node/host/HostNode.hpp"

#include <pybind11/eval.h>

extern py::handle daiNodeModule;

using namespace dai;
using namespace dai::node;

class PyThreadedHostNode : public NodeCRTP<ThreadedHostNode, PyThreadedHostNode> {
   public:
    void run() override {
        PYBIND11_OVERRIDE_PURE(void, ThreadedHostNode, run);
    }
};

class PyHostNode : public NodeCRTP<HostNode, PyHostNode> {
   public:
    std::shared_ptr<Buffer> _process(std::shared_ptr<dai::MessageGroup> in) override {
        PYBIND11_OVERRIDE_PURE(std::shared_ptr<Buffer>, HostNode, _process, in);
    }
};

void bind_hostnode(pybind11::module& m, void* pCallstack){
    // declare upfront
    auto threadedHostNode =
        py::class_<ThreadedHostNode, PyThreadedHostNode, ThreadedNode, std::shared_ptr<ThreadedHostNode>>(daiNodeModule, "ThreadedHostNode", DOC(dai, node, ThreadedHostNode));
    auto hostNode = py::class_<HostNode, PyHostNode, ThreadedHostNode, std::shared_ptr<HostNode>>(daiNodeModule, "HostNode", DOC(dai, node, HostNode));

    ///////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////
    // Call the rest of the type defines, then perform the actual bindings
    Callstack* callstack = (Callstack*) pCallstack;
    auto cb = callstack->top();
    callstack->pop();
    cb(m, pCallstack);
    // Actual bindings
    ///////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////

    threadedHostNode.def(py::init<>()).def("run", &ThreadedHostNode::run);

    hostNode.def(py::init<>())
        .def("_process", &HostNode::_process)
        .def_property_readonly(
            "inputs", [](HostNode& node) { return &node.inputs; }, py::return_value_policy::reference_internal)
        .def("runSyncingOnHost", &HostNode::runSyncingOnHost, DOC(dai, node, HostNode, runSyncingOnHost))
        .def("runSyncingOnDevice", &HostNode::runSyncingOnDevice, DOC(dai, node, HostNode, runSyncingOnDevice));

    py::exec(R"(
        import inspect
        import typing
        def __init_subclass__(cls):
            members = dict(inspect.getmembers(cls))
            assert "process" in members, "Subclass of HostNode must define method 'process'"
            sig = inspect.signature(members["process"])
            assert list(sig.parameters.keys())[0] == "self", \
                'Please use "self" as the first parameter for process method'

            cls.input_desc = {}
            for name, param in sig.parameters.items():
                if name == "self": continue
                annotation = param.annotation
                if annotation == inspect.Parameter.empty:
                    annotation = typing.Any
                cls.input_desc[name] = annotation

            cls.output_desc = sig.return_annotation
            if cls.output_desc == inspect.Signature.empty:
                cls.output_desc = None

            def _process(self, messageGroup):
                return members["process"](self, 
                    *(messageGroup[argname] for argname in cls.input_desc.keys()))
            cls._process = _process

            def build(self, *args):
                for name, arg in zip(cls.input_desc, args):
                    arg.link(self.inputs[name])
                return self
            cls.build = build

        HostNode.__init_subclass__ = classmethod(__init_subclass__)
    )", daiNodeModule.attr("__dict__"));
}
