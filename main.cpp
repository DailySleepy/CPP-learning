#include <string>
#include <iostream>
#include <vector>
#include <functional>
#include <chrono>

using namespace std;

namespace _array
{
	void main()
	{
		cout << boolalpha << is_same_v<int[1], int[2]> << endl; // ����������, ������С
	}
}

namespace _lambda
{
	int g = 1;
	void main()
	{
		struct X : decltype([] { cout << "666" << endl; }) { }; // lambda ��һ���� (��������)
		X x;
		x();

		//auto l1 = [num = 1]() { num++; }; // ERROR: lambda::operator() Ĭ���� const ����
		auto l1 = [num = 1] mutable { num++; };

		int val = 0;
		auto l2 = [&val]() { val++; }; // const �����п����޸����õĲ���

		void (*p1)() = []() {}; // �޲���ʱ, lambda ���Ա���Ϊ��������
		auto p4 = +[] {}; // '+' ������ lambda ת��Ϊ����ָ��, ������Ϊ�� (�����СΪ1)
		cout << sizeof(p1) << " " << sizeof(p4) << endl;

		auto p2 = [](auto n) { return n + 1; };
		auto p3 = []<typename T>(T n) { return n + 1; };
		cout << p2(1) << " " << p3("abc") << endl;
		// auto g1 = [](auto a) { &y; };
		// ERROR: ��Ȼ������δ�����ODR-use, 
		// ��������ģ������׶μ�鵼���ڴ�ʱ���������һ����, ֱ��ģ�屻ʵ����

		const int a = 1;
		constexpr int b = 1;
		static int c = 1;
		int d = 1;
		//const float f = 1; // ERROR: const float ���ܱ���Ϊ constexpr
		constexpr float f = 1;
		auto p5 = []()
		{
			a; b; f; // ��������֪, ����ֱ���滻, ���� ODR-use
			//d; &a; &b; // ERROR: ODR-use �������뱻�����Ա��� lambda �ıհ���
			c; &c; g; &g; // lambda ����ֱ�ӷ���"ȫ�ֱ���"��"����������еľֲ���̬����"
		};

		const int y = 1;
		auto g1 = [] { y; };
		auto g2 = [=] { y; }; // ��ֵ���ʽ, ������ODR-use, ������ʾָ������, �ʲ���
		auto g3 = [=] { sizeof y; }; // ����ֵ���ʽ, ��������ODR-use, �ʲ�����
		cout << sizeof g1 << " " << sizeof g2 << " " << sizeof g3 << '\n'; // 1, 4, 1
	}
}

namespace _noexcept
{
	void f() noexcept
	{
		cout << "f is called" << endl;
	}

	void main()
	{
		cout << boolalpha << noexcept(f()) << endl;
	}
}

namespace user_defined_literals
{
	auto operator""_km(long double d)
	{
		using namespace std::chrono_literals;
		return d * 1000;
	}

	void main()
	{
		auto f = 1.f;

		auto dis = 1.5_km; cout << dis << endl;

		auto str = "sss"sv;

		auto time = 50h;
	}
}

namespace member_pointer
{
	void test();
	struct X
	{
		int i = 0;
		void f() { cout << "no para" << endl; }
		void f(int a) { i = a; cout << "int para: " << a << endl; }
		void print() { cout << "i: " << i << endl; }
	};

	void main()
	{
		X x;
		void (X:: * fp)(int) = &X::f;
		auto lambda = [](void(X::* fp)(int), X& x, int a) { (x.*fp)(a); };

		(x.*fp)(1);
		lambda(fp, x, 2);

		auto b0 = bind(lambda, fp, ref(x), 3); b0();
		auto b1 = bind(fp, ref(x), 4); b1();

		invoke(lambda, fp, ref(x), 5);
		invoke(fp, ref(x), 6);

		x.print();
		invoke(&X::i, x) = 7;
		x.print();

		cout << "test: " << endl;
		test();
	}

