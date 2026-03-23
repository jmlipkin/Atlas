#pragma once

#include "Atlas/Core/Platform.h"

namespace Atlas {
class MacOSPlatform : public Platform {
  protected:
	virtual std::string getResourcesPathImpl() override;
	virtual std::string getExecutablePathImpl() override;
	virtual std::string getAppSupportPathImpl() override;

	virtual std::string openFileDialogImpl(const std::string& filter) override;
	virtual std::string saveFileDialogImpl(const std::string& filter, const std::string& defaultDir) override;
	virtual int			showConfirmDialogImpl(const std::string& message, const std::string& confirm, const std::string& deny, const std::string& cancel) override;

	virtual void buildScriptLibraryImpl(const std::string& buildDir, const std::string& target, BuildOutputCallback onOutput, BuildCompleteCallback onComplete) override;

	virtual void openFileImpl(const std::string& filepath) override;
};
}  // namespace Atlas