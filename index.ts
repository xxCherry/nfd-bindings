let nfd: any;

try {
  nfd = require("./dist/nfd_bindings.node");
} catch {
  nfd = require("./dist/Release/nfd_bindings.node");
}

export function openDialog(filters: Record<string, string>): string {
  return nfd.openDialog(filters);
}

export function openFolderDialog(): string {
  return nfd.openFolderDialog();
}
