#pragma once

#include "Atlas/Core/Font.h"

#include <freetype/freetype.h>
#include <unordered_map>

namespace Atlas {
class FontLibrary {
  public:
	static void init();

	static const FT_Library& getLibrary() { return s_library; }

	static void add(std::string name, const std::shared_ptr<Font>& font);
	static void add(const std::shared_ptr<Font>& font);

	static std::shared_ptr<Font> load(const std::string& name, const std::string& filepath);
	static std::shared_ptr<Font> get(const std::string& name);

	static bool exists(const std::string& name);

  private:
	static FT_Library s_library;
	static std::unordered_map<std::string, std::shared_ptr<Font>> s_fonts;
};
}  // namespace Atlas