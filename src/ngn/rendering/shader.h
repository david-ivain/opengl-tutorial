#pragma once

#include <string>

namespace ngn {
class Shader {
public:
    Shader(const std::string& vertex_path, const std::string& fragment_path);
    ~Shader();

    Shader(Shader&&) = delete;
    Shader(const Shader&) = delete;
    Shader& operator=(const Shader&) = delete;

    void use() const;
    /**
     * @brief Sets the value of a given uniform for this shader.
     */
    template <class T>
    void set(const std::string& name, T value) const;

private:
    const unsigned ID_;
};
}
