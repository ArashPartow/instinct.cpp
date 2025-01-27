# Changelog

## v0.1.5

**Full Changelog**: https://github.com/RobinQu/instinct.cpp/commits/v0.1.5

* Features
  * `instinct-transformer`: New bge-m3 embedding model. Generally speaking, bge-reranker and bge-embedding are still in preview as they are not fast enough for production.
  * `instinct-llm`: New `JinaRerankerModel` for Reranker model API from Jina.ai.
  * `instinct-retrieval`: New `DuckDBBM25Retriever` for BM25 keyword based retriever using DuckDB's built-in function. 
* Improvements
  * Move example code to standalone repository: [instinct-cpp-examples](https://github.com/RobinQu/instinct-cpp-examples). 
  * Rename for all files for camel-case naming conventions 
  * Build system:
    * Fix include paths for internal header files. Now all files are referenced using angle bracket pattern like `#include <instinct/...>`. 
    * Rewrite Cmake install rules. 
    * Run unit tests during `conan build` using `Ctest`.
  * `doc-agent`:
    * Use `retriver-version` argument in CLI to control how retriever related components are constructed.
    * Rewrite lifecycle control using application context
  * `instinct-retrieval`: Fix RAG evaluation. RAG pipeline with MultiPathRetriever should get score more than 80%.

## v0.1.4

**Full Changelog**: https://github.com/RobinQu/instinct.cpp/commits/v0.1.4

* Features
  * `instinct-assistant`: `file-search` support. It takes a summary guided strategy inspired by [RAPTOR](https://arxiv.org/abs/2401.18059), and follows practices adopted by OpenAI. See more at [#16
    ](https://github.com/users/RobinQu/projects/1?pane=issue&itemId=58554735). It has some limitations, please refer to [#22](https://github.com/users/RobinQu/projects/1/views/1?pane=issue&itemId=67421127).
  * new `instinct-transfomer` module that ships built-in models based on transformer. Currently, only [BGE-M3-Reranker](https://huggingface.co/BAAI/bge-reranker-v2-m3/tree/main) is implemented. Most code in first version implementation is refined (copied) from [chatllm.cpp](https://github.com/foldl/chatllm.cpp) for quick start.
  * `instinct-retrieval`: 
    * a new `MultiPathRetriever` that handle recalls from multiple child retrievers and score results with a reranker model.
    * `SimpleRetrieverOperator` and `DuckDBVectorStoreOpeartor` for multi-instance managements.
    * `CitationAnnotatingChain` and `SummaryChain` to support citations and summary required in `instinct-assistant`.
    * Added a mandatory `file-source` metadata field to keep record of file identifier.
  * `instinct-data`:
    * `Aggregate` method on data template class
    * Added `GetObjectState` method on `IObjectStore`.
  * `instinct-core`:
    * Added `LambdaInputParsr` and `LambdaOutputParser` to simplify chain implementation.  
    * `ManagedApplicationContext` to have more delicate lifecycle managements.
* Improvements
  * `mini-assistant`:
    * many bug fixes in related classes.
    * CLI options: we can now assign model provider separately for embedding model and chat model.
  * `instinct-core`:
    * Make options on chat model and embedding model optional using `std::optional`.
    * Fix a consistency issue of `RecursiveCharacterTextSplitter` with the one in `langchain`.
    * Better control of preloading assets of tokenizers.
  * `instinct-server`:
    * fix graceful shutdown of `HttpLibServer`.
  * bump `duckdb` to `0.10.2`

```text
8bde9376fa21401cba17e3ae7a094f07fa3e11293baf2cd729a37e3e9eec0868  doc-agent_linux-amd64.tar.gz
8fad76f8c402240189817b221442b7a596b927653e36ae61d431d1b0cd3776ef  doc-agent_macos-arm64.tar.gz
8042950bc8a28df375e58589423d87c77bae545d4f71d5acf1e224932cfae32f  mini-assistant_linux-amd64.tar.gz
848d1e7a4a2b80c42a7348d37d0337788e67f13d745acbed3652d65b964164c8  mini-assistant_macos-arm64.tar.gz
```


## v0.1.3

**Full Changelog**: https://github.com/RobinQu/instinct.cpp/commits/v0.1.3

Sorry for the delay. Hacking with new agent executor costs me more time than expected. But it's fun and enlightening. As a result, work on rerank models will delay to next release.

* Features
  * New `LLMCompilerAgentExecutor` as default executor in `mini-assistant`. Please see [this notebook](https://github.com/RobinQu/instinct-notebook) for evaluation results.
  * `POST /v1/threads/runs`, which is missed in last released endpoint, is added.
  * `xn::steps::branch` and `BranchStepFunction` for binary branch composing.
* Improvements
  * Migrate `instinct-lab` to [standalone repo](https://github.com/RobinQu/instinct-notebook/). Benchmarks and experimental code will be moved there.
  * Many typos in `mini-assistant` API endpoints.
  * Add more trace logs in debug mode.
  * `BaseChatModel` and `BaseLLM` supports runtime configuration with `configure(const ModelOverrides&)` method.
  * `HttpLibServer` defaults to listen `0.0.0.0`. 


## v0.1.2

**Full Changelog**: https://github.com/RobinQu/instinct.cpp/commits/v0.1.2


* Features
    * First release of `mini-assistant` binary, which provides OpenAI compatible Assistant API services.
    * instinct.cpp library:
        * `instinct-core` module
            * `ChronoUtils` for datetime related functions.
            * `RandomUtils` for random number generation.
            * `SnowflakeIDGenerator` for unique ID generation.
        * `instinct-data` module
            * Data mapper classes that is built for simple ORM, with protobuf message.
            * Task scheduler abstraction and in-process task queue implementation.
            * Object store abstraction and local filesystem implementation.
        * `instinct-llm` module
            * `OpenAIToolAgentExecutor`: Agent executor based on OpenAI function calling API.
        * `insitinct-server` module
            * Error controller for `HttpLibServer` and `DefaultErrorController` implementation.
            * `HttpLibSession` struct for route handler.
* Improvements
    * `instinct-llm` module
        * function call protocol is updated to match OpenAI's standard.
        * `ILanguageModel` has `BindTools` function to support function calling API.
    * `instinct-core` module
        * Exceptions are now with stacktrace generated by `cpptrace`. 
        * assertion errors now throw `ClientException` which is sublcass of `InstinctException`.


```text
cff7ddc64c37aa1fbe7a772a99ab6461f1d8cd93f802897d586586cc74b1f45d  mini-assistant_macos-arm64.tar.gz
73d265ea05aa52d2bd1325c21965ad2accb629fbe218a8ba2f240b88f4a19967  mini-assistant_linux-amd64.tar.gz
872157a97a25551ed4d5768f64018aa38f6a7e3b9516286e2592687744e32bf2  doc-agent_linux-amd64.tar.gz
79c4c7b178bafa560a4a8f7721d8f8361f35e624240972d793577687ecb49368  doc-agent_macos-arm64.tar.gz
```


## v0.1.1

**Full Changelog**: https://github.com/RobinQu/instinct.cpp/commits/v0.1.1

* Features
  * Initial agent frameworks. See `IAgentExecutor`.
  * Function calling: ReACT style tool agent is implemented as intial support for LLM agent. See [TestReactAgent](https://github.com/RobinQu/instinct.cpp/blob/189224e00077777d0cbafb941f564d233adc8ffd/modules/instinct-agent/test/agent/react/TestReACTAgent.cpp).
  * Ingestor:
    * `ParquetFileIngestor` for reading Apache Arrow parquet format.
  * Utitlies: 
    * `HashUtils` for MD5, SHA1, SHA256 hash functions. 
    * `IOUtils`, `SystemUtils` are added.  
    * `ExecuteWithCallback` and `ExecuteBatch` are added to `IHttpClient.hpp`.
    * `IFileVault` and `FileSystemFileVault` for advanced file resource management.
* Improvements
  * bugfixes in `RecursiveCharacterTextSplitter`, resulting  better recall in RAG.
  * Rewrite HTTP server classes with more modular controller design.
  * Code cleanup for DuckDB store classes.
  * Adapt `rag_evaulation.ipynb` for Ollama and vLLM servers. Use mixtral, all-minillm as default models.
* Known issues
  * CI is not ready and some tests relies on local setup.  You may experience problems for building and running on your machines.
  * Large PDF and long text are not tested with current `doc_agent` implementation.