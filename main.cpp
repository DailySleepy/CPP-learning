#include <iostream>
#include <vector>
#include <functional>
#include <numeric>
#include <chrono>
#include <array>
#include <ranges>
#include <algorithm>
#include <string>
//import std;

using namespace std;

namespace _decltype
{
	void main()
	{
		int x = 1;
		int a[1] = { 1 };
		// �����ı�ʶ��:
		using T1 = decltype(x); // T
		// �ǵ����ı�ʶ��:
		// prv -> T
		using T2 = decltype(1);
		using T8 = decltype(&x);
		using T7 = decltype(string("***"));
		// lv -> T&
		using T5 = decltype(("***"));
		using T3 = decltype(a[0]);
		using T4 = decltype((x));
		// xv -> T&&
		using T6 = decltype(move(x));

		// decltype(auto) ��ȫ��������
		auto g = [] -> const int&& { return 0; };
		auto f = [&] -> decltype(auto) { return g(); };
		decltype(auto) v = f();
		auto vv = f();
		decltype(auto) s = "abc";
		auto ss = "abc";

		//decltype(auto) shaders = { "basic.vert", "pbr.vert" };
		// ERROR: �������б���û��һ�����Ա� decltype �Ƶ��Ķ�������
		auto shaders1 = { "basic.vert", "pbr.vert" };
		const char* shaders2[] = { "basic.vert", "pbr.vert" };
		array shaders3 = { "basic.vert", "pbr.vert" };
	}
}

