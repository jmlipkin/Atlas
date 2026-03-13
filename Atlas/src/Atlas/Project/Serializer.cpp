#include "atpch.h"
#include "Atlas/Project/Serializer.h"

#include "Atlas/Project/JSONSerializer.h"

namespace Atlas {

SerializerAPI* Serializer::s_serializerAPI = nullptr;

void Serializer::init() {
    s_serializerAPI = new JSONSerializer();
}

}  // namespace Atlas