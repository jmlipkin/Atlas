#pragma once

#include <string>
namespace Atlas {

class Platform {
  public:
	static inline std::string getResourcesPath() { return s_instance->getResourcesPathImpl(); }
	static inline std::string getExecutablePath() { return s_instance->getExecutablePathImpl(); }
	static inline std::string getAppSupportPath() { return s_instance->getAppSupportPathImpl(); }
	static inline std::string openFileDialog(const std::string& filter) { return s_instance->openFileDialogImpl(filter); }
	static inline std::string saveFileDialog(const std::string& filter) { return s_instance->saveFileDialogImpl(filter); }
	static inline int		  showConfirmDialog(const std::string& message, const std::string& confirm, const std::string& deny, const std::string& cancel) { return s_instance->showConfirmDialogImpl(message, confirm, deny, cancel); }

  protected:
	virtual std::string getResourcesPathImpl()	= 0;
	virtual std::string getExecutablePathImpl() = 0;
	virtual std::string getAppSupportPathImpl() = 0;

	virtual std::string openFileDialogImpl(const std::string& filter) = 0;
	virtual std::string saveFileDialogImpl(const std::string& filter) = 0;
	virtual int showConfirmDialogImpl(const std::string& message, const std::string& confirm, const std::string& deny, const std::string& cancel) = 0;

  private:
	static Platform* s_instance;
};
}  // namespace Atlas