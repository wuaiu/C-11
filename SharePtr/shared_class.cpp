#include<memory>
#include<iostream>
#include<vector>
#include<string>
class ClassA{
public:

	ClassA(int x)
	{
		m_x = x;
	}
	ClassA(std::initializer_list<std::string> il) :
		data(std::make_shared<std::vector<std::string> >(il))
	{
		int x = 1;
	}
	//initializer_list不必放在最后
	void print(std::initializer_list<std::string> il,int x)
	{
		for (auto iter = il.begin(); iter != il.end(); ++iter)
		{
			std::cout << (*iter);
		}
	}

private:
	std::shared_ptr<std::vector<std::string> > data;
	int m_x;
};
int main() {
	ClassA a = { "aaa", "bbb", "ccc" };
	a.print({ "aaa", "bbb", "ccc" },1);
	getchar();
	return 0;
}