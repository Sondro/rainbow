// Copyright (c) 2010-present Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

import * as fs from "fs";
import * as path from "path";
import { makeBanner, makeSafeName } from "./import-asset";

const shadersPath = path.resolve(__dirname, "..", "src", "Graphics", "Shaders");
const shaders = fs
  .readdirSync(shadersPath)
  .filter(isShader)
  .sort();

const copyright = makeBanner(__filename);

fs.writeFile(
  path.join(shadersPath, "..", "Shaders.cpp"),
  [
    copyright,
    "",
    '#include "Graphics/Shaders.h"',
    "",
    "// clang-format off",
    "",
    "namespace",
    "{",
    shaders
      .map(file =>
        [
          `    constexpr char k${makeSafeName(file)}[] =`,
          ...fs
            .readFileSync(path.join(shadersPath, file))
            .toString()
            .split("\n")
            .filter(isCodeLine)
            .map(line => {
              const trimmedLine = line.trimLeft();
              const indent = " ".repeat(line.length - trimmedLine.length);
              return `        ${indent}"${removeComments(trimmedLine)}\\n"`;
            })
        ].join("\n")
      )
      .join(";\n\n") + ";",
    "}  // namespace",
    "",
    ...shaders.map(file => {
      const isHeader = isShaderHeader(file);
      const name = makeSafeName(file);
      const [returnType, returnValue] = isHeader
        ? ["rainbow::czstring", `k${name}`]
        : [
            "Shader::Params",
            `{${inferShaderType(file)}, 0, "Shaders/${file}", k${name}}`
          ];
      return [
        `auto rainbow::gl::${name}() -> ${returnType}`,
        "{",
        `    return ${returnValue};`,
        "}",
        ""
      ].join("\n");
    }),
    "// clang-format on",
    ""
  ].join("\n"),
  { mode: 0o644 },
  () => void 0
);

fs.writeFile(
  path.join(shadersPath, "..", "Shaders.h"),
  [
    copyright,
    "",
    '#include "Common/String.h"',
    '#include "Graphics/ShaderDetails.h"',
    "",
    "namespace rainbow::gl",
    "{",
    ...shaders.map(
      file =>
        isShaderHeader(file)
          ? `    auto ${makeSafeName(file)}() -> czstring;`
          : `    auto ${makeSafeName(file)}() -> Shader::Params;`
    ),
    "}  // namespace rainbow::gl",
    ""
  ].join("\n"),
  { mode: 0o644 },
  () => void 0
);

function inferShaderType(file: string): string {
  switch (path.extname(file)) {
    case ".frag":
      return "Shader::kTypeFragment";
    case ".vert":
      return "Shader::kTypeVertex";
    default:
      throw new Error(`Unknown shader: ${file}`);
  }
}

function isCodeLine(line: string): boolean {
  const trimmedLine = line.trim();
  return Boolean(trimmedLine) && !trimmedLine.startsWith("//");
}

function isShader(file: string): boolean {
  return [".glsl", ".frag", ".vert"].includes(path.extname(file));
}

function isShaderHeader(file: string): boolean {
  return path.extname(file) === ".glsl";
}

function removeComments(line: string): string {
  return line.replace(/\s+\/\/.*/g, "");
}
