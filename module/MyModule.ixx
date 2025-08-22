module;

#include <iostream>
#define PI 1234
using namespace std;

export module MyModule; // ���ϵ���������ȫ��ģ��Ƭ�Σ�Ҳ���ǲ������κ�ģ��
// ���µ�����ʹ�� export ��ǿ��Ա��ⲿʹ��

export import :SubModule; // �ȵ����ٵ�����ģ��

export import OtherModule; // Ҳ���Ե�������ģ��

export string moduleVariable = "this is a module variable";

export struct ModuleClass
{
	ModuleClass()
	{
		cout << "this is a module class" << endl;
	}
};

export void ModuleFunc()
{
	cout << "this is a module func" << endl;
	cout << "my PI is " << PI << endl;
}

void PrivateFunc()
{
	cout << "this is a private func" << endl;
}