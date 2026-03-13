#pragma once

#include "Atlas/Core/Platform.h"

namespace Atlas {
class MacOSPlatform : public Platform {
  protected:
	virtual std::string getResourcesPathImpl()	override;
	virtual std::string getExecutablePathImpl() override;

	virtual std::string openFileDialogImpl(const std::string& filter) override;
	virtual std::string saveFileDialogImpl(const std::string& filter) override;
};
}  // namespace Atlas