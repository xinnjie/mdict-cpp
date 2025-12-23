// swift-tools-version: 6.0
import PackageDescription

let package = Package(
  name: "mdict-cpp",
  platforms: [
    .iOS(.v13),
    .macOS(.v10_15),
  ],
  products: [
    .library(
      name: "MdictSwift",
      targets: ["MdictSwift"])
  ],
  targets: [
    // Dependencies
    .target(
      name: "MdictMiniz",
      path: "deps/miniz",
      exclude: [
        "examples", "tests", "amalgamate.sh", "test.sh", "CMakeLists.txt", "ChangeLog.md",
        "LICENSE", "readme.md", ".clang-format", ".travis.yml",
      ],
      sources: ["miniz.c", "miniz_zip.c", "miniz_tinfl.c", "miniz_tdef.c"],
      publicHeadersPath: ".",
      cSettings: [
        .define("MINIZ_NO_STDIO", to: nil),
        .define("_LARGEFILE64_SOURCE", to: "1"),
      ]
    ),
    .target(
      name: "MdictMinilzo",
      path: "deps/minilzo",
      exclude: ["testmini.c", "Makefile", "README.LZO", "COPYING", "CMakeLists.txt"],
      sources: ["minilzo.c"],
      publicHeadersPath: "."
    ),
    .target(
      name: "MdictTurbobase64",
      path: "deps/turbobase64",
      exclude: [
        "cmake", "rust", "vs", "tb64app.c", "tb64app", "makefile", "CMakeLists.txt", "LICENSE",
        "README.md", "time_.h",
      ],
      sources: ["turbob64c.c", "turbob64d.c", "turbob64v128.c"],
      publicHeadersPath: ".",
      cSettings: [
        .define("NAVX2"),
        .define("NAVX512"),
      ]
    ),

    // Main Target
    .target(
      name: "mdict",
      dependencies: ["MdictMiniz", "MdictMinilzo", "MdictTurbobase64"],
      path: "src",
      exclude: ["mydict.cc"],
      sources: [
        "mdict.cc",
        "mdict_extern.cc",
        "binutils.cc",
        "adler32.cc",
        "ripemd128.c",
      ],
      publicHeadersPath: "include",
      cxxSettings: [
        .headerSearchPath("."),  // For internal includes
        .headerSearchPath("../deps"),  // For deps includes like "miniz/miniz.h"
        .define("MDICT_USE_MINIZ"),
      ]
    ),

    // Swift Wrapper
    .target(
      name: "MdictSwift",
      dependencies: ["mdict"],
      path: "Sources/MdictSwift"
    ),

    // Tests
    .testTarget(
      name: "MdictSwiftTests",
      dependencies: [
        "MdictSwift"
      ],
      path: "Tests/MdictSwiftTests",
      resources: [
        .copy("testdict.mdx")
      ]
    ),
  ],
  cxxLanguageStandard: .cxx17
)