	void test()
	{
		#define flag true

		struct X
		{
			int i = 0;
			void inc() { i++; }
			void print() { cout << "i: " << i << endl; }
		};

		X x;
		x.print();
		if (!flag)
		{
			std::invoke(&X::inc, x);
			x.print();
			std::invoke(&X::inc, &x);
			x.print();
			std::invoke(&X::inc, std::ref(x));
			x.print();
			// 0 1 2 3
			//invoke��������, Arg&&ͨ������, ����x, &x, ref(x)�Ľ��һ��(�������ݳ�Աָ��ͳ�Ա����ָ��)
		}
		else
		{
			auto b1 = bind(&X::inc, x); b1();
			x.print();
			auto b2 = bind(&X::inc, &x); b2();
			x.print();
			auto b3 = bind(&X::inc, ref(x)); b3();
			x.print();
			// 0 0 1 2
			//bind��������������, ��Ҫ��ʹ��decay_t���˻�����(��thread��ͬ��Ҫ��), ���Դ�ֵx�����i����Ӱ��
		}
	}
};

namespace default_parameter
{
	struct B
	{
		virtual void f(int a = 666) { cout << "B: " << a << endl; }
		// virtual �ṩ����ʱ��̬, ʵ�ֺ�����д
	};
	struct D : B
	{
		void f(int a = 0) override { cout << "D: " << a << endl; }
	};

	struct X
	{
		//const int a = 0; // ������ΪĬ�ϲ���, ��Ϊ���뿪���������ʱ const ������ʧЧ, ����Ҫstatic
		//constexpr int a = 0; // ERROR: �� static ����������ʵ��, ���� constexpr ����ì��, ����Ҫstatic
		//static int a = 0; // ERROR: static ���������� inline �� const/constexpr
		inline static int a = 0; // inline �����֮ǰ�İ汾�б���������� static �������ж�����鷳
		const static int aa = 1;
		void f(int b = sizeof a + aa) { cout << b << endl; } // Ĭ�ϲ������������ھ�̬�洢��, ��static
	};

	void main()
	{
		unique_ptr<B> p = make_unique<D>();
		p->f();
		// Ĭ�ϲ���: ����ʱ��̬��, ����p��B*����, Ĭ�ϲ������� B:f()
		// ���ú���: ����ʱ��̬��, ����pʵ��ָ��Ķ�����D, �ʵ��� D:f()

		X x;
		x.f();
	}

};

namespace _forward
{
	template <class _Ty>
	constexpr _Ty&& _forward_(remove_reference_t<_Ty>& _Arg) noexcept
		// ���ȱ����� & _Arg, ��ֹ����
		// ����� remove_reference_t ��Ϊ��ʵ�������۵�, ��Ϊ�����۵�ֻ�������Ƶ�ʱ���� (����������, using)
		// _Arg ������ remove_reference_t<_Ty>& ����ȷ���Ĺ��̲��������Ƶ�
		// �����Ƶ�������ģ����� _Ty �ϣ�_Arg �������������� _Ty �Ƶ�����Ĳ���
	{
		return static_cast<_Ty&&>(_Arg); // �����۵�
	}

	void f(const string& s) { cout << "f��ֵ: " << s << endl; }
	void f(string&& s) { cout << "f��ֵ: " << s << endl; }
	template<typename T>
	void g(T&& arg) // ��������, �����۵�
	{
		//f(arg); // ��Զ�� f��ֵ
		f(forward<T>(arg));
		// ����ת��: ��ת��������ͬʱ��������ԭʼ����ֵ/��ֵ������ֵ���(value category)����
		// û������ת��:
		// ʹ�� T ���²���Ҫ�Ŀ���, ʹ�� T& ���޷�������ֵ
		// Ψһ�ķ�����Ϊ��ֵ��ֵ�����������, ������ֵ��Ȼֻ���� const& ������
	}
	void main(void)
	{
		string&& s("Hello");
		f(s); // ���� f��ֵ, ��������ֵ���ñ�����һ����ֵ
		f(dynamic_cast<string&&>(s)); // ���� f��ֵ  ps: dynamic_cast ���˶�̬��, �����Դ���ֵ����ֵ����
		
		// �� g() ��������ת������ȷ����һ�� f()
		g(s);
		g(move(s));
	}
}

namespace _auto
{
	void main()
	{
		int a = 1;
		auto&& b = ref(a).get();
		b++;
		cout << a << endl;
		cout << b << endl;

		int x = 0;
		auto&& v = cref(x).get();
	}
}

int main()
{
	_forward::main();
}