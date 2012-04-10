
// フックの追加
#define INSTALL_HOOK(dll_name, func_name, hook_func, module_handle)	\
	ReplaceIATEntryInAllMods(	\
			  dll_name	\
			, ::GetProcAddress(GetModuleHandleA(dll_name), func_name)	\
			, (PROC)(hook_func)	\
			, module_handle	\
		);

#define UNINSTALL_HOOK(dll_name, func_name, hook_func, module_handle)	\
	ReplaceIATEntryInAllMods(	\
			  dll_name	\
			, (PROC)(hook_func)	\
			, ::GetProcAddress(GetModuleHandleA(dll_name), func_name)	\
			, module_handle	\
		);

#define INSTALL_HOOK_BY_ORDER(dll_name, func_order, hook_func, module_handle)	\
	INSTALL_HOOK(dll_name, (LPCSTR)func_order, hook_func, module_handle)
#define UNINSTALL_HOOK_BY_ORDER(dll_name, func_order, hook_func, module_handle)	\
	UNINSTALL_HOOK(dll_name, (LPCSTR)func_order, hook_func, module_handle)

// すべてのモジュールに対してAPIフックを行う関数
bool ReplaceIATEntryInAllMods(PCSTR pszModuleName, PROC pfnCurrent, PROC pfnNew, HMODULE hmodCaller = NULL);

