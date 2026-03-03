#pragma once

namespace Atlas {

enum class ImageFormat {
    None = 0,
    R8,
    RGB8,
    RGBA8,
    RGBA32F
};

struct TextureSpecification {
    uint32_t width = 1;
    uint32_t height = 1;
    ImageFormat format = ImageFormat::RGBA8;
};

class Texture {
   public:
    virtual ~Texture() = default;

    virtual uint32_t getWidth() const = 0;
    virtual uint32_t getHeight() const = 0;

	virtual bool operator==(const Texture& other) const = 0;

	virtual std::string& getFilepath() = 0;
	virtual std::string getFilepath() const = 0;

    virtual void setData(void* data, uint32_t size) = 0;

    static std::shared_ptr<Texture> create(const std::string& filepath);
    static std::shared_ptr<Texture> create(uint32_t width, uint32_t height);
    static std::shared_ptr<Texture> create(TextureSpecification specs, void* data = nullptr);

    static uint32_t formatSize(ImageFormat);
};

}  // namespace Atlas