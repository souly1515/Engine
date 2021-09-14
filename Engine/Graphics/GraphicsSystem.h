#pragma once


class GraphicsSystem
{
protected:
	static GraphicsSystem* m_gs;
public:
	virtual void UpdateBegin() = 0;

	virtual void UpdateEnd(unsigned int flags) = 0;

	static GraphicsSystem* GetInstance();
	static void Exit();
};

