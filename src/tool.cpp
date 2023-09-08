#include "tool.hpp"

namespace toy2d {

    std::string ReadWholeFile(const std::string& filename) {
        std::ifstream file(filename, std::ios::binary | std::ios::ate);

        if (!file.is_open()) {
            std::cout << "read " << filename << " failed" << std::endl;
            return std::string{};
        }
        else {
            std::cout << "success load " << filename <<std::endl;
        }

        auto size = file.tellg();
        std::cout << size << std::endl;
        std::string content;
        content.resize(size);
        file.seekg(0);

        file.read(content.data(), content.size());


        return content;
    }

}