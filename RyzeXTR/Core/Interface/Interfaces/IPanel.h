#pragma once

class IPanel {
public:
	void SetKeyBoardInputEnabled(unsigned int nVGUIPanel, bool bState)
	{
		util::CallVFunc<void>(this, 31U, nVGUIPanel, bState);
	}

	void SetMouseInputEnabled(unsigned int nVGUIPanel, bool bState)
	{
		util::CallVFunc<void>(this, 32U, nVGUIPanel, bState);
	}

	const char* GetName(unsigned int nVGUIPanel)
	{
		return util::CallVFunc<const char*>(this, 36U, nVGUIPanel);
	}

	const char* GetNameClass(unsigned int nVGUIPanel)
	{
		return util::CallVFunc<const char*>(this, 37U, nVGUIPanel);
	}
};