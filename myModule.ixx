module;

#include <iostream>
#define PI 1234

export module myModule; // ���ϵ����ݶ��ǽ��ڸ��ļ��ɼ�

using namespace std;

export string moduleVariable = "this is a module variable";
export void moduleFunc() // ʹ�� export ��ǿ��Ա��ⲿʹ�õĺ����ͱ���
{
	cout << "this is a module" << endl;
	cout << "my PI is " << PI << endl;
}

void privateFunc()
{
	cout << "this is a private func" << endl;
}