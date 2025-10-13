const nfd = require(__dirname + "nfd_bindings.node");

export function openDialog(filters: Record<string, string>): string {
  return nfd.openDialog(filters);
}
