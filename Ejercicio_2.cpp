#include <iostream>
#include <memory>

class Window {
public:
    virtual ~Window() = default;
    virtual void draw() = 0;
};
class Button {
public:
    virtual ~Button() = default;
    virtual void draw() = 0;
};
class Shader {
public:
    virtual ~Shader() = default;
    virtual void compile() = 0;
};

class RenderFactory {
public:
    virtual ~RenderFactory() = default;
    virtual std::unique_ptr<Window> createWindow() = 0;
    virtual std::unique_ptr<Button> createButton() = 0;
    virtual std::unique_ptr<Shader> createShader() = 0;
};

class OpenGLWindow : public Window {
public:
    void draw() override { std::cout << "Dibujando Window en OpenGL\n"; }
};
class OpenGLButton : public Button {
public:
    void draw() override { std::cout << "Dibujando Button en OpenGL\n"; }
};
class OpenGLShader : public Shader {
public:
    void compile() override { std::cout << "Compilando Shader en OpenGL\n"; }
};

class VulkanWindow : public Window {
public:
    void draw() override { std::cout << "Dibujando Window en Vulkan\n"; }
};
class VulkanButton : public Button {
public:
    void draw() override { std::cout << "Dibujando Button en Vulkan\n"; }
};
class VulkanShader : public Shader {
public:
    void compile() override { std::cout << "Compilando Shader en Vulkan\n"; }
};

class OpenGLFactory : public RenderFactory {
public:
    std::unique_ptr<Window> createWindow() override { return std::make_unique<OpenGLWindow>(); }
    std::unique_ptr<Button> createButton() override { return std::make_unique<OpenGLButton>(); }
    std::unique_ptr<Shader> createShader() override { return std::make_unique<OpenGLShader>(); }
};
class VulkanFactory : public RenderFactory {
public:
    std::unique_ptr<Window> createWindow() override { return std::make_unique<VulkanWindow>(); }
    std::unique_ptr<Button> createButton() override { return std::make_unique<VulkanButton>(); }
    std::unique_ptr<Shader> createShader() override { return std::make_unique<VulkanShader>(); }
};

int main() {
    std::unique_ptr<RenderFactory> factory =
    std::make_unique<OpenGLFactory>();
    auto window = factory->createWindow();
    auto button = factory->createButton();
    window->draw();
    button->draw();
    factory = std::make_unique<VulkanFactory>();
    auto shader = factory->createShader();
    shader->compile();
}