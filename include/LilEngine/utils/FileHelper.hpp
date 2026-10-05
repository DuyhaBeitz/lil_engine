#pragma once
#include <string>

void CopyFile(std::string source, std::string dest);
void CopyAsset(std::string source);
std::string ReadFile(std::string source);
void WriteFile(std::string content, std::string dest);