# Frequently asked questions

## Does ark require C++20?

Yes. CMake requests C++20 and disables compiler extensions. See [installation](../get-started/installation.md).

## Is arknet part of this repository?

arknet is an independent header-only networking project. ark consumes its pinned release; `ARKNET_SOURCE_DIR` selects a local checkout for development.

## Do the network tests require Python or Ruby?

No. They compile as permanent C++ doctest programs. The separate Excel configuration generator has its own requirements.

## Have 100,000–200,000 simultaneous connections been validated?

That is the multi-process deployment target. Local correctness tests do not establish that capacity; see [acceptance scope](../performance/benchmark.md).
