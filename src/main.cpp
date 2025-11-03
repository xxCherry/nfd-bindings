#include "nfd.h"
#include <napi.h>
#include <nfd.hpp>

#define ARG_CHECK(arg_count)                                                   \
  if (args.Length() != arg_count) {                                            \
    Napi::Error::New(env, "Wrong number of arguments")                         \
        .ThrowAsJavaScriptException();                                         \
    return env.Null();                                                         \
  }

namespace {

using export_t = Napi::Value(const Napi::CallbackInfo &args);

#if _WIN32
using string_t = std::wstring;
using pchar_t = char16_t;

constexpr int32_t size_multiplier = 2;
#else
using string_t = std::string;
using pchar_t = char;

constexpr int32_t size_multiplier = 1;
#endif

string_t get_platform_string_value(Napi::String str) {
#if _WIN32
  const auto value = str.Utf16Value();

  return std::wstring(value.begin(), value.end());
#else
  return str.Utf8Value();
#endif
}

void export_init() { NFD_Init(); }

Napi::Value open_dialog(const Napi::CallbackInfo &args) {
  const auto env = args.Env();

  ARG_CHECK(1);

  const auto js_filter_items = args[0].As<Napi::Object>();

  std::vector<string_t> storage;
  storage.reserve(
      js_filter_items.GetPropertyNames().As<Napi::Array>().Length() *
      size_multiplier);

  std::vector<nfdnfilteritem_t> filters;
  filters.reserve(storage.capacity() / size_multiplier);

  const auto keys = js_filter_items.GetPropertyNames();
  for (auto i = 0u; i < keys.Length(); ++i) {
    const auto key = keys.Get(i);

    const auto key_val = get_platform_string_value(key.As<Napi::String>());
    const auto val_val =
        get_platform_string_value(js_filter_items.Get(key).As<Napi::String>());

    storage.emplace_back(string_t(key_val.begin(), key_val.end()));
    storage.emplace_back(string_t(val_val.begin(), val_val.end()));

    const auto name_ptr = storage[storage.size() - 2].c_str();
    const auto spec_ptr = storage[storage.size() - 1].c_str();

    filters.push_back(nfdnfilteritem_t{.name = name_ptr, .spec = spec_ptr});
  }

  NFD::UniquePathN out_path;
  const auto result = NFD::OpenDialog(out_path, filters.data(), filters.size());
  if (result == NFD_OKAY) {
    return Napi::String::From(env,
                              reinterpret_cast<pchar_t *>(out_path.get()));
  } else if (result == NFD_CANCEL) {
    return Napi::String::From(env, "");
  } else {
    Napi::Error::New(env, NFD_GetError()).ThrowAsJavaScriptException();
    return env.Null();
  }
}

Napi::Value open_folder_dialog(const Napi::CallbackInfo &args) {
  const auto env = args.Env();

  NFD::UniquePathN out_path;
  const auto result = NFD::PickFolder(out_path);
  if (result == NFD_OKAY) {
    return Napi::String::From(env,
                              reinterpret_cast<pchar_t *>(out_path.get()));
  } else if (result == NFD_CANCEL) {
    return Napi::String::From(env, "");
  } else {
    Napi::Error::New(env, NFD_GetError()).ThrowAsJavaScriptException();
    return env.Null();
  }
}

} // namespace

Napi::Object register_exports(Napi::Env env, Napi::Object exports) {
  export_init();

  const auto register_export = [&](std::string name, export_t export_fn) {
    exports.Set(Napi::String::New(env, name),
                Napi::Function::New(env, export_fn));
  };

  register_export("openDialog", open_dialog);
  register_export("openFolderDialog", open_folder_dialog);

  return exports;
}

NODE_API_MODULE(nfd, register_exports);