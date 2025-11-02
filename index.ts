let binding: any;

if (process.platform == "win32") {
  try {
    binding = require("./dist/nfd_bindings.node");
  } catch {
    binding = require("./dist/Release/nfd_bindings.node");
  }
} else if (process.platform == "linux") {
    binding = require("./dist/libnfd_bindings.node");
} else {
  throw new Error("Unsupported platform");
}

export function openDialog(filters: Record<string, string>): string {
  return binding.openDialog(filters);
}

export function openFolderDialog(): string {
  return binding.openFolderDialog();
}
