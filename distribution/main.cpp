#include <random>
#include<ctime>
#include<string>
#include<iostream>
int main() {
	//��������޷�������
	std::default_random_engine e(time(0));
	for (size_t i = 0; i < 10; ++i)
	{
		std::cout << e() << " ";
	}
	std::cout << std::endl;
	//��Χ����ֲ�����

	std::uniform_int_distribution<unsigned> u(0, 9);
	for (size_t i = 0; i < 10; ++i)
	{
		std::cout << u(e) << " ";
	}
	std::cout << std::endl;
	//��Χ����ֲ�ʵ��
	std::uniform_real_distribution<double> u1(0, 1);
	for (size_t i = 0; i < 10; ++i)
	{
		std::cout << u1(e) << std::endl;
	}

	//��̬�����ֲ�ʵ��
	std::normal_distribution<> n(4, 1.5);
	std::vector<unsigned> vals(9);
	for (size_t i = 0; i != 200; ++i)
	{
		unsigned v = lround(n(e));
		if (v < vals.size())
		{
			++vals[v];
		}
	}

	for (size_t j = 0; j != vals.size(); ++j)
	{
		std::cout << j << ":" << std::string(vals[j], '*') << std::endl;
	}
	//��Ŭ���ֲ�
	std::bernoulli_distribution b;
	e.seed(time(0));
	bool first = b(e);
	std::cout << (first ? "We go first" : "You get to go first") << std::endl;
	getchar();
	return 0;
}