#include "atpch.h"
#include "Atlas/Project/Serializer.h"

#include "Atlas/Project/JSONSerializer.h"

namespace Atlas {

std::shared_ptr<Serializer> Serializer::create(const std::string& filepath, SerializerMode mode, SerializerFormat format) {
    switch(format) {
        case SerializerFormat::JSON: {
            return std::make_shared<JSONSerializer>(filepath, mode);
        }
        default: {
            AT_CORE_ASSERT(false, "Invalid Serializer format specified");
            return nullptr;
        }

    }
}

}  // namespace Atlas