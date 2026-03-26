#pragma once

#include <string>
#include <functional>
namespace Atlas {

class Platform {
  public:
	using BuildOutputCallback	= std::function<void(const std::string&)>;
	using BuildCompleteCallback = std::function<void(bool)>;

	static inline std::string getResourcesPath() { return s_instance->getResourcesPathImpl(); }
	static inline std::string getExecutablePath() { return s_instance->getExecutablePathImpl(); }
	static inline std::string getAppSupportPath() { return s_instance->getAppSupportPathImpl(); }

	static inline std::string openFileDialog(const std::string& filter) { return s_instance->openFileDialogImpl(filter); }
	static inline std::string saveFileDialog(const std::string& filter, const std::string& defaultDir = "") { return s_instance->saveFileDialogImpl(filter, defaultDir); }

	static inline std::vector<std::string> getFileList(const std::string& directory, const std::string& filter) { return s_instance->getFileListImpl(directory, filter); }

	static inline int  showConfirmDialog(const std::string& message, const std::string& confirm, const std::string& deny, const std::string& cancel) { return s_instance->showConfirmDialogImpl(message, confirm, deny, cancel); }
	static inline void buildScriptLibrary(const std::string& buildDir, const std::string& target, BuildOutputCallback onOutput, BuildCompleteCallback onComplete) { s_instance->buildScriptLibraryImpl(buildDir, target, onOutput, onComplete); }

	static inline void openFile(const std::string& filepath) { s_instance->openFileImpl(filepath); }

  protected:
	virtual std::string getResourcesPathImpl()	= 0;
	virtual std::string getExecutablePathImpl() = 0;
	virtual std::string getAppSupportPathImpl() = 0;

	virtual std::string openFileDialogImpl(const std::string& filter)								 = 0;
	virtual std::string saveFileDialogImpl(const std::string& filter, const std::string& defaultDir) = 0;

	virtual std::vector<std::string> getFileListImpl(const std::string& directory, const std::string& filter) = 0;

	virtual int	 showConfirmDialogImpl(const std::string& message, const std::string& confirm, const std::string& deny, const std::string& cancel)				= 0;
	virtual void buildScriptLibraryImpl(const std::string& buildDir, const std::string& target, BuildOutputCallback onOutput, BuildCompleteCallback onComplete) = 0;

	virtual void openFileImpl(const std::string& filepath) = 0;

  private:
	static Platform* s_instance;
};
}  // namespace Atlas