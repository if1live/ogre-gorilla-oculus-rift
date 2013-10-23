// Ŭnicode please 
#pragma once

namespace kuuko {;

template<typename T>
void safeDelete(T* &ptr) 
{
	if(ptr != nullptr) {
		delete(ptr);
		ptr = nullptr;
	}
}

template<typename T>
void safeDeleteArray(T* &ptr) 
{
	if(ptr != nullptr) {
		delete[](ptr);
		ptr = nullptr;     
	}
}

template<typename T>
void safeDeleteWithNullCheck(T* &ptr) 
{
	OVR_ASSERT(ptr != nullptr);
	delete(ptr);
	ptr = nullptr;
}

template<typename T>
void safeDeleteArrayWithNullCheck(T* &ptr) 
{
	OVR_ASSERT(ptr != nullptr);
	delete[](ptr);
	ptr = nullptr;
}

template<typename T, int N>
int getArraySize(T (&arr)[N]) 
{
	return N;
}
}   // namespace kuuko