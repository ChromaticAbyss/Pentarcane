#pragma once

#include <string>
#include <vector>

std::vector<std::string> LoadUnlocks(const std::string& file_name);

void SaveUnlocks(const std::string& file_name, const std::vector<std::string>& unlocks);