namespace _array
{
	struct FArray
	{
		~FArray() = default;
		int size;
		char data[];
		// ��������, ���ֶ����ݺ� struct ͷ���������ڴ��Ͻ�������
		// �������� vector һ���������������ڴ����
	};
	void main()
	{
		int a[5]{ 1 }; // ֻ��ʼ����һ��Ԫ��Ϊ1, ����Ϊ0
		cout << a[1] << endl;

		cout << boolalpha << is_same_v<int[1], int[2]> << endl; // ����������, ������С
		// ֻ�ڴ���, ���ʽ(����sizeof, &)���˻�Ϊָ��

		// vector<int[2]> v;
		// v.push_back({ 1, 2 }); 
		// ERROR: ����Ԫ����Ҫ���ƶ��ɿ���, ��ԭʼ���鲻����

		vector<array<int, 2>> v;
		v.push_back({ 1, 2 });
		// v.emplace_back(1, 2);
		// ERROR: emplace �����ڹ��캯��, ������������ʵ�Ǿۺϳ�ʼ��{}, �ⲻ�����ڹ��캯��

		// ָ�����������ָ�루����Ϊ int (*)[5]����������ָ����Ԫ�ص�ָ�� int*
		auto p1 = &a;
		// int*
		auto p0 = a;
		auto p2 = &a[0];
		auto p3 = a + 1;

		int n = 10;
		{
			cout << "malloc:" << endl;
			auto p = (FArray*)malloc(sizeof(FArray) + n * sizeof(char));
			p->size = n;
			strcpy_s(p->data, 10, "123456789");
			cout << p->data << endl;
			free(p);
			// ��Ϊ FArray �� Plain Old Data(POD) ����, ���Կ���ʹ�ü򵥵� malloc / free
		}
		{
			cout << "new:" << endl;
			void* mem = ::operator new(sizeof(FArray) + n * sizeof(char)); // ����ԭʼ�ڴ棬�����ù��캯����
			auto* p = new (mem) FArray; // ʹ�ö�λ new ���ѷ�����ڴ� mem �Ϲ��� FArray ����, �������ڴ����
			p->size = n;
			strcpy_s(p->data, 10, "123456789");
			cout << p->data << endl;
			p->~FArray(); // ��� FArray ������Ҫ�������Դ, ������ֶ�������������
			::operator delete(mem); // �ͷ�ԭʼ�ڴ�
		}

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
	// ���һ�� noexcept �����׳��쳣��������������� std::terminate() ��ǿ����ֹ
{
	void f() noexcept // ��ͨ����Ĭ���� has except ��, noexcept ��ʽָ���������
	{
		cout << "f is called" << endl;
	}

	struct C
	{
		int* p = nullptr;
		C() : p(new int(1)) {}
		~C() { if (p) delete p; }
		// ��������Ĭ���� noexcept ��, ���� noexcept(false)
	};

	void main()
	{
		// noexcept ����ֵ, ������ú���
		cout << boolalpha << noexcept(f()) << endl; // true
		C c; cout << boolalpha << noexcept(c.~C()) << endl; // true
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

namespace _move
{
	struct X
	{
		X() {}
		X(const X& o) {};
		X(X&& o) {};
	};
	X f()
	{
		X x;
		return x; // NRVO �� �ƶ�����
		// ��Ȼx����ֵ, ���Ǳ�����֪��x��������, ���Ի��ǽ�x��Ϊ����ʽ���ƶ�����, �Զ������ƶ�����
		// ��Ӧ��ʹ�� move(x), ����ƻ� NRVO
	}
	struct Y
	{
		X x;
		X get() { return move(x); }
		// �����x��һ����ֵ, �Ҳ�����������, ���Բ���һ����ʽ���ƶ�����, ��Ҫ�ֶ� move �������ƶ�����
	};

}

namespace _forward
	// ����ת��: ��ת��������ͬʱ������������ֵ���
	// ����:
	// 1.�������ú�ģ�������Ƶ�: ����ֵ���, ����ֵ����Ϊ��ֵ����, T�Ƶ�ΪU&������ֵ����Ϊ��ֵ����, T�Ƶ�Ϊֵ����U
	// 2.forward ��������ת��: ����ֵ���, ֻ����ֵ����תΪ��ֵ, ����תΪ��ֵ (move ������ת��Ϊ��ֵ)
	// ���ʹ��: ��������/�ƶ����濽��, ����һ����������������
	// ʹ�� T ���²���Ҫ�Ŀ���, ʹ�� T& ���޷�������ֵ
	// C++11ǰ, Ψһ�ķ�����Ϊ��ֵ��ֵ�����������, ������ֵ��Ȼֻ���� const& ������
	// C++11��(��������ת���������), ������ֵ���ò�����ֵ, �� move �����ƶ�, ������Ȼ��Ҫ����������
{
	template <class T>
	constexpr T&& __forward(remove_reference_t<T>& arg) noexcept
		// ���ȱ����� & arg, ��ֹ����
		// ����� remove_reference_t ������һ�����Ƶ�������
		// ��ֹ������ͨ���������Զ��Ƶ�T, ��������ת�����ǲ�Ӧ�ñ������, ��Ϊ��ʱ�����Ѿ���ʧ��ֵ���
		// ����ǿ���û��ڵ��� forward ʱͨ�� <> ��ָ��ģ����� T, ��˳������ֵ�����һ��Ϣ
		// ��Ȼȥ�� remove_reference Ҳ�����ֶ�ʹ�� <>, �����ⲻ��ǿ�Ƶ�, ��������
		// �ҵ�����ת������������������ͷ�����ͻʱ (�����Ϊ��ֵ����, ����TΪ��ֵ����) �ᱨ��
	{
		return static_cast<T&&>(arg); // �����۵�
		// ���������ģ����� T�������� arg �����ֵ���������ת��������
	}

	template <class T>
	constexpr T&& __forward(remove_reference_t<T>&& _Arg) noexcept // forward ������ֵ����������
	{
		cout << "r forward" << endl;
		return static_cast<T&&>(_Arg);
	}

	void f(const string& s) { cout << "f��ֵ: " << s << endl; }
	void f(string&& s) { cout << "f��ֵ: " << s << endl; }
	template<typename T>
	void g(T&& arg) // ��������, �����۵�
	{
		//f(arg); 
		// ERROR: ��Զ�� f��ֵ, ��Ϊ��������ֵ���ñ�����һ����ֵ
		// ���ؾ���ͬʱ�����˲���������(��ֵ����)��ֵ���(��ֵ), ֵ������ȼ�����
		f(__forward<T>(arg)); // ������ת������ȷ����һ�� f()
	}

	void main()
	{
		string s("Hello");
		g(s);
		g(move(s));

		string&& ss = move(s);
		__forward<string&&>(ss);
	}
}

namespace type_traits
{
	// 1.����ж���������
	template<typename T>
	struct is_int_strict : false_type {};
	template<>
	struct is_int_strict<int> : true_type {};
	template<typename T>
	struct is_int : is_int_strict<remove_cv_t<remove_reference_t<T>>> {};
	// ���Ƴ�����, ���Ƴ� cv: �� & ���� const, ������ const ���� &
	// Ҳ����ֱ���� remove_cvref_t
	template<typename T>
	constexpr bool my_is_int_v{ is_int<T>::value };

	// 2.ʹ������������ѡ����ģ��
	class IMeasurable
	{
	public:
		virtual double length() const = 0;
	};
	class Vec : public IMeasurable
	{
	public:
		int x, y;
		Vec(int x, int y) : x(x), y(y) {}
		double length() const { return sqrt(x * x + y * y); }
	};
	template<typename T>
	double length(T&& t)
	{
		if constexpr (is_arithmetic_v<T>)
			return abs(t);
		else if constexpr (is_base_of_v<IMeasurable, remove_reference_t<T>>)
			// �������Ͳ����� is_base_of �жϣ��������Ƴ�����
			return t.length();
		return 0;
	}

	void main()
	{
		cout << boolalpha << my_is_int_v<const int&> << endl;

		cout << length(2) << endl;
		cout << length(Vec(3, 4)) << endl;
		const auto v = Vec(3, 4);
		cout << length(v) << endl;
	}
}

namespace SFINAE
{
	namespace test1
	{
		class SomeObj1
		{
		public:
			void Dump2File() const
			{
				std::cout << "dump this object to file" << std::endl;
			}
		};
		class SomeObj2 {};

		template<class T>
		auto DumpObj(const T& t) -> decltype(t.Dump2File())
			// ʹ�� decltype ������ SFINAE
			// ��� T û�иó�Ա������ʽ��Ч��decltype �޷��Ƶ�����, ���Ը�ģ��
		{
			t.Dump2File();
		}
		void DumpObj(...)
		{
			std::cout << "the object must have a member function Dump2File" << std::endl;
		}
		void main()
		{
			SomeObj1 obj1;
			DumpObj(obj1);
			SomeObj2 obj2;
			DumpObj(obj2);
		}
	}

	namespace test2
	{
		template<typename, typename = void>
		struct has_type_member : std::false_type {};

		template<typename T>
		struct has_type_member<T, std::void_t<typename T::type>> : std::true_type {};
		// �� void_t ������������ʵ����Ϊ void, ���ʵ����ʧ��, ���� SFINAE

		struct A { using type = int; };
		struct B {};

		void main()
		{
			std::cout << boolalpha << has_type_member<A>::value << std::endl;
			std::cout << boolalpha << has_type_member<B>::value << std::endl;
		}
	}

	void main()
	{
		cout << "test1:" << endl;
		test1::main();
		cout << "test2:" << endl;
		test2::main();
	}
}

namespace CRTP
{
	template<typename T>
	struct Logger
	{
		void printLog(const std::string& message)
		{
			std::cout << "[ID:" << static_cast<T*>(this)->getID() << "] ";
			static_cast<T*>(this)->logMessage(message);
		}
	};

	// �̳����Լ�Ϊģ������Ļ���, ���ͨ�õķ��� printLog
	struct Player : Logger<Player>
	{
		int getID() const { return 101; }
		void logMessage(const std::string& message)
		{
			std::cout << "Player: " << message << std::endl;
		}
	};
	struct Enemy : Logger<Enemy>
	{
		int getID() const { return 202; }
		void logMessage(const std::string& message)
		{
			std::cout << "Enemy: " << message << std::endl;
		}
	};

	void main()
	{
		Player player;
		Enemy enemy;

		player.printLog("spawned");
		enemy.printLog("defeated");

		// vector<unique_ptr<Logger<Player>>> entities;
		// vector<unique_ptr<Logger<Enemy>>>  entities;
		// �������޷������߷���ͬһ��������, ��Ϊ Logger<Player> �� Logger<Enemy> �ǲ�ͬ����
		// ������������� Logger �̳��� ILoggerBase, ��Ȼ���������麯�� virtual printLog, ���Ǽ������麯���ĵ��ô���
		// ��ͳ�麯���ӿ�: getID + logMessage �����麯�� --> CRTP: printLog һ���麯�� + (getID + logMessage) ���ξ�̬����
	}
}

namespace _concept
{
	template<typename T>
	concept AddAble = // �� requires ������ Concept
		requires(T a) // requires ���Ƿ���ֵΪ bool �ĳ������ʽ
	{
		{ a + a } -> same_as<T>;
		requires is_arithmetic_v<T>; // Ƕ��ʹ��
	};

	template<typename T>
	concept Divisible = // ֱ���ò����������ʽ������ Concept
		is_arithmetic_v<T>;

	template<typename T>
	auto add1(T a, T b) requires AddAble<T>;

	template<AddAble T> // ������һ�� Concept �ļ�Լ��
	auto add2(T a, T b);

	template<typename T>
		requires requires(T a, T b) { { a + b } -> same_as<T>; } // ʹ������ requires
	auto add3(T a, T b);

	// ʡ�� template<typename T, typename U> Ҳ����ʹ�ò�ͬ���͵Ĳ���
	auto add4(AddAble auto a, AddAble auto b); // �� Concept ֻ��Ҫһ������ʱ, ����ֱ������ auto

	template<typename T>
	auto mean(T a, T b) requires AddAble<T>&& Divisible<T>; // ʹ�ö�� Concept

	template<typename T> // Concept ������ģ��
		requires integral<T>&& AddAble<T>
	struct Pair
	{
		T a, b;
		Pair(T a, T b) : a(a), b(b) {};
	};

	template<integral auto N> // Concept ���� NTTP��������ģ�������
	struct Array
	{
		int arr[N];
	};

	void main()
	{
		Pair p(1, 2);
		Array<3> a;
	}
}

import MyModule;
namespace _module
{
	void main()
	{
		cout << moduleVariable << endl;
		ModuleClass mc;
		ModuleFunc();
		SubModuleFunc();
		OtherModuleFunc();
	}
}

namespace range
{
	void main()
	{
		struct Person { std::string name; int age; };
		vector<Person> people = { {"Alice", 30}, {"Bob", 25}, {"Charlie", 28} };
		ranges::sort(people, less{}, &Person::age);
		cout << "ranges" << endl;
	}
}

namespace test
{
	void main()
	{
	}
}

int main()
{
	_noexcept::main();
}