#include <eiffel/ProjectInfo.h>
#include <utility/ConsoleArguments.h>
#include <iostream>

bool checkIfDifferent(std::filesystem::path const& from,
	std::filesystem::path const& to)
{
	if (!std::filesystem::exists(to)) return true;
	
	auto from_modified = std::filesystem::last_write_time(from);
	auto to_modified = std::filesystem::last_write_time(to);

	return from_modified != to_modified;
}

void copyIfDifferent(std::filesystem::path const& from,
	std::filesystem::path const& to)
{
	if (std::filesystem::is_directory(from))
	{
		if (!std::filesystem::exists(to))
			std::filesystem::create_directories(to);
	}
	else if (checkIfDifferent(from, to))
	{
		//std::filesystem::create_directories(to.parent_path());
		if (std::filesystem::exists(to))
			std::filesystem::remove(to);
		std::filesystem::copy(from, to);
	}
}

void copyAssets(std::filesystem::path asset_directory,
	std::filesystem::path runtime_directory)
{
	if (!std::filesystem::is_directory(asset_directory)) return;
	std::cout << "Copying assets..." << std::endl;

	//for (auto file : std::filesystem::recursive_directory_iterator(asset_directory))
	//{
	//	auto file_path = file.path();
	//	auto file_name = file_path.filename().string();

	//	copyIfDifferent(file_path, runtime_directory / file_name);
	//}

	auto copy_flags = std::filesystem::copy_options::update_existing | std::filesystem::copy_options::recursive;
	std::filesystem::copy(asset_directory, runtime_directory, copy_flags);
}

int main_impl(int num_args, const char* args[])
{
	std::cout << "Running EiffelPostBuild..." << std::endl;

	auto arguments = ConsoleArguments(num_args, args);
	auto project_id = arguments.getString("project");
	auto runtime_directory = std::filesystem::path(arguments.getString("runtime"));
	auto exe_directory = std::filesystem::path(arguments.getString("exe_dir"));

	auto project_info = eiffel::getProjectInfo(project_id);
	auto exe_name = project_info.name + ".exe";

	std::filesystem::create_directory(runtime_directory);

	copyAssets(project_info.paths.assets_directory, runtime_directory);

	std::cout << "Copying exe..." << std::endl;
	copyIfDifferent(exe_directory / exe_name, runtime_directory / exe_name);

	std::cout << "Done" << std::endl;
	return 0;
}

int main(int num_args, const char* args[])
{

	try
	{
		return main_impl(num_args, args);
	}
	catch (std::exception const& e)
	{
		std::cout << "Error: " << e.what() << std::endl;
		return 1;
	}
}
