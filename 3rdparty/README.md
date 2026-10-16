# 3rdparty libraries

| lib                                                                                      | branch | tag    |
| ---------------------------------------------------------------------------------------- | ------ | ------ |
| [Protobuf](https://github.com/protocolbuffers/protobuf)                                |        | vcpkg baseline |
| [arknet](https://github.com/OpenArkStudio/arknet)                                       |        | v0.1.0 |
| [RapidXML](https://rapidxml.sourceforge.net/)                                          |        | 1.13   |
| [toml++](https://github.com/marzer/tomlplusplus) (application manifests, MIT)             |        | v3.4.0 fallback |
| [args](https://github.com/Taywee/args/releases/tag/6.2.2)                                |        | 6.2.2  |
| [spdlog](https://github.com/gabime/spdlog/releases/tag/v1.17.0)                          | v1.x   | v1.17.0 |
| [json](https://github.com/nlohmann/json/releases/download/v3.9.1/json.hpp)               | v3.9.1 |        |
| [cpp-bredis](https://github.com/basiliscos/cpp-bredis/releases/tag/v0.11)                |        | v0.11  |
| [readerwriterqueue](https://github.com/cameron314/readerwriterqueue/releases/tag/v1.0.3) |        | v1.0.3 |
| [librdkafka](https://github.com/edenhill/librdkafka/releases/tag/v1.6.0)                 |        | v1.6.0 |
| [mysqlpp](https://tangentsoft.com/mysqlpp/home)                                          |        | v3.2.5 |
| [doctest](https://github.com/doctest/doctest) (network tests only)                       |        | 2.5.3  |
| [Standalone Asio](https://github.com/chriskohlhoff/asio)                                |        | 1.38.2 |
| [Boost.Asio/Beast](https://www.boost.org/) (optional backend)                           |        | vcpkg baseline |

arknet is maintained in its own repository and acquired by CMake at the release
tag. RapidXML is supplied by the package manager; the unavailable ArkNX fork is
no longer a submodule. The vcpkg manifest pins the package baseline. See
[network builds and regression tests](../docs/networking.md).

The optional `ark::config` target privately uses toml++ under its
[MIT license](https://github.com/marzer/tomlplusplus/blob/v3.4.0/LICENSE).
CMake prefers an installed `tomlplusplus` 3.4+ package and otherwise fetches commit
`30172438cee64926dc41fdd9c11fb3ba5b2ba9de` (v3.4.0); no parser source is vendored.
See [application manifests](../docs/service-manifests.md) for the configuration API.

### toml++ license notice

```text
MIT License

Copyright (c) Mark Gillard <mark.gillard@outlook.com.au>

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated
documentation files (the "Software"), to deal in the Software without restriction, including without limitation the
rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to
permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the
Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR
OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
```
