from conan import ConanFile
from conan.tools.build import check_min_cppstd
from conan.tools.cmake import cmake_layout
from conan.tools.files import copy, get
import os


class InstinctCppRecipe(ConanFile):
    name = "instinct_cpp"
    version = "0.1.0"
    settings = "os", "compiler", "build_type", "arch"
    generators = "CMakeDeps", "CMakeToolchain"
    no_copy_source = True
    package_type = "header-library"
    topics = "GenAI", "Agent", "RAG", "LLM", "AI", "header-only"

    def layout(self):
        cmake_layout(self, src_folder="src")

    def validate(self):
        check_min_cppstd(self, 20)

    def build_requirements(self):
        self.tool_requires("cmake/3.27.9")

    def requirements(self):
        self.requires("duckdb/0.10.1", options={"with_httpfs": True})
        self.requires("uriparser/0.9.7")
        self.requires("crossguid/0.2.2")
        self.requires("protobuf/3.21.12")
        self.requires("reactiveplusplus/2.1.1")
        self.requires("icu/74.1", options={"with_extras": True, "data_packaging": "static"})
        self.requires("tsl-ordered-map/1.1.0")
        self.requires("fmt/10.2.1")
        self.requires("fmtlog/2.2.1")
        self.requires("nlohmann_json/3.11.3")
        self.requires("base64/0.5.2")
        self.requires("libcurl/8.6.0")
        self.requires("cpp-httplib/0.15.3")
        self.requires("tesseract/5.3.3")
        self.requires("pdfium/95.0.4629")
        self.requires("duckx/1.2.2")

        # deps of examples
        self.requires("cli11/2.4.1")
        # test deps
        self.test_requires("gtest/1.14.0")

    def build(self):
        pass

    def package(self):
        for sub_module in ["instinct-core", "instinct-llm", "instinct-retrieval", "instinct-server"]:
            print(f"copy {sub_module}")
            copy(self,
                 "*.hpp", src=os.path.join(self.source_folder, "modules", sub_module, "include"),
                 dst=os.path.join(self.package_folder, "include", sub_module))
        copy(self, "LICENSE", src=self.source_folder, dst=os.path.join(self.package_folder, "licenses"))

    def package_info(self):
        # For header-only packages, libdirs and bindirs are not used
        # so it's necessary to set those as empty.
        self.cpp_info.bindirs = []
        self.cpp_info.libdirs = []