#pragma once

namespace Atlas {

class Platform {
  public:
	static inline std::string getResourcesPath() { return s_instance->getResourcesPathImpl(); }
	static inline std::string getExecutablePath() { return s_instance->getExecutablePathImpl(); }
	static inline std::string openFileDialog(const std::string& filter) { return s_instance->openFileDialogImpl(filter); }
	static inline std::string saveFileDialog(const std::string& filter) { return s_instance->saveFileDialogImpl(filter); }

  protected:
	virtual std::string getResourcesPathImpl()	= 0;
	virtual std::string getExecutablePathImpl() = 0;

	virtual std::string openFileDialogImpl(const std::string& filter) = 0;
	virtual std::string saveFileDialogImpl(const std::string& filter) = 0;

  private:
	static Platform* s_instance;
};
}  // namespace Atlas