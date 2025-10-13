const nfd = require("./dist/module.node");

export function openDialog(filters: Record<string, string>): string {
  return nfd.openDialog(filters);
}
