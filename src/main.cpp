#include <napi.h>

#define ARG_CHECK(arg_count)                                                   \
  if (args.Length() != 0) {                                                    \
    Napi::Error::New(env, "Wrong number of arguments")                         \
        .ThrowAsJavaScriptException();                                         \
    return env.Null();                                                         \
  }

namespace {

using ExportT = Napi::Value(const Napi::CallbackInfo &args);

Napi::Value hello(const Napi::CallbackInfo &args) {
  Napi::Env env = args.Env();

  ARG_CHECK(0);

  return Napi::String::From(env, "hi!");
}

} // namespace

Napi::Object register_exports(Napi::Env env, Napi::Object exports) {
  const auto register_export = [&](std::string name, ExportT export_fn) {
    exports.Set(Napi::String::New(env, name),
                Napi::Function::New(env, export_fn));
  };

  register_export("hello", hello);

  return exports;
}

NODE_API_MODULE(nfd, register_exports);