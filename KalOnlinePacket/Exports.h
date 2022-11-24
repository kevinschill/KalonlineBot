#pragma once
class IExports
{
public:
	static FARPROC p[15 * 4];
	static volatile unsigned long _p;	// Needs to be volatile to ensure thread-safety
	virtual void Initialize();
private:

	HINSTANCE hL = 0;
};


static IExports* Exports = new IExports;