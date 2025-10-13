let nfd = require("./dist/nfd_bindings.node");
if (!nfd) {
  nfd = require("./dist/Release/nfd_bindings.node");
}

export function openDialog(filters: Record<string, string>): string {
  return nfd.openDialog(filters);
}
