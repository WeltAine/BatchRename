#include <iostream>
#include <filesystem>
#include <vector>
#include <string>

#include <io.h>
#include <fcntl.h>

//控制台设置
void SetupConsole() {
	//设置控制台为宽字符模式
	_setmode(_fileno(stdout), _O_U16TEXT);
	_setmode(_fileno(stdin), _O_U16TEXT);

	//设置控制台字体
}


std::vector<std::wstring> GetFilesInDirectory(const std::wstring& directoryPath) {

	std::vector<std::wstring> files;

	try {
		for (const auto& entry : std::filesystem::directory_iterator(directoryPath)) {
			if (entry.is_regular_file()) {
				files.push_back(entry.path().filename().wstring());

			}
		}
	}
	catch(const std::filesystem::filesystem_error& e) {
		std::wcerr << L"错误：读取目录失败 (" << e.what() << L")" << std::endl;
	}

	return files;

}

std::vector<std::wstring> GetSubfolderNames(const std::wstring& folderPath) {

	std::vector<std::wstring> subfolders;

	try {
		for (const auto& entry : std::filesystem::directory_iterator(folderPath)) {
			if (entry.is_directory()) {
				subfolders.push_back(entry.path().filename().wstring());
			}
		}
	}
	catch (const std::filesystem::filesystem_error& e) {
		std::wcerr << L"错误：" << e.what() << std::endl;
	}

	return subfolders;
}



//自动解析，并批量重命名
std::vector<std::wstring> BatchRename(const std::wstring& directoryPath, std::wstring& sonFile, int offset = 0) {

	int count = 0;

	int firstIndex = sonFile.find_first_of(L'(') == std::wstring::npos? sonFile.find_first_of(L'（') : sonFile.find_first_of(L'(');
	firstIndex++;
	int lastIndex = sonFile.find_first_of(L')') == std::wstring::npos? sonFile.find_first_of(L'）') : sonFile.find_first_of(L')');
	lastIndex--;

	std::vector<std::wstring> newFiles;
	newFiles.clear();

	//获取编号
	std::wstring serialNumber;
	serialNumber.clear();
	for (int i = firstIndex; i <= lastIndex; i++) {

		if(sonFile.at(i) != L'.')
		serialNumber += sonFile.at(i);
	}

	std::wstring sonDirectoryPath = directoryPath + L"\\" + sonFile;
	auto files = GetFilesInDirectory(sonDirectoryPath);

	for (auto file : files) {

		try {
			//重命名
			std::wstring oldPath = sonDirectoryPath + file;
			file.replace(offset, serialNumber.length(), serialNumber);
			std::wstring newPath = sonDirectoryPath + file;

			std::filesystem::rename(oldPath, newPath);
			newFiles.push_back(file);
		}
		catch (const std::filesystem::filesystem_error& e) {
			std::wcerr << L"重命名失败: " << e.what() << std::endl;
			return newFiles;
		}

	}
	return newFiles;

}


void ResetName() {

}


int main() {

	SetupConsole();
	bool end = false;

	while (end != true) {


		std::wcout << L"请输入文件夹路径（可包含日文）：";
		std::wstring folderPath;
		std::getline(std::wcin, folderPath);

		if (folderPath == L"E" || folderPath == L"e") {
			end == true;
			continue;
		}

		//auto files = GetFilesInDirectory(folderPath);
		auto subFolders = GetSubfolderNames(folderPath);
		std::wcout << L"\n找到 " << subFolders.size() << L" 个文件夹：" << std::endl;
		for (const auto& folder : subFolders) {
			std::wcout << L"  • " << folder << std::endl;
		}

		int offset = subFolders.size() < 10 ? 0 : 1;

		std::wcout << L"请输入Y以确认开始批量重命名，其余任意按钮重新选择批量重命名对象";
		std::wstring input;
		std::getline(std::wcin, input);

		if (input == L"Y" || input == L"y") {

			for (int i = 0; i < subFolders.size(); i++) {

				auto newFiles = BatchRename(folderPath, subFolders[i], offset);

				std::wcout << L"\n已修改 " << subFolders[i] << L"下" << newFiles.size() << L" 个文件：" << std::endl;
				for (const auto& file : newFiles) {
					std::wcout << L"  • " << file << std::endl;
				}


			}

			std::wcout << L"\n已完成批量重命名 ，输入E结束程序" << std::endl;


		}
		else {
			continue;
		}
	}


	std::wcout << L"\n操作完成，按回车键退出...";
	std::wcin.get();
	return 0;
}