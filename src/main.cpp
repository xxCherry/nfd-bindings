#include <napi.h>
#include <nfd.hpp>

#define ARG_CHECK(arg_count)                                                   \
  if (args.Length() != arg_count) {                                            \
    Napi::Error::New(env, "Wrong number of arguments")                         \
        .ThrowAsJavaScriptException();                                         \
    return env.Null();                                                         \
  }

namespace {

using ExportT = Napi::Value(const Napi::CallbackInfo &args);

void export_init() { NFD_Init(); }

Napi::Value open_dialog(const Napi::CallbackInfo &args) {
  const auto env = args.Env();

  ARG_CHECK(1);

  const auto js_filter_items = args[0].As<Napi::Object>();

  std::vector<std::wstring> storage;
  storage.reserve(
      js_filter_items.GetPropertyNames().As<Napi::Array>().Length() * 2);

  std::vector<nfdnfilteritem_t> filters;
  filters.reserve(storage.capacity() / 2);

  const auto keys = js_filter_items.GetPropertyNames();
  for (auto i = 0u; i < keys.Length(); ++i) {
    const auto key = keys.Get(i);

    const auto key_u16 = key.As<Napi::String>().Utf16Value();
    const auto val_u16 =
        js_filter_items.Get(key).As<Napi::String>().Utf16Value();

    storage.emplace_back(std::wstring(key_u16.begin(), key_u16.end()));
    storage.emplace_back(std::wstring(val_u16.begin(), val_u16.end()));

    const auto name_ptr = storage[storage.size() - 2].c_str();
    const auto spec_ptr = storage[storage.size() - 1].c_str();

    filters.push_back(nfdnfilteritem_t{.name = name_ptr, .spec = spec_ptr});
  }

  NFD::UniquePathN out_path;
  const auto result = NFD::OpenDialog(out_path, filters.data(), filters.size());
  if (result == NFD_OKAY) {
    return Napi::String::From(env,
                              reinterpret_cast<char16_t *>(out_path.get()));
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
                              reinterpret_cast<char16_t *>(out_path.get()));
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

  const auto register_export = [&](std::string name, ExportT export_fn) {
    exports.Set(Napi::String::New(env, name),
                Napi::Function::New(env, export_fn));
  };

  register_export("openDialog", open_dialog);
  register_export("openFolderDialog", open_folder_dialog);

  return exports;
}

NODE_API_MODULE(nfd, register_exports);