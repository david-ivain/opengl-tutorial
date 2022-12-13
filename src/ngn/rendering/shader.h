#pragma once

#include <string>

namespace ngn {
class Shader {
public:
    Shader(const std::string& vertex_path, const std::string& fragment_path);
    ~Shader();

    void use() const;
    template <class T>
    void set(const std::string& name, T value) const;

private:
    const unsigned ID_;
};
}
