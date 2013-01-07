#pragma once

#define N_KEY 300

class KeyManager
{
public:
	KeyManager();
	void Update();
	
	void Enable(int key) { m_abEnable[key] = true; }

	const bool IsPressed(int key) const { return m_abNewValue[key] && !m_abLastValue[key]; }
	const bool IsReleased(int key) const { return !m_abNewValue[key] && m_abLastValue[key]; }
	const bool IsDown(int key) const { return m_abNewValue[key]; }
	const bool IsUp(int key) const { return !m_abNewValue[key]; }

private:
	bool m_abEnable[N_KEY];
	bool m_abLastValue[N_KEY];
	bool m_abNewValue[N_KEY];
};
