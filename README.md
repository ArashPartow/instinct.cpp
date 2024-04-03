# instinct.cpp

`instinct.cpp` is a framework for developing AI Agent applications powered by language models.

[![Discord](https://img.shields.io/badge/Discord%20Chat-purple?style=flat-square&logo=discord&logoColor=white&link=https%3A%2F%2Fdiscord.gg%2F5cVnVyh3)](https://discord.gg/5cVnVyh3)   [![C++ 20](https://img.shields.io/badge/C%2B%2B-20-blue?style=flat-square&link=https%3A%2F%2Fen.wikipedia.org%2Fwiki%2FC%252B%252B20)](https://en.wikipedia.org/wiki/C%2B%2B20)    [![License](https://img.shields.io/badge/Apache%20License-2.0-green?style=flat-square&logo=Apache&link=.%2FLICENSE)](./LICENSE)


**🚨 This project is under active development and has not reached to GA stage of first major release. See more at [Roadmap section](#roadmap).**

## Features

What `instinct.cpp` offer:

* Integration for privacy-first LLM providers: Built-in support for [Ollama](https://ollama.com/) and other OpenAI compatible API services like [nitro](https://nitro.jan.ai/) and more.
* Building blocks for common application patterns like RAG, Function-calling agents.
* Programing paradigms: `Chain` for composable LLM pipelines and `Actor` for Agent execution flows.
* Embeddable source files: Header only library, which means less binary footprints on projects dependent on it. 
* Multi-platform compatability.

What `instinct.cpp` cannot offer:

* A C++ version of `langchain`. While this project learns a lot from `langchain` including Prompt related classes and functional API designs, `instinct.cpp` will focus on opinionated components while providing extensive interfaces for vendor specific implementations. For example, there are tons of vector database integration available in `langchain`, but `instinct.cpp` will keep [DuckDB](https://duckdb.org/) implementation for embedded scenario and both [Weaviate](https://github.com/weaviate/weaviate) and [milvus](https://milvus.io/) client integration for cloud scenario.
* Chatbot UI or so: `instinct.cpp` will not be a full-stack solution including UI components. While there is a selected collection of examples for this framework, they will remain in the flavour of `CLI`.

## Getting started

### Install from conan center

WIP

### Build from sources

System Requirements: 

* CMake 3.26+
* Compiler that supports C++ 20: GCC 13+ or Clang 15+
* Conan 2+

This project relies on [conan](https://conan.io/) to resolve dependencies. To build and install:

```shell
mkdir build && cd build
conan install .. --build=missing
cmake .. && cmake --build . --target INSTALL --preset conan-release
```

### Quick start

```c++

```

### What's next

You can learn more about this frameworks by following links below:

* [Chaining](docs/chaining.md)
* [Built-in components](docs/components.md) 
* Reference apps
  * [doc-agent](./modules/instinct-examples/doc-agent) : Chat with your doc locally with privacy.
  * assistant-agent
  * db-agent


## Roadmap

| Milestone                                                     | Features                                                                                 | DDL  |
|---------------------------------------------------------------|------------------------------------------------------------------------------------------|------|
| v0.1.0                                                        | Long-short memory, PDF/TXT/DOCX ingestor, RAG reference app, `Chain` programing paradigm | 3.29 |
| [v0.1.1](https://github.com/RobinQu/instinct.cpp/milestone/1) | Performance tuning, RAG evaluation, external knowledge retriever, ...                    | 4.12 |
| [v0.1.2](https://github.com/RobinQu/instinct.cpp/milestone/2) | Function calling, `Actor` programing paradigm and reference app                          | 4.30 |
| v0.1.3                                                        | Multi agent support and reference app                                                    | 5.24 |
| v0.2.0                                                        | Features will be frozen. More benchmarks and documentations.                             | 5.31 |


Contributions are welcomed! You can join [discord server](https://discord.gg/5cVnVyh3), or contact me via [email](mailto:robinqu@gmail.com).