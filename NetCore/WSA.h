#pragma once
class WSA
{
public:
	virtual ~WSA();

	static void init() { s_instance = new WSA(); }
	static void destroy() { delete s_instance; }
	static WSA* get() { return s_instance; }

private:
	WSA();

	static WSA* s_instance;
};

